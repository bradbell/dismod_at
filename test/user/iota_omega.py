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
# true values used to simulate data
iota_20        = 1e-4
iota_100       = 1e-1
omega_20       = 2e-4
omega_100      = 2e-1
age_list       = [  0.0, 20.0, 100.0 ]
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
test_program = 'test/user/iota_omega.py'
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
# change into the build/test/user directory
distutils.dir_util.mkpath('build/test/user')
os.chdir('build/test/user')
# ------------------------------------------------------------------------
def constant_weight_fun(a, t) :
	return 1.0
# note that the a, t values are not used for this case
def fun_zero(a, t) :
	return ('prior_zero', 'prior_none', 'prior_none')
def fun_omega_parent(a, t) :
	return ('prior_rate_parent', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_iota_parent(a, t) :
	if a <= 20.0 :
		return ('prior_iota_20', 'prior_gauss_zero', 'prior_gauss_zero')
	else :
		return ('prior_rate_parent', 'prior_gauss_zero', 'prior_gauss_zero')
# ------------------------------------------------------------------------
def example_db (file_name) :
	import copy
	import dismod_at
	import math
	# ----------------------------------------------------------------------
	# age table (in age_list above)
	#
	# time table
	time_list   = [ 1995.0, 2015.0 ]
	#
	# integrand table
	integrand_dict = [
		{ 'name':'Sincidence',  'eta':1e-4 },
		{ 'name':'mtother',     'eta':1e-4 }
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
	# values that change between rows:
	for age in range(0, 100, 20) :
		#
		if age < 20.0 :
			meas_value = iota_20
		else :
			slope      = (iota_100 - iota_20) / 80.0
			meas_value = iota_20 + (age - 20.0)  * slope
		row['age_lower']    = age
		row['age_upper']    = age
		row['integrand']    = 'Sincidence'
		row['meas_value']   = meas_value
		row['meas_std']     = meas_value * 0.1
		data_dict.append( copy.copy(row) )
		#
	# values that change between rows:
	for age in range(0, 100, 20) :
		if age < 20.0 :
			meas_value = omega_20
		else :
			slope      = (omega_100 - omega_20) / 80.0
			meas_value = omega_20 + (age - 20.0)  * slope
		row['age_lower']    = age
		row['age_upper']    = age
		row['integrand']    = 'mtother'
		row['meas_value']   = meas_value
		row['meas_std']     = meas_value * 0.1
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
		},{ # prior_gauss_zero
			'name':     'prior_gauss_zero',
			'density':  'gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      1.0,
			'eta':      None
		},{ # prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    min(iota_20 , omega_20) / 2.0 ,
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
	smooth_dict = [
		{ # smooth_omega_parent
			'name':                     'smooth_omega_parent',
			'age_id':                   range(len(age_list)),
			'time_id':                  range(len(time_list)),
			'mulstd_value_prior_name':  '',
			'mulstd_dage_prior_name':   '',
			'mulstd_dtime_prior_name':  '',
			'fun':                       fun_omega_parent
		},{ # smooth_iota_parent
			'name':                     'smooth_iota_parent',
			'age_id':                   range(len(age_list)),
			'time_id':                  range(len(time_list)),
			'mulstd_value_prior_name':  '',
			'mulstd_dage_prior_name':   '',
			'mulstd_dtime_prior_name':  '',
			'fun':                       fun_iota_parent
		},{ # smooth_zero
			'name':                     'smooth_zero',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'mulstd_value_prior_name':  '',
			'mulstd_dage_prior_name':   '',
			'mulstd_dtime_prior_name':  '',
			'fun':                       fun_zero
		}
	]
	# --------------------------------------------------------------------------
	# rate table
	rate_dict = [
		{
			'name':          'pini',
			'parent_smooth': '',
			'child_smooth':  ''
		},{
			'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
			'child_smooth':  ''
		},{
			'name':          'rho',
			'parent_smooth': '',
			'child_smooth':  ''
		},{
			'name':          'chi',
			'parent_smooth': '',
			'child_smooth':  ''
		},{
			'name':          'omega',
			'parent_smooth': 'smooth_omega_parent',
			'child_smooth':  ''
		}
	]
	# ------------------------------------------------------------------------
	# option_dict
	option_dict = [
		{ 'name':'parent_node_name',       'value':'world'             },
		{ 'name':'number_sample',          'value':'1'                 },
		{ 'name':'fit_sample_index',       'value':None                },
		{ 'name':'ode_step_size',          'value':'5.0'               },
		{ 'name':'random_seed',            'value':'0'                 },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },

		{ 'name':'quasi_fixed',            'value':'true'              },
		{ 'name':'derivative_test_fixed',  'value':'first-order'       },
		{ 'name':'max_num_iter_fixed',     'value':'100'               },
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
tolerance         = 1e-3
for var_id in range( len(var_dict) ) :
	row   = var_dict[var_id]
	assert row['var_type'] == 'rate'
	assert row['node_id']  == 0
	age     = age_list[ row['age_id'] ]
	rate_id = row['rate_id']
	value  = fit_var_dict[var_id]['variable_value']
	if rate_id == iota_rate_id :
		if age <= 20.0 :
			value_true = iota_20
		else :
			slope      = (iota_100 - iota_20) / 80.0
			value_true = iota_20 + (age - 20.0) * slope
	else :
		assert rate_id == omega_rate_id
		if age <= 20.0 :
			value_true = omega_20
		else :
			slope      = (omega_100 - omega_20) / 80.0
			value_true = omega_20 + (age - 20.0) * slope
	assert( value / value_true - 1.0 ) < tolerance
# -----------------------------------------------------------------------------
print('iota_omega.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
