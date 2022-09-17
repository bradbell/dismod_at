# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
#
# testing effect of random bounds greater than zero and less than infinity
#
iota_true = { 'north_america' : 1e-2, 'united_states' : 0.7, 'canada' : -0.7 }
bound_random = 0.6
# ------------------------------------------------------------------------
import sys
import os
import subprocess
import copy
from math import exp
test_program = 'test/user/bound_random.py'
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
# change into the build/test/user directory
if not os.path.exists('build/test/user') :
    os.makedirs('build/test/user')
os.chdir('build/test/user')
# ------------------------------------------------------------------------
# note that the a, t values are not used for this test
def constant_weight_fun(a, t) :
	return 1.0
def fun_rate_child(a, t) :
	return ('prior_rate_child', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_rate_parent(a, t) :
	return ('prior_rate_parent', 'prior_gauss_zero', 'prior_gauss_zero')
# ------------------------------------------------------------------------
def example_db (file_name) :
	import dismod_at
	# ----------------------------------------------------------------------
	# age table
	age_list    = [    0.0,  100.0 ]
	#
	# time table
	time_list   = [ 1990.0,  2015.0 ]
	#
	# integrand table
	integrand_table = [
		{ 'name':'Sincidence', 'minimum_meas_cv':0.0 }
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
		{ 'name':'constant',  'age_id':[0], 'time_id':[0], 'fun':fun }
	]
	#
	# covariate table: no covriates
	covariate_table = list()
	#
	# mulcov table
	mulcov_table = list()
	# ----------------------------------------------------------------------
	# data table: same order as list of integrands
	data_table = list()
	# write out data
	row = {
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    50.0,
		'age_upper':    50.0,
		'integrand':    'Sincidence',
		'meas_std':     iota_true['north_america'] * 0.2,
		'eta':          None,
		'subgroup':     'world',
	}
	for node in [ 'north_america', 'united_states', 'canada' ] :
		row['node']        = node
		if node == 'north_america' :
			row['meas_value']  = iota_true[node]
		else :
			effect = exp( iota_true[node] )
			row['meas_value']  = iota_true['north_america'] * effect
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    iota_true['north_america'] / 100,
			'upper':    iota_true['north_america'] * 100,
			'mean':     iota_true['north_america'] * 2,
			'std':      None,
			'eta':      None
		},{ # prior_rate_child
			'name':     'prior_rate_child',
			'density':  'gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      10.0, # very large so like a uniform distribution
			'eta':      None
		},{ # prior_gauss_zero
			'name':     'prior_gauss_zero',
			'density':  'gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      1e-2,
			'eta':      None
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	smooth_table = [
		{ # smooth_rate_child
			'name':                     'smooth_rate_child',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                      fun_rate_child
		},{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                      fun_rate_parent
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
			'parent_smooth': None,
			'child_smooth':  None,
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
		{ 'name':'bound_random',           'value':str(bound_random)   },
		{ 'name':'random_seed',            'value':'0'                 },
		{ 'name':'ode_step_size',          'value':'10.0'              },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },

		{ 'name':'quasi_fixed',            'value':'true'          },
		{ 'name':'derivative_test_fixed',  'value':'first-order'   },
		{ 'name':'max_num_iter_fixed',     'value':'100'           },
		{ 'name':'print_level_fixed',      'value':'0'             },
		{ 'name':'tolerance_fixed',        'value':'1e-11'         },

		{ 'name':'derivative_test_random', 'value':'second-order'  },
		{ 'name':'max_num_iter_random',    'value':'100'           },
		{ 'name':'print_level_random',     'value':'0'             },
		{ 'name':'tolerance_random',       'value':'1e-11'         }
	]
	# ----------------------------------------------------------------------
	# avgint table: same order as list of integrands
	avgint_table = list()
	# ----------------------------------------------------------------------
	# nslist_table:
	nslist_table = dict()
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
	n_smooth  = len( smooth_table )
	return
# ===========================================================================
# Create database and run init, start, fit, sample
file_name = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
for command in [ 'init', 'fit' ] :
	cmd = [ program, file_name, command ]
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
# check the solution
#
# get variable and fit_var tables
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
rate_table      = dismod_at.get_table_dict(connection, 'rate')
node_table      = dismod_at.get_table_dict(connection, 'node')
#
# one age and one time for each of north_america, canada, unites_states
n_var = len(var_table)
assert n_var == 3
#
# check of values uses the fact that the data density is Gaussian
for var_id in range( n_var ) :
	var_type = var_table[var_id]['var_type']
	assert( var_type == 'rate' )
	#
	rate_id = var_table[var_id]['rate_id']
	assert( rate_table[rate_id]['rate_name'] == 'iota' )
	#
	fit_value = fit_var_table[var_id]['fit_var_value']
	node_id   = var_table[var_id]['node_id']
	node_name = node_table[node_id]['node_name']
	truth     = iota_true[node_name]
	assert node_name in [ 'north_america', 'united_states', 'canada' ]
	if node_name == 'north_america' :
		# parent fit is not perfect because it tries to fit child data
		# and Laplace term in the objective.
		relerr = fit_value / iota_true['north_america'] - 1.0
		assert abs(relerr) < 1e-1
	if node_name == 'united_states' :
		assert( truth > bound_random )
		relerr = fit_value / bound_random - 1.0
		assert abs(relerr) < 1e-7
	if node_name == 'canada' :
		assert( truth < - bound_random )
		relerr = fit_value / bound_random + 1.0
		assert abs(relerr) < 1e-7
# -----------------------------------------------------------------------
print('bound_random.py: OK')
# END PYTHON
