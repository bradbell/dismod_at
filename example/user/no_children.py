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
# $begin user_no_children.py$$ $newlinech #$$
# $spell
#	init
#	avgint
#	dage
#	dtime
# $$
#
# $section Case with no Children; i.e., no Random Effects$$
#
# $code
# $srcfile%
#	example/user/no_children.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import distutils.dir_util
import subprocess
import copy
test_program = 'example/user/no_children.py'
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
	integrand_list = [
		'prevalence',
		'Sincidence',
		'remission',
		'mtexcess',
		'mtother'
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
		'node':        'canada',
		'weight':      'constant',
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    0.0
	}
	# values that change between rows: (one data point for each integrand)
	for avgint_id in range( len(integrand_list) ) :
		integrand         = integrand_list[avgint_id]
		row['integrand']  = integrand
		if integrand == 'prevalence' :
			# prevalence is measured at age zero
			row['age_upper'] = 0.0
		else :
			# other integrands are averaged from age zero to one hundred
			row['age_upper'] = 100.0
		avgint_table.append( copy.copy(row) )
	# ----------------------------------------------------------------------
	# data table: same order as list of integrands
	data_table = list()
	# values that are the same for all data rows
	row = {
		'node':        'canada',
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    0.0
	}
	# values that change between rows: (one data point for each integrand)
	for integrand_id in range( len(integrand_list) ) :
		rate_id           = integrand_id
		meas_value        = 1e-2 * (rate_id + 1)
		meas_std          = 0.2 * meas_value
		integrand         = integrand_list[integrand_id]
		row['meas_value'] = meas_value
		row['meas_std']   = meas_std
		row['integrand']  = integrand
		if integrand == 'prevalence' :
			# prevalence is measured at age zero
			row['age_upper'] = 0.0
		else :
			# other integrands are averaged from age zero to one hundred
			row['age_upper'] = 100.0
		# data_id = rate_id = integand_id
		data_table.append( copy.copy(row) )
	#
	# add one outlyer at end of data table with hold_out true
	row['hold_out']   = True # if outlyer were false, fit would fail
	row['integrand']  = data_table[0]['integrand']
	row['meas_std']   = data_table[0]['meas_std']
	row['eta']        = None;
	row['age_upper']  = data_table[0]['age_upper']
	row['meas_value'] = 10. * data_table[0]['meas_value']
	data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    1e-4,
			'mean':     1e-1,
		},{ # prior_gauss_zero
			'name':     'prior_gauss_zero',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      1e-2,
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
			'fun':                       fun_rate_parent
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'pini',
			'parent_smooth': 'smooth_rate_parent',
		},{
			'name':          'iota',
			'parent_smooth': 'smooth_rate_parent',
		},{
			'name':          'rho',
			'parent_smooth': 'smooth_rate_parent',
		},{
			'name':          'chi',
			'parent_smooth': 'smooth_rate_parent',
		},{
			'name':          'omega',
			'parent_smooth': 'smooth_rate_parent',
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'canada'       },
		{ 'name':'ode_step_size',          'value':'10.0'         },
		{ 'name':'random_seed',            'value':'0'            },
		{ 'name':'rate_case',              'value':'iota_pos_rho_pos' },

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
	rate_true = []
	for rate_id in range( len( data_table ) ) :
		# for this particular example
		data_id    = rate_id
		meas_value = data_table[data_id]['meas_value']
		rate_true.append(meas_value)
	#
	return (n_smooth, rate_true)
# ===========================================================================
file_name             = 'example.db'
(n_smooth, rate_true) = example_db(file_name)
program               = '../../devel/dismod_at'
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
# get variable and fit_var tables
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
# mulstd variables
for smooth_id in range( n_smooth ) :
	for var_type in [ 'mulstd_value', 'mulstd_dage', 'mulstd_dtime' ] :
		count = 0
		for var_id in range( len(var_table) ) :
			row   = var_table[var_id]
			match = row['var_type'] == var_type
			match = match and row['smooth_id'] == smooth_id
			if match :
				count += 1
				fit_var_id     = var_id
				fit_var_value  = fit_var_table[fit_var_id]['fit_var_value']
				assert fit_var_value  == 1.0
		assert count == 0
#
# rate variables
parent_node_id = 3
check_tol      = 1e-3
n_rate         = 5;
for rate_id in range(n_rate) :
	count = 0
	for var_id in range( len(var_table) ) :
		row   = var_table[var_id]
		match = row['var_type'] == 'rate'
		match = match and row['rate_id'] == rate_id
		if match :
			assert row['node_id'] == parent_node_id
			count         += 1
			check          = rate_true[rate_id]
			fit_var_id     = var_id
			fit_var_value   = fit_var_table[fit_var_id]['fit_var_value']
			err            = fit_var_value  / check - 1.0
			assert abs(err) <= check_tol
	# There is one age point and two time points for each rate
	assert count == 2
# -----------------------------------------------------------------------------
print('no_children.py: OK')
# END PYTHON
