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
# $begin user_lasso_covariate.py$$ $newlinech #$$
# $spell
#	Covariates
#	covariate
# $$
#
# $section Using Lasso with Covariates (Under Construction)$$
#
# The example currently generates a convergence warning and only estimates
# one covariate multiplier (eventually will estimate two).
#
# $code
# $verbatim%
#	example/user/lasso_covariate.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import distutils.dir_util
import subprocess
test_program = 'example/user/lasso_covariate.py'
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
# note that the a, t values are not used for this case
def fun_zero(a, t) :
	return ('prior_zero', 'prior_none', 'prior_none')
def fun_rate_child(a, t) :
	return ('prior_gauss_zero', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_iota_parent(a, t) :
	return ('prior_value_parent', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_mulcov(a, t) :
	return ('prior_mulcov', 'prior_gauss_zero', 'prior_gauss_zero')
# ------------------------------------------------------------------------
# true values used to simulate data
iota_true     = 0.05
n_data        = 11
mulcov_income = iota_true / n_data
#
def example_db (file_name) :
	import copy
	import dismod_at
	import math
	# ----------------------------------------------------------------------
	# age table
	age_list    = [    0.0, 50.0,    100.0 ]
	#
	# time table
	time_list   = [ 1995.0, 2005.0, 2015.0 ]
	#
	# integrand table
	integrand_dict = [
		{ 'name':'Sincidence',  'eta':1e-6 }
	]
	#
	# node table: world -> north_america
	#             north_america -> (united_states, canada)
	node_dict = [
		{ 'name':'world',         'parent':'' },
		{ 'name':'north_america', 'parent':'world' },
		{ 'name':'united_states', 'parent':'north_america' },
		{ 'name':'canada',        'parent':'north_america' }
	]
	#
	# weight table: The constant function 1.0 (one age and one time point)
	fun = constant_weight_fun
	weight_dict = [
		{ 'name':'constant',  'age_id':[1], 'time_id':[1], 'fun':fun }
	]
	#
	# covariate table: no covriates
	income_reference = 100.00
	covariate_dict = [
		{'name':'income', 'reference':income_reference, 'max_difference':None},
		{'name':'elevation', 'reference':100.0,  'max_difference':None}
	]
	#
	# mulcov table
	mulcov_dict = [
		{
			'covariate':'income',
			'type':     'rate_mean',
			'effected': 'iota',
			'smooth':   'smooth_mulcov'
		#},{
		#	'covariate':'elevation',
		#	'type':     'rate_mean',
		#	'effected': 'iota',
		#	'smooth':   'smooth_mulcov'
		}
	]
	# --------------------------------------------------------------------------
	# data table: same order as list of integrands
	data_dict = list()
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
	for data_id in range( 10 ) :
		income      = (data_id - n_data / 2) + income_reference
		elevation   = 100.0 * ( data_id % 3 )
		meas_value  = iota_true
		meas_value *= math.exp( (income - income_reference) * mulcov_income )
		meas_std    = 0.2 * meas_value
		integrand   = integrand_dict[0]['name']
		row['meas_value'] = meas_value
		row['meas_std']   = meas_std
		row['integrand']  = integrand
		row['income']     = income
		row['elevation']  = elevation
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
		},{ # prior_one
			'name':     'prior_one',
			'density':  'uniform',
			'lower':    1.0,
			'upper':    1.0,
			'mean':     1.0,
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
			'density':  'laplace',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      5.0,
			'eta':      None
		}
	]
	# --------------------------------------------------------------------------
	# smooth table
	middle_age_id  = 1
	middle_time_id = 1
	last_age_id    = 2
	last_time_id   = 2
	smooth_dict = [
		{   # smooth_rate_child
			'name':                     'smooth_rate_child',
			'age_id':                   [ last_age_id ],
			'time_id':                  [ last_time_id ],
			'mulstd_value_prior_name':  'prior_one',
			'mulstd_dage_prior_name':   'prior_one',
			'mulstd_dtime_prior_name':  'prior_one',
			'fun':                      fun_rate_child
		},{ # smooth_iota_parent
			'name':                     'smooth_iota_parent',
			'age_id':                   [ 0, last_age_id ],
			'time_id':                  [ 0, last_time_id ],
			'mulstd_value_prior_name':  'prior_one',
			'mulstd_dage_prior_name':   'prior_one',
			'mulstd_dtime_prior_name':  'prior_one',
			'fun':                       fun_iota_parent
		},{ # smooth_zero
			'name':                     'smooth_zero',
			'age_id':                   [ middle_age_id ],
			'time_id':                  [ middle_time_id ],
			'mulstd_value_prior_name':  'prior_one',
			'mulstd_dage_prior_name':   'prior_one',
			'mulstd_dtime_prior_name':  'prior_one',
			'fun':                       fun_zero
		},{ # smooth_mulcov
			'name':                     'smooth_mulcov',
			'age_id':                   [ middle_age_id ],
			'time_id':                  [ middle_time_id ],
			'mulstd_value_prior_name':  'prior_one',
			'mulstd_dage_prior_name':   'prior_one',
			'mulstd_dtime_prior_name':  'prior_one',
			'fun':                       fun_mulcov
		}
	]
	# --------------------------------------------------------------------------
	# rate table
	rate_dict = [
		{
			'name':          'pini',
			'parent_smooth': 'smooth_zero',
			'child_smooth':  'smooth_rate_child'
		},{
			'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
			'child_smooth':  'smooth_rate_child'
		},{
			'name':          'rho',
			'parent_smooth': 'smooth_zero',
			'child_smooth':  'smooth_rate_child'
		},{
			'name':          'chi',
			'parent_smooth': 'smooth_zero',
			'child_smooth':  'smooth_rate_child'
		},{
			'name':          'omega',
			'parent_smooth': 'smooth_zero',
			'child_smooth':  'smooth_rate_child'
		}
	]
	# ------------------------------------------------------------------------
	# option_dict
	option_dict = [
		{ 'name':'parent_node_id','value':'0'            },
		{ 'name':'ode_step_size', 'value':'10.0'         },
		{ 'name':'tolerance',     'value':'1e-10'        },
		{ 'name':'max_num_iter',  'value':'100'          },
		{ 'name':'print_level',   'value':'0'            },
		{ 'name':'random_seed',   'value':'0'            },
		{ 'name':'number_sample', 'value':'1'            },
		{ 'name':'rate_info',     'value':'rho_and_chi_zero' }
	]
	# --------------------------------------------------------------------------
	# avg_case table: empty
	avg_case_dict = list()
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
		avg_case_dict
	)
	# -----------------------------------------------------------------------
	n_smooth  = len( smooth_dict )
	rate_true = []
	for rate_id in range( len( data_dict ) ) :
		# for this particular example
		data_id    = rate_id
		meas_value = data_dict[data_id]['meas_value']
		rate_true.append(meas_value)
	#
	return
