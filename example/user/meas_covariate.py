# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_meas_covariate.py$$ $newlinech #$$
# $spell
#	init
#	avgint
#	Covariates
#	covariate
#	Integrands
# $$
#
# $section Using Measurement Covariates on Multiple Integrands$$
#
# $head Commands$$
# init, start, fit
#
# $head See Also$$
# $cref user_lasso_covariate.py$$
#
# $code
# $srcfile%
#	example/user/meas_covariate.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# true values used to simulate data
iota_true        = 0.05
remission_true   = 0.10
n_data           = 51
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
import math
test_program = 'example/user/meas_covariate.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
	usage  = 'python3 ' + test_program + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
print(test_program)
#
# import dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
	sys.path.insert(0, local_dir)
import dismod_at
#
# change into the build/example/user directory
distutils.dir_util.mkpath('build/example/user')
os.chdir('build/example/user')
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def constant_weight_fun(a, t) :
	return 1.0
# note that the a, t values are not used for this case
def fun_rate_child(a, t) :
	return ('prior_gauss_zero', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_rate_parent(a, t) :
	return ('prior_value_parent', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_mulcov(a, t) :
	return ('prior_mulcov', 'prior_gauss_zero', 'prior_gauss_zero')
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ----------------------------------------------------------------------
	# age table
	age_list    = [    0.0, 50.0,    100.0 ]
	#
	# time table
	time_list   = [ 1995.0, 2005.0, 2015.0 ]
	#
	# integrand table
	integrand_list = [
		'Sincidence',
		'remission'
	]
	#
	# node table: world -> north_america
	#             north_america -> (united_states, canada)
	node_table = [
		{ 'name':'world',         'parent':'' },
		{ 'name':'north_america', 'parent':'world' },
		{ 'name':'united_states', 'parent':'north_america' },
		{ 'name':'canada',        'parent':'north_america' }
	]
	#
	# weight table: The constant function 1.0 (one age and one time point)
	fun = constant_weight_fun
	weight_table = [
		{ 'name':'constant',  'age_id':[1], 'time_id':[1], 'fun':fun }
	]
	#
	# covariate table:
	covariate_table = [
		{'name':'income', 'reference':0.5, 'max_difference':None},
		{'name':'sex',    'reference':0.0, 'max_difference':0.6}
	]
	#
	# mulcov table
	# income has been scaled the same as sex so man use same smoothing
	mulcov_table = [
		{
			'covariate': 'income',
			'type':      'meas_value',
			'effected':  'Sincidence',
			'smooth':    'smooth_mulcov'
		},{	# Example of a mulcov table entry that is not used
			'covariate': 'income',
			'type':      'rate_value',
			'effected':  'rho',
			'smooth':    None
		},{
			'covariate': 'income',
			'type':      'meas_value',
			'effected':  'remission',
			'smooth':    'smooth_mulcov'
		}
	]
	#
	# avgint table: empty
	avgint_table = list()
	#
	# nslist_table:
	nslist_table = dict()
	# ----------------------------------------------------------------------
	# data table:
	data_table = list()
	# values that are the same for all data rows
	row = {
		'node':        'world',
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   1995.0,
		'time_upper':   1995.0,
		'age_lower':    0.0,
		'age_upper':    0.0
	}
	# values that change between rows:
	mulcov_incidence = 1.0
	mulcov_remission = 2.0;
	income_reference = 0.5
	n_integrand      = len( integrand_list )
	for data_id in range( n_data ) :
		integrand   = integrand_list[ data_id % n_integrand ]
		income      = data_id / float(n_data-1)
		sex         = ( data_id % 3 - 1.0 ) / 2.0
		meas_value  = iota_true
		effect      = (income - income_reference) * mulcov_incidence
		meas_value *= math.exp(effect)
		if integrand == 'remission' :
			meas_value  = remission_true
			# note that sex has no effect
			effect      = (income - income_reference) * mulcov_remission
			meas_value *= math.exp(effect)
		meas_std    = 0.1 * meas_value
		row['meas_value'] = meas_value
		row['meas_std']   = meas_std
		row['eta']        = None;
		row['integrand']  = integrand
		row['income']     = income
		row['sex']        = sex
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_gauss_zero
			'name':     'prior_gauss_zero',
			'density':  'gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      0.01,
			'eta':      None
		},{ # prior_value_parent
			'name':     'prior_value_parent',
			'density':  'uniform',
			'lower':    0.01,
			'upper':    1.00,
			'mean':     0.1,
			'std':      None,
			'eta':      None
		},{ # prior_mulcov
			'name':     'prior_mulcov',
			'density':  'uniform',
			'lower':    -5.0,
			'upper':     5.0,
			'mean':     0.0,
			'std':      None,
			'eta':      None
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	middle_age_id  = 1
	middle_time_id = 1
	last_age_id    = 2
	last_time_id   = 2
	smooth_table = [
		{   # smooth_rate_child
			'name':                     'smooth_rate_child',
			'age_id':                   [ last_age_id ],
			'time_id':                  [ last_time_id ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                      fun_rate_child
		},{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   [ 0, last_age_id ],
			'time_id':                  [ 0, last_time_id ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                       fun_rate_parent
		},{ # smooth_mulcov
			'name':                     'smooth_mulcov',
			'age_id':                   [ middle_age_id ],
			'time_id':                  [ middle_time_id ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                       fun_mulcov
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'pini',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		},{
			'name':          'iota',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child',
			'child_nslist':  None
		},{
			'name':          'rho',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child',
			'child_nslist':  None
		},{
			'name':          'chi',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		},{
			'name':          'omega',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'world'        },
		{ 'name':'ode_step_size',          'value':'10.0'         },
		{ 'name':'random_seed',            'value':'0'            },
		{ 'name':'rate_case',              'value':'iota_pos_rho_pos' },

		{ 'name':'quasi_fixed',            'value':'true'         },
		{ 'name':'derivative_test_fixed',  'value':'first-order'  },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-8'         },

		{ 'name':'derivative_test_random', 'value':'second-order' },
		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-10'        }
	]
	# ----------------------------------------------------------------------
	# create database
	dismod_at.create_database(
		file_name,
		age_list,
		time_list,
		integrand_list,
		node_table,
		weight_table,
		covariate_table,
		avgint_table,
		data_table,
		prior_table,
		smooth_table,
		nslist_table,
		rate_table,
		mulcov_table,
		option_table
	)
	# ----------------------------------------------------------------------
	n_smooth  = len( smooth_table )
	rate_true = []
	for rate_id in range( len( data_table ) ) :
		# for this particular example
		data_id    = rate_id
		meas_value = data_table[data_id]['meas_value']
		rate_true.append(meas_value)
	#
	return
# ===========================================================================
# Note that this process uses the fit results as the truth for simulated data
# The fit_var table corresponds to fitting with no noise.
# The sample table corresponds to fitting with noise.
file_name      = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
for command in [ 'init', 'start', 'fit' ] :
	cmd = [ program, file_name, command ]
	if command == 'start' :
		cmd.append('prior_mean')
	if command == 'fit' :
		variables = 'both'
		cmd.append(variables)
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
var_table     = dismod_at.get_table_dict(connection, 'var')
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
#
middle_age_id  = 1
middle_time_id = 1
last_age_id    = 2
last_time_id   = 2
parent_node_id = 0
tol            = 5e-7
#
# check parent iota and remission values
count             = 0
iota_rate_id      = 1
remission_rate_id = 2
for var_id in range( len(var_table) ) :
	row   = var_table[var_id]
	match = row['var_type'] == 'rate'
	match = match and row['node_id'] == parent_node_id
	if match and row['rate_id'] == iota_rate_id :
		count += 1
		value = fit_var_table[var_id]['variable_value']
		assert abs( value / iota_true - 1.0 ) < tol
	if match and row['rate_id'] == remission_rate_id :
		count += 1
		value = fit_var_table[var_id]['variable_value']
		assert abs( value / remission_true - 1.0 ) < 5.0 * tol
assert count == 8
#
# check covariate multiplier values
count                   = 0
mulcov_incidence        = 1.0
mulcov_remission        = 2.0;
remission_integrand_id  = 1
for var_id in range( len(var_table) ) :
	row   = var_table[var_id]
	match = row['var_type'] == 'mulcov_meas_value'
	if match :
		integrand_id = row['integrand_id']
		count       += 1
		value        = fit_var_table[var_id]['variable_value']
		if integrand_id == remission_integrand_id :
			assert abs( value / mulcov_remission - 1.0 ) < tol
		else :
			assert abs( value / mulcov_incidence - 1.0 ) < tol
			assert abs( value / mulcov_incidence - 1.0 ) < tol
assert count == 2
# -----------------------------------------------------------------------------
print('meas_covariate.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
