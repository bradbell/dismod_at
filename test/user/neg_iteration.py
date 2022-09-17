# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# Test a case where lower limit == initial value < upper limit
# and using max_num_iter_fixed = -1.
iota_true        = 0.05
iota_upper       = iota_true * 10;
iota_initial     = iota_true / 2.0;
iota_lower       = iota_initial
n_data           = 3
# ------------------------------------------------------------------------
import sys
import os
import subprocess
test_program = 'test/user/neg_iteration.py'
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
def constant_weight_fun(a, t) :
	return 1.0
# note that the a, t values are not used for this case
def fun_zero(a, t) :
	return ('prior_zero', 'prior_none', 'prior_none')
def fun_rate_child(a, t) :
	return ('prior_gauss_zero', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_iota_parent(a, t) :
	return ('prior_iota_parent', 'prior_gauss_zero', 'prior_gauss_zero')
# ------------------------------------------------------------------------
def example_db (file_name) :
	import copy
	import dismod_at
	import math
	# ----------------------------------------------------------------------
	# age table
	age_list    = [    0.0, 100.0 ]
	#
	# time table
	time_list   = [ 1995.0, 2015.0 ]
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
	# covariate table:
	covariate_table = list()
	#
	# mulcov table
	mulcov_table = list()
	# ----------------------------------------------------------------------
	# data table:
	data_table = list()
	# values that are the same for all data rows
	row = {
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   time_list[0],
		'time_upper':   time_list[-1],
		'age_lower':    age_list[0],
		'age_upper':    age_list[-1],
		'meas_value':   iota_true,
		'meas_std':     iota_true * 0.1,
		'eta':          None,
		'integrand':    'Sincidence',
		'subgroup':     'world',
	}
	# values that change between rows:
	for data_id in range( n_data ) :
		if data_id % 2 == 0 :
			row['node'] = 'north_america'
		else :
			row['node'] = 'canada'
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
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
			'std':      0.1,
			'eta':      None
		},{ # prior_iota_parent
			'name':     'prior_iota_parent',
			'density':  'uniform',
			'lower':    iota_lower,
			'upper':    iota_upper,
			'mean':     iota_initial,
			'std':      None,
			'eta':      None
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	smooth_table = [
		{   # smooth_rate_child
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
	# use very large tolerance_fixed to avoid warning during test.
	option_table = [
		{ 'name':'parent_node_name',       'value':'north_america'     },
		{ 'name':'ode_step_size',          'value':'10.0'              },
		{ 'name':'random_seed',            'value':'0'                 },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },

		{ 'name':'quasi_fixed',            'value':'true'              },
		{ 'name':'derivative_test_fixed',  'value':'none'              },
		{ 'name':'max_num_iter_fixed',     'value':'-1'               },
		{ 'name':'print_level_fixed',      'value':'0'                 },
		{ 'name':'tolerance_fixed',        'value':'1e+10'             },

		{ 'name':'derivative_test_random', 'value':'second-order'      },
		{ 'name':'max_num_iter_random',    'value':'50'                },
		{ 'name':'print_level_random',     'value':'0'                 },
		{ 'name':'tolerance_random',       'value':'1e-10'             }
	]
	# ----------------------------------------------------------------------
	# avgint table: empty
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
file_name      = 'example.db'
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
# Results for fitting with no noise
var_table     = dismod_at.get_table_dict(connection, 'var')
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
#
parent_node_id = 1
eps            = 1e-4
#
# check parent rates values
iota_rate_id      = 1
for var_id in range( len(var_table) ) :
	row    = var_table[var_id]
	value  = fit_var_table[var_id]['fit_var_value']
	assert row['var_type'] == 'rate'
	if row['node_id'] == parent_node_id :
		if row['rate_id'] == iota_rate_id :
			assert value == iota_initial
		else :
			assert value ==  0.0
# -----------------------------------------------------------------------------
print('neg_iteration.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
