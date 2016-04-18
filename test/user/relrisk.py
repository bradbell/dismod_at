# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-16 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# true values used to simulate data
other_true      = 0.05
relrisk_true    = 2.0
n_data          = 3
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import pdb
test_program = 'test/user/relrisk.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
	usage  = 'python3 ' + test_program + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
print(test_program)
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
def fun_rate_child(a, t) :
	return ('prior_gauss_zero', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_other_parent(a, t) :
	return ('prior_other_parent', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_chi_parent(a, t) :
	return ('prior_chi_parent', 'prior_gauss_zero', 'prior_gauss_zero')
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
	integrand_dict = [
		{ 'name':'relrisk',  'eta':1e-6 }
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
	# values that are the same for all data rows
	row = {
		'node':        'world',
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   time_list[0],
		'time_upper':   time_list[-1],
		'age_lower':    age_list[0],
		'age_upper':    age_list[-1],
		'meas_value':   relrisk_true,
		'meas_std':     relrisk_true * 0.01,
		'integrand':    'relrisk'
	}
	# values that change between rows:
	for data_id in range( n_data ) :
		if data_id % 2 == 0 :
			row['node'] = 'north_america'
		else :
			row['node'] = 'canada'
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
			'std':      0.1,
			'eta':      None
		},{ # prior_other_parent
			'name':     'prior_other_parent',
			'density':  'uniform',
			'lower':    other_true,
			'upper':    other_true,
			'mean':     other_true,
			'std':      None,
			'eta':      None
		},{ # prior_chi_parent
			'name':     'prior_chi_parent',
			'density':  'uniform',
			'lower':    0.0,
			'upper':    1.0,
			'mean':     0.1,
			'std':      None,
			'eta':      None
		}
	]
	# --------------------------------------------------------------------------
	# smooth table
	smooth_dict = [
		{   # smooth_rate_child
			'name':                     'smooth_rate_child',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                      fun_rate_child
		},{ # smooth_other_parent
			'name':                     'smooth_other_parent',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                       fun_other_parent
		},{ # smooth_chi_parent
			'name':                     'smooth_chi_parent',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                       fun_chi_parent
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
			'parent_smooth': 'smooth_zero',
			'child_smooth':  'smooth_rate_child'
		},{
			'name':          'iota',
			'parent_smooth': 'smooth_other_parent',
			'child_smooth':  'smooth_rate_child'
		},{
			'name':          'rho',
			'parent_smooth': 'smooth_other_parent',
			'child_smooth':  'smooth_rate_child'
		},{
			'name':          'chi',
			'parent_smooth': 'smooth_chi_parent',
			'child_smooth':  'smooth_rate_child'
		},{
			'name':          'omega',
			'parent_smooth': 'smooth_other_parent',
			'child_smooth':  'smooth_rate_child'
		}
	]
	# ------------------------------------------------------------------------
	# option_dict
	option_dict = [
		{ 'name':'parent_node_name',       'value':'north_america'     },
		{ 'name':'number_sample',          'value':'1'                 },
		{ 'name':'fit_sample_index',       'value':None                },
		{ 'name':'ode_step_size',          'value':'10.0'              },
		{ 'name':'random_seed',            'value':'0'                 },
		{ 'name':'rate_case',              'value':'iota_pos_rho_pos'  },

		{ 'name':'quasi_fixed',            'value':'true'              },
		{ 'name':'derivative_test_fixed',  'value':'first-order'       },
		{ 'name':'max_num_iter_fixed',     'value':'100'               },
		{ 'name':'print_level_fixed',      'value':'0'                 },
		{ 'name':'tolerance_fixed',        'value':'1e-10'             },
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
rate_dict    = dismod_at.get_table_dict(connection, 'rate')
#
parent_node_id = 1
eps            = 1e-4
#
# check parent rates values
count             = 0
iota_rate_id      = 1
for var_id in range( len(var_dict) ) :
	row   = var_dict[var_id]
	assert row['var_type'] == 'rate'
	if row['node_id'] == parent_node_id :
		count += 1
		value  = fit_var_dict[var_id]['variable_value']
		rate   = rate_dict[ row['rate_id'] ]['rate_name']
		if rate == 'pini' :
			assert abs( value ) < eps
		elif rate == 'chi' :
			chi_true = (relrisk_true - 1.0) * other_true
			assert abs( value / chi_true - 1.0 ) < eps
		else :
			assert abs( value / other_true - 1.0 ) < eps
assert count == 5
#
# check child rates values
count             = 0
for var_id in range( len(var_dict) ) :
	row   = var_dict[var_id]
	if row['node_id'] != parent_node_id :
		count += 1
		value = fit_var_dict[var_id]['variable_value']
		assert abs(value) < eps
assert count == 5 * 2
# -----------------------------------------------------------------------------
print('relrisk.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
