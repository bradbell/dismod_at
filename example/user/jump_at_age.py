# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_jump_at_age.py$$ $newlinech #$$
# $spell
# $$
#
# $section Case with a Jump in Rate at an Age$$
#
# $code
# $verbatim%
#	example/user/jump_at_age.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# true values used to simulate data
iota_20        = 1e-4
iota_100       = 1e-1
omega_0        = 2e-4
omega_100      = 2e-1
iota_age_list  = [ 0.0, 20.0, 21.0, 100.0 ]
omega_age_list = [ 0.0, 1.0, 5.0, 10.0, 20.0, 40.0, 80.0, 100.0 ]
data_age_list  = [ 0.0, 1.0, 5.0, 10.0, 20.0, 40.0, 80.0, 100.0 ]
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
test_program = 'example/user/jump_at_age.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
	usage  = 'python3 ' + test_program + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
#
# import dismod_at
sys.path.append( os.getcwd() + '/python' )
import dismod_at
#
# change into the build/example/user directory
distutils.dir_util.mkpath('build/example/user')
os.chdir('build/example/user')
# ------------------------------------------------------------------------
def constant_weight_fun(a, t) :
	return 1.0
#
def fun_zero(a, t) :
	return ('prior_zero', 'prior_none', 'prior_none')
#
def fun_omega_parent(a, t) :
	return ('prior_rate_parent', 'prior_difference', 'prior_difference')
#
def fun_iota_parent(a, t) :
	if a <= 20.0 :
		return ('prior_iota_20', 'prior_none', 'prior_none')
	elif a <= 21.0 :
		return ('prior_rate_parent', 'prior_none', 'prior_difference')
	else :
		return ('prior_rate_parent', 'prior_difference', 'prior_difference')
#
def iota_true(age) :
	if age <= 20.0 :
		return iota_20
	else :
		return iota_100
#
def omega_true(age) :
	slope = (omega_100 - omega_0) / (100. - 0.)
	return omega_0 + (age - 0.0) * slope
