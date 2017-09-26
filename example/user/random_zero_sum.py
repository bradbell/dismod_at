#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_random_zero_sum.py$$ $newlinech #$$
# $spell
#	init
# $$
#
# $section Fitting With Sum of Random Effect Constrained to Zero$$
#
# $head Commands$$
# init, start, fit, fit
#
# $head Discussion$$
# This example demonstrates using
# The $cref/random_zero_sum/option_table/random_zero_sum/$$
# to improve the estimation of the fixed effects.
#
# $code
# $srcfile%
#	example/user/random_zero_sum.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
number_data   = 20
iota_true     = 1e-2
rho_true      = 2e-2
incidence_cv  = 0.05
remission_cv  = 0.05
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
import math
import random
import time
test_program = 'example/user/random_zero_sum.py'
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
python_seed = int( time.time() )
random.seed( python_seed )
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def constant_weight_fun(a, t) :
	return 1.0
def fun_rate_child(a, t) :
	return ('prior_rate_child', 'prior_gauss_diff', 'prior_gauss_diff')
def fun_rate_parent(a, t) :
	return ('prior_rate_parent', 'prior_gauss_diff', 'prior_gauss_diff')
# ------------------------------------------------------------------------
def example_db (file_name) :
	import dismod_at
	# ----------------------------------------------------------------------
	# age list
	age_list    = [ 0.0,   50.0,  100.0 ]
	#
	# time list
	time_list   = [ 1990.0, 2010.0 ]
	#
	# integrand table
	integrand_list = [
		'Sincidence',
		'remission'
	]
	#
	# node table: north_america -> (united_states, canada)
	node_table = [
		{ 'name':'north_america', 'parent':''              },
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
	# covariate table: no covriates
	covariate_table = list()
	#
	# mulcov table
	mulcov_table = list()
	#
	# avgint table: same order as list of integrands
	avgint_table = list()
	#
	# nslist_table:
	nslist_table = dict()
	# ----------------------------------------------------------------------
	# data table: same order as list of integrands
	data_table = list()
	# write out data
	row = {
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'age_lower':    50.0,
		'age_upper':    50.0,
	}
	for data_id in range(number_data) :
		if data_id % 4 == 0 or data_id % 4 == 1 :
			row['node']       = 'united_states'
			row['data_name']  = 'us_' + str( data_id / 2 )
		else :
			row['node']       = 'canada'
			row['data_name']  = 'ca_' + str( data_id / 2 )
		#
		if data_id % 4 == 0 or data_id % 4 == 2 :
			row['time_lower'] = 1990.0
			row['time_upper'] = 1990.0
		else :
			row['time_lower'] = 2010.0
			row['time_upper'] = 2010.0
		#
		if data_id < number_data / 2 :
			row['integrand']  = 'Sincidence'
			row['meas_std']   = iota_true * incidence_cv
			row['eta']        = None;
			noise             = iota_true * random.gauss(0.0, incidence_cv)
			row['meas_value'] = iota_true + noise
		else :
			row['integrand']  = 'remission'
			row['meas_std']   = rho_true * incidence_cv
			row['eta']        = None;
			noise             = rho_true * random.gauss(0.0, incidence_cv)
			row['meas_value'] = rho_true + noise
		#
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    1e-4,
			'upper':    None,
			'mean':     1e-1,
			'std':      None,
			'eta':      None
		},{ # prior_rate_child
			'name':     'prior_rate_child',
			'density':  'gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      100.0, # very large so like a uniform distribution
			'eta':      None
		},{ # prior_gauss_diff
			'name':     'prior_gauss_diff',
			'density':  'gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      100.0, # very large so like uniform
			'eta':      None
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	smooth_table = [
		{ # smooth_rate_child
			'name':                     'smooth_rate_child',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0, 1 ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                      fun_rate_child
		},{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0, 1 ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                       fun_rate_parent
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
		{ 'name':'parent_node_name',       'value':'north_america'     },
		{ 'name':'random_zero_sum',        'value':'iota rho'          },
		{ 'name':'random_seed',            'value':'0'                 },
		{ 'name':'ode_step_size',          'value':'10.0'              },
		{ 'name':'rate_case',              'value':'iota_pos_rho_pos'  },

		{ 'name':'quasi_fixed',            'value':'true'          },
		{ 'name':'derivative_test_fixed',  'value':'first-order'   },
		{ 'name':'max_num_iter_fixed',     'value':'100'           },
		{ 'name':'print_level_fixed',      'value':'0'             },
		{ 'name':'tolerance_fixed',        'value':'1e-10'         },

		{ 'name':'derivative_test_random', 'value':'second-order'  },
		{ 'name':'max_num_iter_random',    'value':'100'           },
		{ 'name':'print_level_random',     'value':'0'             },
		{ 'name':'tolerance_random',       'value':'1e-10'         }
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
	return
# ===========================================================================
# Create database and run init, start, fit with zero sum for random effects
file_name = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
fit_count      = 0
for command in [ 'init', 'fit', 'fit' ] :
	cmd = [ program, file_name, command ]
	if command == 'fit' :
		if fit_count == 0 :
			variables = 'fixed' # not necessary, but here for testing
		else :
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
# check the zero random effects solution
#
# get variable and fit_var tables
var_table     = dismod_at.get_table_dict(connection, 'var')
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
rate_table    = dismod_at.get_table_dict(connection, 'rate')
node_table    = dismod_at.get_table_dict(connection, 'node')
time_table    = dismod_at.get_table_dict(connection, 'time')
#
# for rate = iota, rho :
#	for node = north_america, canada, united_states
#		for time = 1990, 2010
n_var = len(var_table)
assert n_var == 12
#
# initialize sum of random effects for each rate and time
sum_random = {
	'iota' : [ 0.0, 0.0 ],
	'rho'  : [ 0.0, 0.0 ]
}
# check of values uses the fact that the data density is Gaussian
count_random = 0
ok           = True
for var_id in range( n_var ) :
	var_type = var_table[var_id]['var_type']
	assert( var_type == 'rate' )
	#
	rate_id   = var_table[var_id]['rate_id']
	rate_name = rate_table[rate_id]['rate_name']
	#
	node_id   = var_table[var_id]['node_id']
	node_name = node_table[node_id]['node_name']
	#
	# note there are only two time_id values in time_table
	time_id   = var_table[var_id]['time_id']
	time      = time_table[time_id]['time']
	#
	value     = fit_var_table[var_id]['variable_value']
	#
	if node_name == 'north_america' :
		if rate_name == 'iota' :
			err = value / iota_true - 1.0
		else :
			err = value / rho_true - 1.0
		ok = ok and abs(err) < 0.1
	else :
		sum_random[rate_name][time_id] += value
		count_random += 1
assert count_random == 8
for rate in [ 'iota', 'rho' ] :
	for time_id in [ 0 , 1 ] :
		ok = ok and  abs( sum_random[rate][time_id] ) < 1e-10
#
if not ok :
	print('python_seed = ', python_seed)
assert ok
# -----------------------------------------------------------------------
print('random_zero_sum: OK')
# END PYTHON
