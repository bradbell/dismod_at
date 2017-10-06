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
# $begin user_predict_fit.py$$ $newlinech #$$
# $spell
#	init
# $$
#
# $section Predict Average Integrand Using Results of a Fit$$
#
# $code
# $srcfile%
#	example/user/predict_fit.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
iota_north_america = 1e-2
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import distutils.dir_util
import subprocess
import copy
import math
test_program = 'example/user/predict_fit.py'
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
# ---------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def constant_weight_fun(a, t) :
	return 1.0
def fun_rate_child(a, t) :
	return ('prior_rate_child', 'prior_zero', 'prior_dtime_child')
def fun_rate_parent(a, t) :
	return ('prior_rate_parent', 'prior_zero', 'prior_dtime_parent')
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
		'susceptible'
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
	# covariate table: no covriates
	covariate_table = list()
	#
	# mulcov table
	mulcov_table = list()
	#
	# nslist_table:
	nslist_table = dict()
	# ----------------------------------------------------------------------
	# avgint table: same order as list of integrands
	avgint_table = list()
	# values that are the same for all data rows
	row = {
		'integrand':   'susceptible',
		'node':        'north_america',
		'weight':      'constant',
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    50.0,
		'age_upper':    50.0
	}
	avgint_table.append( copy.copy(row) )
	row['node'] = 'canada'
	avgint_table.append( copy.copy(row) )
	row['node'] = 'united_states'
	avgint_table.append( copy.copy(row) )
	# ----------------------------------------------------------------------
	# data table:
	data_table = list()
	row = {
		'node':        'north_america',
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    0.0,
		'age_upper':    100.0,
		'integrand':   'Sincidence',
		'meas_value':   iota_north_america,
		'meas_std':     1e-1 * iota_north_america,
	}
	data_table.append( copy.copy(row) )
	row['node'] = 'canada';
	row['meas_value'] = math.exp(0.2) * iota_north_america
	data_table.append( copy.copy(row) )
	row['node'] = 'united_states';
	row['meas_value'] = math.exp(-0.2) * iota_north_america
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
		},{ # prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    1e-2 * iota_north_america,
			'upper':    1e+2 * iota_north_america,
			'mean':     iota_north_america,
		},{ # prior_rate_child
			'name':     'prior_rate_child',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      3.0,
		},{ # prior_dtime_parent
			'name':     'prior_dtime_parent',
			'density':  'log_gaussian',
			'mean':     0.0,
			'std':      1.0,
			'eta':      1e-6
		},{ # prior_dtime_child
			'name':     'prior_dtime_child',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      1.0,
			'eta':      1e-6
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	middle_age_id  = 1
	last_time_id   = 2
	smooth_table = [
		{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   [ middle_age_id ],
			'time_id':                  [ 0, last_time_id ],
			'fun':                      fun_rate_parent
		}, { # smooth_rate_child
			'name':                     'smooth_rate_child',
			'age_id':                   [ middle_age_id ],
			'time_id':                  [ 0, last_time_id ],
			'fun':                      fun_rate_child
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
	# --------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'north_america' },
		{ 'name':'ode_step_size',          'value':'1.0'           },
		{ 'name':'random_seed',            'value':'0'             },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },

		{ 'name':'quasi_fixed',            'value':'true'         },
		{ 'name':'derivative_test_fixed',  'value':'first-order'  },
		{ 'name':'max_num_iter_fixed',     'value':'30'           },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-10'        },

		{ 'name':'derivative_test_random', 'value':'second-order' },
		{ 'name':'max_num_iter_random',    'value':'30'           },
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
# ===========================================================================
file_name             = 'example.db'
example_db(file_name)
program               = '../../devel/dismod_at'
for command in [ 'init', 'fit', 'predict' ] :
	cmd = [ program, file_name, command ]
	if command == 'fit' :
		variables = 'both'
		cmd.append(variables)
	if command == 'predict' :
		cmd.append('fit_var')
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# get predict table
predict_table  = dismod_at.get_table_dict(connection, 'predict')
avgint_table   = dismod_at.get_table_dict(connection, 'avgint')
node_table     = dismod_at.get_table_dict(connection, 'node')
#
# check that all the avgint_table values were predicted (no subsetting)
assert len(predict_table) == 3
#
# S(a) = exp( - iota * a )
S_north_america = math.exp( - iota_north_america * 50.0 )
S_canada        = math.exp( - math.exp(0.2) * iota_north_america * 50.0 )
S_united_states = math.exp( - math.exp(-0.2) * iota_north_america * 50.0 )
truth = {
	'north_america' : S_north_america,
	'canada'        : S_canada,
	'united_states' : S_united_states
}
for i in range(3) :
	avgint_id = predict_table[i]['avgint_id']
	node_id   = avgint_table[avgint_id]['node_id']
	node      = node_table[node_id]['node_name']
	check     = truth[node]
	value     = predict_table[i]['avg_integrand']
	assert( abs( value / check - 1.0 ) ) < 1e-2
# -----------------------------------------------------------------------------
print('predict_fit.py: OK')
# END PYTHON
