#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_students.py$$ $newlinech #$$
# $spell
#	init
# $$
#
# $section Using Student's-t to Fitting Data with Outliers$$
#
# $head Source Code$$
# $srcfile%
#	example/user/students.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
iota_parent        = 1e-2   # true value of iota for parent
iota_random_effect = 0.5;   # true positive random effect value
n_data             = 30;    # must be a multiple of 3
n_outlier          = 3;     # number of data points that are outliers
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
import math
test_program = 'example/user/students.py'
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
def fun_rate_child(a, t) :
	return ('prior_rate_child', None, 'prior_students_zero')
def fun_rate_parent(a, t) :
	return ('prior_rate_parent', None, 'prior_students_zero')
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
	integrand_table = [
		{ 'name':'Sincidence' }
	]
	#
	# node table: north_america -> (united_states, canada)
	node_table = [
		{ 'name':'north_america', 'parent':'' },
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
	# valeus that are the same for all data
	# If you change the density to gaussian, this program will report a bad fit.
	row = {
		'density':     'students',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    50.0,
		'age_upper':    50.0,
		'integrand':    'Sincidence',
		'meas_std':     iota_parent / 10.,
		'nu':           3.0
	}
	for data_id in range( n_data ):
		# make sure both child and parent data gets included in fit
		# by balancing the offset between the two
		if data_id % 3 == 0 :
			row['node']        = 'united_states'
			meas_value = iota_parent * math.exp( iota_random_effect )
		elif data_id % 3 == 1 :
			row['node']        = 'canada'
			meas_value = iota_parent * math.exp( -iota_random_effect )
		else :
			row['node']        = 'north_america'
			meas_value = iota_parent
		#
		sign = 2.0 * (data_id % 2) - 1.0
		if data_id < n_outlier :
			meas_value = math.exp(sign * 5.0) * meas_value
		row['meas_value']  = meas_value
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    1e-4,
			'upper':    1.0,
			'mean':     0.1,
		},{ # prior_rate_child
			'name':     'prior_rate_child',
			'density':  'students',
			'mean':     0.0,
			'std':      10.0,
			'nu':       10.0,
		},{ # prior_students_zero
			'name':     'prior_students_zero',
			'density':  'students',
			'mean':     0.0,
			'std':      1e-2,
			'nu':       10.0,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	middle_age_id  = 1
	last_time_id   = 2
	smooth_table = [
		{ # smooth_rate_child
			'name':                     'smooth_rate_child',
			'age_id':                   [ middle_age_id ],
			'time_id':                  [ 0, last_time_id ],
			'fun':                      fun_rate_child
		},{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   [ middle_age_id ],
			'time_id':                  [ 0, last_time_id ],
			'fun':                      fun_rate_parent
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'iota',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child',
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'north_america'     },
		{ 'name':'random_seed',            'value':'0'                 },
		{ 'name':'ode_step_size',          'value':'10.0'              },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },

		{ 'name':'quasi_fixed',            'value':'true'          },
		{ 'name':'derivative_test_fixed',  'value':'adaptive'      },
		{ 'name':'max_num_iter_fixed',     'value':'100'           },
		{ 'name':'print_level_fixed',      'value':'0'             },
		{ 'name':'tolerance_fixed',        'value':'1e-11'         },

		{ 'name':'derivative_test_random', 'value':'second-order'  },
		{ 'name':'max_num_iter_random',    'value':'100'           },
		{ 'name':'print_level_random',     'value':'0'             },
		{ 'name':'tolerance_random',       'value':'1e-11'         }
	]
	# ----------------------------------------------------------------------
	# create database
	dismod_at.create_database(
		file_name,
		age_list,
		time_list,
		integrand_table,
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
# Create database and run init, start, fit with just fixed effects
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
#
# get solution from fit_var table
node_table     = dismod_at.get_table_dict(connection, 'node')
rate_table     = dismod_at.get_table_dict(connection, 'rate')
var_table      = dismod_at.get_table_dict(connection, 'var')
fit_var_table  = dismod_at.get_table_dict(connection, 'fit_var')
#
# optimal values when standard deviation of random effects is infinity
n_var = len( fit_var_table )
for var_id in range( n_var ) :
	var_type = var_table[var_id]['var_type']
	assert( var_type == 'rate' )
	#
	rate_id = var_table[var_id]['rate_id']
	assert( rate_table[rate_id]['rate_name'] == 'iota' )
	#
	value   = fit_var_table[var_id]['fit_var_value']
	#
	node_id  = var_table[var_id]['node_id']
	north_america   = node_table[node_id]['node_name'] == 'north_america'
	united_states   = node_table[node_id]['node_name'] == 'united_states'
	canada          = node_table[node_id]['node_name'] == 'canada'
	if north_america :
		err = value / iota_parent - 1.0
		if abs(err) > 1e-2:
			print('north_america', iota_parent, value, err)
			assert False
	elif united_states :
		err = value / iota_random_effect - 1.0
		if abs(err) > 1e-2:
			print('united_states', iota_random_effect, value, err)
			assert False
	else :
		assert canada
		err = value / iota_random_effect + 1.0
		if abs(err) > 1e-2:
			print('canada', - iota_random_effect, value, err)
			assert False
# -----------------------------------------------------------------------
print('students: OK')
# END PYTHON