# ------------------------------------------------------------------------
def example_db (file_name) :
	import copy
	import dismod_at
	import math
	# ----------------------------------------------------------------------
	# age table (in age_list above)
	age_list = sorted( set( iota_age_list + omega_age_list ) )
	#
	# time table
	time_list   = [ 1995.0, 2015.0 ]
	#
	# integrand table
	integrand_dict = [
		{ 'name':'Sincidence',  'eta':1e-5 },
		{ 'name':'mtother',     'eta':1e-5 }
	]
	#
	# node table: world
	node_dict = [ { 'name':'world',         'parent':'' } ]
	#
	# weight table: The constant function 1.0 (one age and one time point)
	fun = constant_weight_fun
	weight_dict = [
		{ 'name':'constant',  'age_id':[0], 'time_id':[0], 'fun':fun }
	]
	#
	# covariate table:
	covariate_dict = list()
	#
	# mulcov table
	mulcov_dict = list()
	# --------------------------------------------------------------------------
	# data table:
	data_dict = list()
	#
	# values that are the same for all data rows
	row = {
		'node':        'world',
		'density':     'log_gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   time_list[0],
		'time_upper':   time_list[-1]
	}
	# Sincidence data (exclude data at 100)
	for age in data_age_list :
		#
		meas_value = iota_true(age)
		row['age_lower']    = age
		row['age_upper']    = age
		row['integrand']    = 'Sincidence'
		row['meas_value']   = meas_value
		row['meas_std']     = meas_value * 0.1
		data_dict.append( copy.copy(row) )
		#
	# mtother data (include data at 100)
	for age in data_age_list :
		meas_value = omega_true(age)
		row['age_lower']    = age
		row['age_upper']    = age
		row['integrand']    = 'mtother'
		row['meas_value']   = meas_value
		row['meas_std']     = meas_value * 0.01
		data_dict.append( copy.copy(row) )
	# --------------------------------------------------------------------------
	# prior_table
	prior_dict = [
		{   # prior_zero
			'name':     'prior_zero',
			'density':  'uniform',
			'lower':    0.0,
			'upper':    0.0,
			'mean':     0.0,
			'std':      None,
			'eta':      None
		},{ # prior_none
			'name':     'prior_none',
			'density':  'uniform',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      None,
			'eta':      None
		},{ # prior_difference
			'name':     'prior_difference',
			'density':  'log_gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      1.0,
			'eta':      1e-4
		},{ # prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    min(omega_true(0.0), iota_20),
			'upper':    1.0,
			'mean':     0.1,
			'std':      None,
			'eta':      None
		},{ # prior_iota_20
			'name':     'prior_iota_20',
			'density':  'uniform',
			'lower':    iota_20,
			'upper':    iota_20,
			'mean':     iota_20,
			'std':      None,
			'eta':      None
		}
	]
	# --------------------------------------------------------------------------
	# smooth table
	#
	iota_age_id = list()
	for age in iota_age_list :
		iota_age_id.append( age_list.index(age) )
	#
	omega_age_id = list()
	for age in omega_age_list :
		omega_age_id.append( age_list.index(age) )
	#
	smooth_dict = [
		{ # smooth_omega_parent
			'name':                     'smooth_omega_parent',
			'age_id':                   omega_age_id,
			'time_id':                  range(len(time_list)),
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                       fun_omega_parent
		},{ # smooth_iota_parent
			'name':                     'smooth_iota_parent',
			'age_id':                   iota_age_id,
			'time_id':                  range(len(time_list)),
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                       fun_iota_parent
		},{ # smooth_zero
			'name':                     'smooth_zero',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                       fun_zero
		}
	]
	# --------------------------------------------------------------------------
	# rate table
	rate_dict = [
		{
			'name':          'pini',
			'parent_smooth': None,
			'child_smooth':  None
		},{
			'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
			'child_smooth':  None
		},{
			'name':          'rho',
			'parent_smooth': None,
			'child_smooth':  None
		},{
			'name':          'chi',
			'parent_smooth': None,
			'child_smooth':  None
		},{
			'name':          'omega',
			'parent_smooth': 'smooth_omega_parent',
			'child_smooth':  None
		}
	]
	# ------------------------------------------------------------------------
	# option_dict
	option_dict = [
		{ 'name':'parent_node_name',       'value':'world'             },
		{ 'name':'number_sample',          'value':'1'                 },
		{ 'name':'fit_sample_index',       'value':None                },
		{ 'name':'ode_step_size',          'value':'1.0'               },
		{ 'name':'random_seed',            'value':'0'                 },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },

		{ 'name':'quasi_fixed',            'value':'true'              },
		{ 'name':'derivative_test_fixed',  'value':'first-order'       },
		{ 'name':'max_num_iter_fixed',     'value':'200'               },
		{ 'name':'print_level_fixed',      'value':'0'                 },
		{ 'name':'tolerance_fixed',        'value':'1e-6'              },
		{ 'name':'random_bound',           'value':None                },

		{ 'name':'derivative_test_random', 'value':'second-order'      },
		{ 'name':'max_num_iter_random',    'value':'100'               },
		{ 'name':'print_level_random',     'value':'0'                 },
		{ 'name':'tolerance_random',       'value':'1e-10'             }
	]
	# --------------------------------------------------------------------------
	# avgint table: empty
	avgint_dict = list()
	# --------------------------------------------------------------------------
	# create database
	dismod_at.create_database(
		file_name,
		age_list,
		time_list,
		integrand_dict,
		node_dict,
		weight_dict,
		covariate_dict,
		data_dict,
		prior_dict,
		smooth_dict,
		rate_dict,
		mulcov_dict,
		option_dict,
		avgint_dict
	)
	# -----------------------------------------------------------------------
	n_smooth  = len( smooth_dict )
	return
# ===========================================================================
file_name      = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
for command in [ 'init', 'start', 'fit' ] :
	cmd = [ program, file_name, command ]
	if command == 'start' :
		cmd.append('prior_mean')
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# Results for fitting with no noise
var_dict     = dismod_at.get_table_dict(connection, 'var')
fit_var_dict = dismod_at.get_table_dict(connection, 'fit_var')
#
parent_node_id = 1
eps            = 1e-4
#
# check rates values
count             = 0
iota_rate_id      = 1
omega_rate_id     = 4
max_err           = 0.0
tolerance         = 1e-3
age_list          = sorted( set( iota_age_list + omega_age_list ) )
for var_id in range( len(var_dict) ) :
	row   = var_dict[var_id]
	assert row['var_type'] == 'rate'
	assert row['node_id']  == 0
	age    = age_list[ row['age_id'] ]
	rate_id = row['rate_id']
	value  = fit_var_dict[var_id]['variable_value']
	if rate_id == iota_rate_id :
		value_true = iota_true(age)
		rate       = 'iota'
	else :
		assert rate_id == omega_rate_id
		value_true = omega_true(age)
		rate       = 'omega'
	max_err = max(max_err, abs( value / value_true - 1.0 ) )
	if( abs(value / value_true - 1.0) > tolerance ) :
		print(rate, age, value / value_true - 1.0 )
assert max_err <= tolerance
# -----------------------------------------------------------------------------
print('jump_at_age.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
