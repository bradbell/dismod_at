# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-19 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_re_scale.py$$ $newlinech #$$
# $spell
#	init
#	avgint
#	dage
#	dtime
# $$
#
# $section Case Where Re-Scaling is Useful$$
#
# $head Source Code$$
# $srcfile%
#	example/user/re_scale.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import distutils.dir_util
import copy
test_program = 'example/user/re_scale.py'
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
def example_db (file_name) :
	def fun_rate_parent(a, t) :
		return ('prior_rate_parent', 'prior_gauss_zero', None)
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
	# node table: world -> north_america
	#             north_america -> (united_states, canada)
	node_table = [
		{ 'name':'world',         'parent':'' },
		{ 'name':'north_america', 'parent':'world' },
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
	# nslist_table:
	nslist_table = dict()
	#
	# avgint_table
	avgint_table = list()
	# ----------------------------------------------------------------------
	# data table: same order as age_list
	data_table = list()
	# values that are the same for all data rows
	row = {
		'node':        'canada',
		'density':     'gaussian',
		'weight':      '',
		'hold_out':     False,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'integrand':   'Sincidence',
		'age_lower':    0.0
	}
	# values that change between rows: (one data point for each integrand)
	for age_id in range( len(age_list) ) :
		age               = age_list[age_id]
		meas_value        = 1e-4 * (50.0 + age)
		row['meas_value'] = meas_value
		row['meas_std']   = 1e-4 * (50.0 + age_list[0])
		row['age_lower']  = age
		row['age_upper']  = age
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    1e-4,
			'upper':    1.0,
			'mean':     0.01,
		},{ # prior_gauss_zero
			'name':     'prior_gauss_zero',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      1e-6,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	smooth_table = [
		{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   range( len(age_list) ),
			'time_id':                  [ 0 ],
			'fun':                      fun_rate_parent
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'iota',
			'parent_smooth': 'smooth_rate_parent',
		}
	]
	# ----------------------------------------------------------------------
	# option_table: max_num_iter_fixed will be set later
	option_table = [
		{ 'name':'parent_node_name',       'value':'canada'       },
		{ 'name':'ode_step_size',          'value':'10.0'         },
		{ 'name':'random_seed',            'value':'0'            },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },
		{ 'name':'warn_on_stderr',         'value':'false'        },

		{ 'name':'quasi_fixed',            'value':'true'         },
		{ 'name':'derivative_test_fixed',  'value':'first-order'  },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-12'        },

		{ 'name':'derivative_test_random', 'value':'second-order' },
		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-10'        }
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
	return
# ===========================================================================
# create the database
file_name  = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([
	program, file_name, 'set', 'option', 'max_num_iter_fixed', '1'
])
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
dismod_at.system_command_prc([
	program, file_name, 'set', 'scale_var', 'fit_var'
])
dismod_at.system_command_prc([
	program, file_name, 'set', 'option', 'max_num_iter_fixed', '30'
])
dismod_at.system_command_prc([
	program, file_name, 'set', 'option', 'warn_on_stderr', 'true'
])
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
#
# get tables
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
age_table       = dismod_at.get_table_dict(connection, "age")
log_table       = dismod_at.get_table_dict(connection, "log")
#
# check that convergence was detected during final fit by making
# sure there are no warnings during the fit
fit_log_id = None
for log_id in range( len(log_table) ) :
	if log_table[log_id]['message'] == 'begin fit both' :
		fit_log_id = log_id
assert log_table[fit_log_id + 1]['message'] == 'end fit'
#
# rate variables
assert len(age_table) == 3
iota_optimal = 1e-4 * (50.0 + age_table[1]['age'])
for var_id in range( len(var_table) ) :
	iota_fit   = fit_var_table[var_id]['fit_var_value']
	assert abs( iota_fit / iota_optimal - 1.0 ) < 1e-4
# -----------------------------------------------------------------------------
print('re_scale.py: OK')
# END PYTHON
