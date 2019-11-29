#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-19 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_zero_sum_child_rate.py$$ $newlinech #$$
# $spell
#	init
# $$
#
# $section Fitting With Sum of Random Effect Constrained to Zero$$
#
# $head Purpose$$
# This example demonstrates using
# The $cref/zero_sum_child_rate/option_table/zero_sum_child_rate/$$
# to improve the speed and estimation of the fixed effects.
#
# $head Problem Parameters$$
# $srcfile%example/user/zero_sum_child_rate.py%
#	0%# begin problem parameters%# end problem parameters%1
# %$$
#
# $head Data Simulation$$
# The true rate for the parent region $code north_america$$,
# used for simulating data, are
# $icode iota_parent$$ and $icode rho_parent$$ problem parameters.
# Rate effect used for $code canada$$ is $icode rate_effect_child$$
# and for $code united_states$$ $codei%-%rate_effect_child%$$.
# Note that these rates are constant in age and time.
#
# $head Nodes$$
# There are just three nodes for this example,
# The parent node, $code north_america$$, and the two child nodes
# $code united_states$$ and $code canada$$.
#
# $head Model Variables$$
# The non-zero $cref model_variables$$ for this example are
# $cref/iota/rate_table/rate_name/iota/$$ and $icode rho$$.
# Both the parent and child rates use a grid with one point in age
# and two points in time. Thus there are six model variables for each rate,
# two for the parent rates and four for the child rate effects.
# The resulting rates will be constant
# in age and constant in time except between the two time grid points
# where it is linear.
#
# $head Source Code$$
# $srcfile%
#	example/user/zero_sum_child_rate.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
# begin problem parameters
number_data       = 50
iota_parent       = 1e-2
rho_parent        = 2e-2
rate_effect_child = 0.2;
measurement_cv    = 0.01
# end problem parameters
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import copy
import math
import random
import time
test_program = 'example/user/zero_sum_child_rate.py'
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
def example_db (file_name) :
	def fun_rate_child(a, t) :
		return ('prior_rate_child', None, 'prior_gauss_diff')
	def fun_rate_parent(a, t) :
		return ('prior_rate_parent', None, 'prior_gauss_diff')
	import dismod_at
	# ----------------------------------------------------------------------
	# age list
	age_list    = [ 0.0,   50.0,  100.0 ]
	#
	# time list
	time_list   = [ 1990.0, 2010.0 ]
	#
	# integrand table
	integrand_table = [
		{ 'name':'Sincidence' },
		{ 'name':'remission' }
	]
	#
	# node table: north_america -> (united_states, canada)
	node_table = [
		{ 'name':'north_america', 'parent':''              },
		{ 'name':'united_states', 'parent':'north_america' },
		{ 'name':'canada',        'parent':'north_america' }
	]
	#
	# weight table:
	weight_table = list()
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
		'weight':      '',
		'hold_out':     False,
		'age_lower':    50.0,
		'age_upper':    50.0,
		'subgroup':     'world',
	}
	for data_id in range(number_data) :
		if data_id % 3 == 0 :
			row['node']       = 'north_america'
			row['data_name']  = 'na_' + str( data_id / 2 )
			effect_true       = 0.0
		if data_id % 3 == 1 :
			row['node']       = 'united_states'
			row['data_name']  = 'us_' + str( data_id / 2 )
			effect_true       = - rate_effect_child
		if data_id % 3 == 2 :
			row['node']       = 'canada'
			row['data_name']  = 'ca_' + str( data_id / 2 )
			effect_true       = + rate_effect_child
		if data_id % 2 == 0 :
			row['time_lower'] = 1990.0
			row['time_upper'] = 1990.0
		else :
			row['time_lower'] = 2010.0
			row['time_upper'] = 2010.0
		#
		if data_id < number_data / 2 :
			iota_true         = math.exp(effect_true) * iota_parent
			row['integrand']  = 'Sincidence'
			row['meas_std']   = iota_true * measurement_cv
			noise             = iota_true * random.gauss(0.0, measurement_cv)
			row['meas_value'] = iota_true + noise
		else :
			rho_true          = math.exp(effect_true) * rho_parent
			row['integrand']  = 'remission'
			row['meas_std']   = rho_true * measurement_cv
			noise             = rho_true * random.gauss(0.0, measurement_cv)
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
			'lower':    min(iota_true, rho_true) / 100.0,
			'upper':    max(iota_true, rho_true) * 100.0,
			'mean':     (iota_true + rho_true),
		},{ # prior_rate_child
			'name':     'prior_rate_child',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      100.0, # very large so like a uniform distribution
		},{ # prior_gauss_diff
			'name':     'prior_gauss_diff',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      100.0, # very large so like uniform
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	smooth_table = [
		{ # smooth_rate_child
			'name':                     'smooth_rate_child',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0, 1 ],
			'fun':                      fun_rate_child
		},{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0, 1 ],
			'fun':                       fun_rate_parent
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'iota',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child',
		},{
			'name':          'rho',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child',
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'north_america'     },
		{ 'name':'zero_sum_child_rate',    'value':'iota rho'          },
		{ 'name':'random_seed',            'value':'0'                 },
		{ 'name':'ode_step_size',          'value':'10.0'              },
		{ 'name':'rate_case',              'value':'iota_pos_rho_pos'  },

		{ 'name':'quasi_fixed',            'value':'true'          },
		{ 'name':'derivative_test_fixed',  'value':'first-order'   },
		{ 'name':'max_num_iter_fixed',     'value':'100'           },
		{ 'name':'print_level_fixed',      'value':'0'             },
		{ 'name':'tolerance_fixed',        'value':'1e-12'         },

		{ 'name':'derivative_test_random', 'value':'second-order'  },
		{ 'name':'max_num_iter_random',    'value':'100'           },
		{ 'name':'print_level_random',     'value':'0'             },
		{ 'name':'tolerance_random',       'value':'1e-10'         }
	]
	# ----------------------------------------------------------------------
	# subgroup_table
	subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
	# ----------------------------------------------------------------------
	# create database
	dismod_at.create_database(
		file_name,
		age_list,
		time_list,
		integrand_table,
		node_table,
		subgroup_table,
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
# ===========================================================================
# Create database and run init, start, fit with zero sum for random effects
file_name = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
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
	value     = fit_var_table[var_id]['fit_var_value']
	#
	if node_name == 'north_america' :
		if rate_name == 'iota' :
			err = value / iota_parent - 1.0
		else :
			err = value / rho_parent - 1.0
	elif node_name == 'canada' :
		err = value / rate_effect_child  - 1.0
	else :
		assert node_name == 'united_states'
		err = - value / rate_effect_child  - 1.0
	if abs(err) > 0.1 :
		print('node_name, err=', node_name, err)
		print('python_seed = ', python_seed)
		assert False
	if node_name != 'north_america' :
		sum_random[rate_name][time_id] += value
		count_random += 1
assert count_random == 8
for rate in [ 'iota', 'rho' ] :
	for time_id in [ 0 , 1 ] :
		if( abs( sum_random[rate][time_id] ) ) > 1e-9 :
			print('rate, sum random = ', rate, sum_random[rate][time_id] )
			print('python_seed = ', python_seed)
			assert False
#
# -----------------------------------------------------------------------
print('zero_sum_child_rate: OK')
# END PYTHON