# ===========================================================================
file_name      = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
for command in [ 'init', 'start', 'fit' ] :
	cmd  = [ program, command, file_name ]
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# get parent rate variable values
var_dict     = dismod_at.get_table_dict(connection, 'var')
fit_var_dict = dismod_at.get_table_dict(connection, 'fit_var')
#
middle_age_id  = 1
middle_time_id = 1
last_age_id    = 2
last_time_id   = 2
parent_node_id = 0
tol            = 1e-8
#
# check parent iota values
count          = 0
iota_rate_id   = 1
for var_id in range( len(var_dict) ) :
	row   = var_dict[var_id]
	match = row['var_type'] == 'rate'
	match = match and row['rate_id'] == iota_rate_id
	match = match and row['node_id'] == parent_node_id
	if match :
		count += 1
		value = fit_var_dict[var_id]['fit_var_value']
		assert abs( value / iota_true - 1.0 ) < tol
assert count == 4
#
# check other iota values
for var_id in range( len(var_dict) ) :
	row   = var_dict[var_id]
	match = row['var_type'] == 'rate'
	if row['rate_id'] == iota_rate_id :
		match = match and row['node_id'] != parent_node_id
	if match :
		value = fit_var_dict[var_id]['fit_var_value']
		assert abs( value ) < tol
#
# check covariate multiplier values
tol  = 1e-5
count = 0
for var_id in range( len(var_dict) ) :
	row   = var_dict[var_id]
	match = row['var_type'] == 'mulcov_rate_mean'
	if match :
		count       += 1
		value        = fit_var_dict[var_id]['fit_var_value']
		covariate_id = row['covariate_id']
		if covariate_id == 0 :
			assert abs( value / mulcov_income - 1.0 ) < tol
		else :
			assert abs( value - 1.0 ) < tol
assert count == 1
# -----------------------------------------------------------------------------
print('lasso_covariate.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
