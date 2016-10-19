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
# $begin user_predict_fit.py$$ $newlinech #$$
# $spell
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
test_program = 'example/user/predict_fit.py'
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
# ---------------------------------------------------------------------------
# note that the a, t values are not used for this example
# note that the a, t values are not used for this example
def constant_weight_fun(a, t) :
	return 1.0
def fun_zero(a, t) :
	return ('prior_zero', 'prior_zero', 'prior_zero')
def fun_one(a, t) :
	return ('prior_one', 'prior_one', 'prior_one')
def fun_rate_child(a, t) :
	return ('prior_gauss_zero', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_rate_parent(a, t) :
	return ('prior_rate_parent', 'prior_gauss_zero', 'prior_gauss_zero')
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
	integrand_table = [
		{ 'name':'Sincidence',  'eta':1e-6 }
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
	# --------------------------------------------------------------------------
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
	row['meas_value'] = 2.0 * iota_north_america
	data_table.append( copy.copy(row) )
	row['node'] = 'united_states';
	row['meas_value'] = 0.5 * iota_north_america
	data_table.append( copy.copy(row) )
	#
	for data_id in range( len( data_table ) ) :
		data_table[data_id]['data_name'] = 'd' + str(data_id)
	# --------------------------------------------------------------------------
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
		},{ # prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    1e-2 * iota_north_america,
			'upper':    1e+2 * iota_north_america,
			'mean':     iota_north_america,
			'std':      None,
			'eta':      None
		},{ # prior_gauss_zero
			'name':     'prior_gauss_zero',
			'density':  'gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      5.0, # large so get close fit to data
			'eta':      None
		}
	]
	# --------------------------------------------------------------------------
	# smooth table
	middle_age_id  = 1
	last_time_id   = 2
	smooth_table = [
		{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   [ middle_age_id ],
			'time_id':                  [ 0, last_time_id ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                      fun_rate_parent
		}, { # smooth_rate_child
			'name':                     'smooth_rate_child',
			'age_id':                   [ middle_age_id ],
			'time_id':                  [ 0, last_time_id ],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                      fun_rate_child
		}
	]
	# --------------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'pini',
			'parent_smooth': None,
			'child_smooth':  None
		},{
			'name':          'iota',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child',
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
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'north_america' },
		{ 'name':'fit_simulate_index',     'value':None            },
		{ 'name':'ode_step_size',          'value':'10.0'          },
		{ 'name':'random_seed',            'value':'0'             },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },

		{ 'name':'quasi_fixed',            'value':'true'         },
		{ 'name':'derivative_test_fixed',  'value':'first-order'  },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-10'        },

		{ 'name':'derivative_test_random', 'value':'second-order' },
		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-10'        }
	]
	# --------------------------------------------------------------------------
	# avgint table: same order as list of integrands
	avgint_table = list()
	# values that are the same for all data rows
	row = {
		'integrand':   'Sincidence',
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
	# --------------------------------------------------------------------------
	# create database
	dismod_at.create_database(
		file_name,
		age_list,
		time_list,
		integrand_table,
		node_table,
		weight_table,
		covariate_table,
		data_table,
		prior_table,
		smooth_table,
		rate_table,
		mulcov_table,
		option_table,
		avgint_table
	)
# ===========================================================================
file_name             = 'example.db'
example_db(file_name)
program               = '../../devel/dismod_at'
for command in [ 'init', 'start', 'fit', 'sample', 'predict' ] :
	cmd = [ program, file_name, command ]
	if command == 'simulate' :
		number_simulate = '1'
		cmd.append(number_simulate)
	if command == 'start' :
		cmd.append('prior_mean')
	if command == 'sample' :
		cmd.append('fit_var')
		cmd.append('1') # number_sample must be 1 for fit_var method
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
predict_table    = dismod_at.get_table_dict(connection, 'predict')
avgint_table   = dismod_at.get_table_dict(connection, 'avgint')
node_table     = dismod_at.get_table_dict(connection, 'node')
subset_dict     = dismod_at.get_table_dict(connection, 'avgint_subset')
#
# check that all the avgint_table values were predicted (no subsetting)
assert len(predict_table) == 3
#
truth = {
	'north_america' : iota_north_america,
	'canada' : 2.0 * iota_north_america,
	'united_states' : 0.5 * iota_north_america
}
for i in range(3) :
	subset_id = predict_table[i]['avgint_subset_id']
	avgint_id = subset_dict[subset_id]['avgint_id']
	node_id   = avgint_table[avgint_id]['node_id']
	node      = node_table[node_id]['node_name']
	check     = truth[node]
	value     = predict_table[i]['avg_integrand']
	assert( abs( value / check - 1.0 ) ) < 1e-2
# -----------------------------------------------------------------------------
print('predict_fit.py: OK')
# END PYTHON
