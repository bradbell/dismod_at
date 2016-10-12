#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-16 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_constrain_ran.py$$ $newlinech #$$
# $spell
# $$
#
# $section Fitting Simulated Data Example$$
#
# $head Under Construction$$
#
# $code
# $srcfile%
#	example/user/constrain_ran.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
iota_true         = 1e-2;
iota_child_offset = +0.5;
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
import math
test_program = 'example/user/constrain_ran.py'
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
# change into the build/example/user directory
distutils.dir_util.mkpath('build/example/user')
os.chdir('build/example/user')
# ------------------------------------------------------------------------
# note that the a, t values are not used for this example
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
	covariate_dict = list()
	#
	# mulcov table
	mulcov_dict = list()
	# --------------------------------------------------------------------------
	# data table: same order as list of integrands
	data_dict = list()
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
		'meas_std':     iota_true / 10.
	}
	# make sure both child and parent data gets included in fit
	# by balancing the offset between the two
	row['node']        = 'united_states'
	row['meas_value']  = iota_true * (1.0 + iota_child_offset)
	data_dict.append( copy.copy(row) )
	row['node']        = 'canada'
	row['meas_value']  = iota_true * (1.0 + iota_child_offset)
	data_dict.append( copy.copy(row) )
	row['node']        = 'north_america'
	row['meas_value']  = iota_true * (1.0 - iota_child_offset)
	data_dict.append( copy.copy(row) )
	data_dict.append( copy.copy(row) )

	# --------------------------------------------------------------------------
	# prior_table
	prior_dict = [
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
			'lower':    0.0,
			'upper':    0.0,
			'mean':     0.0,
			'std':      1.0,
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
	# --------------------------------------------------------------------------
	# smooth table
	middle_age_id  = 1
	last_time_id   = 2
	smooth_dict = [
		{ # smooth_rate_child
			'name':                     'smooth_rate_child',
			'age_id':                   [ middle_age_id ],
			'time_id':                  [ 0, last_time_id ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                      fun_rate_child
		},{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   [ middle_age_id ],
			'time_id':                  [ 0, last_time_id ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                       fun_rate_parent
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
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child'
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
			'parent_smooth': None,
			'child_smooth':  None
		}
	]
	# ------------------------------------------------------------------------
	# option_dict
	option_dict = [
		{ 'name':'parent_node_name',       'value':'north_america'     },
		{ 'name':'number_simulate',        'value':'1'                 },
		{ 'name':'fit_simulate_index',     'value':None                },
		{ 'name':'ode_step_size',          'value':'10.0'              },
		{ 'name':'random_seed',            'value':'0'                 },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },

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
	# --------------------------------------------------------------------------
	# avgint table: same order as list of integrands
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
# Create database and run init, stgart, fit
file_name = 'example.db'
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
# get variable and fit_var tables
var_dict       = dismod_at.get_table_dict(connection, 'var')
fit_var_dict   = dismod_at.get_table_dict(connection, 'fit_var')
rate_dict      = dismod_at.get_table_dict(connection, 'rate')
node_dict      = dismod_at.get_table_dict(connection, 'node')
#
# one age and two times for each of north_america, canada, unites_states
n_var = len(var_dict)
assert n_var == 6
#
for var_id in range( n_var ) :
	var_type = var_dict[var_id]['var_type']
	assert( var_type == 'rate' )
	#
	rate_id = var_dict[var_id]['rate_id']
	assert( rate_dict[rate_id]['rate_name'] == 'iota' )
	#
	value   = fit_var_dict[var_id]['variable_value']
	#
	node_id  = var_dict[var_id]['node_id']
	parent   = node_dict[node_id]['node_name'] == 'north_america'
	if parent :
		err = value / iota_true - 1.0
		assert abs(err) < 1e-6
	else :
		canada         = node_dict[node_id]['node_name'] == 'canada'
		united_states  = node_dict[node_id]['node_name'] == 'united_states'
		#
		assert value == 0.0
		assert canada or united_states
# -----------------------------------------------------------------------
print('constrain_ran: OK')
# END PYTHON
