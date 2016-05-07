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
# $begin user_no_children.py$$ $newlinech #$$
# $spell
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
sys.path.append( os.getcwd() + '/python' )
import dismod_at
#
# change into the build/example/user directory
distutils.dir_util.mkpath('build/example/user')
os.chdir('build/example/user')
# ---------------------------------------------------------------------------
# note that the a, t values are not used for this example
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
	integrand_dict = [
		{ 'name':'prevalence',  'eta':1e-6 },
		{ 'name':'Sincidence',  'eta':1e-6 },
		{ 'name':'remission',   'eta':1e-6 },
		{ 'name':'mtexcess',    'eta':1e-6 },
		{ 'name':'mtother',     'eta':1e-6 }
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
	for integrand_id in range( len(integrand_dict) ) :
		rate_id           = integrand_id
		meas_value        = 1e-2 * (rate_id + 1)
		meas_std          = 0.2 * meas_value
		integrand         = integrand_dict[integrand_id]['name']
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
		data_dict.append( copy.copy(row) )
	#
	# add one outlyer at end of data table with hold_out true
	row['hold_out']   = True # if outlyer were false, fit would fail
	row['integrand']  = data_dict[0]['integrand']
	row['meas_std']   = data_dict[0]['meas_std']
	row['age_upper']  = data_dict[0]['age_upper']
	row['meas_value'] = 10. * data_dict[0]['meas_value']
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
		},{ # prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    1e-4,
			'upper':    None,
			'mean':     1e-1,
			'std':      None,
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
		{ # smooth_rate_parent
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
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  None
		},{
			'name':          'iota',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  None
		},{
			'name':          'rho',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  None
		},{
			'name':          'chi',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  None
		},{
			'name':          'omega',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  None
		}
	]
	# ------------------------------------------------------------------------
	# option_dict
	option_dict = [
		{ 'name':'parent_node_name',       'value':'canada'       },
		{ 'name':'number_simulate',        'value':'1'            },
		{ 'name':'fit_simulate_index',     'value':None           },
		{ 'name':'ode_step_size',          'value':'10.0'         },
		{ 'name':'random_seed',            'value':'0'            },
		{ 'name':'rate_case',              'value':'iota_pos_rho_pos' },

		{ 'name':'quasi_fixed',            'value':'true'         },
		{ 'name':'derivative_test_fixed',  'value':'first-order'  },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-10'        },
		{ 'name':'random_bound',           'value':None           },

		{ 'name':'derivative_test_random', 'value':'second-order' },
		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-10'        }
	]
	# --------------------------------------------------------------------------
	# avgint table: same order as list of integrands
	avgint_dict = list()
	# values that are the same for all data rows
	row = {
		'node':        'canada',
		'weight':      'constant',
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    0.0
	}
	# values that change between rows: (one data point for each integrand)
	for avgint_id in range( len(integrand_dict) ) :
		integrand         = integrand_dict[avgint_id]['name']
		row['integrand']  = integrand
		if integrand == 'prevalence' :
			# prevalence is measured at age zero
			row['age_upper'] = 0.0
		else :
			# other integrands are averaged from age zero to one hundred
			row['age_upper'] = 100.0
		avgint_dict.append( copy.copy(row) )
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
	rate_true = []
	for rate_id in range( len( data_dict ) ) :
		# for this particular example
		data_id    = rate_id
		meas_value = data_dict[data_id]['meas_value']
		rate_true.append(meas_value)
	#
	return (n_smooth, rate_true)
# ===========================================================================
file_name             = 'example.db'
(n_smooth, rate_true) = example_db(file_name)
program               = '../../devel/dismod_at'
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
#
# mulstd variables
for smooth_id in range( n_smooth ) :
	for var_type in [ 'mulstd_value', 'mulstd_dage', 'mulstd_dtime' ] :
		count = 0
		for var_id in range( len(var_dict) ) :
			row   = var_dict[var_id]
			match = row['var_type'] == var_type
			match = match and row['smooth_id'] == smooth_id
			if match :
				count += 1
				fit_var_id     = var_id
				variable_value = fit_var_dict[fit_var_id]['variable_value']
				assert variable_value == 1.0
		assert count == 0
#
# rate variables
parent_node_id = 3
check_tol      = 1e-3
n_rate         = 5;
for rate_id in range(n_rate) :
	count = 0
	for var_id in range( len(var_dict) ) :
		row   = var_dict[var_id]
		match = row['var_type'] == 'rate'
		match = match and row['rate_id'] == rate_id
		if match :
			assert row['node_id'] == parent_node_id
			count         += 1
			check          = rate_true[rate_id]
			fit_var_id     = var_id
			variable_value  = fit_var_dict[fit_var_id]['variable_value']
			err            = variable_value / check - 1.0
			assert abs(err) <= check_tol
	# There is one age point and two time points for each rate
	assert count == 2
# -----------------------------------------------------------------------------
print('no_children.py: OK')
# END PYTHON
