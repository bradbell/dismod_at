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
# $begin diabetes.py$$ $newlinech #$$
# $spell
# $$
#
# $section A Diabetes Example$$
#
# $code
# $srcfile%
#	test/user/diabetes.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
import distutils.dir_util
import subprocess
test_program = 'test/user/diabetes.py'
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
# ---------------------------------------------------------------------------
# note that the a, t values are not used for this example
def fun_one(a, t) :
	return 1.0
def fun_rate(a, t) :
	return ('prior_rate', 'prior_difference', 'prior_difference')
def fun_omega(a, t) :
	if a == 10 :
		return ('prior_omega_10', 'prior_difference', 'prior_difference')
	if a == 40 :
		return ('prior_omega_40', 'prior_difference', 'prior_difference')
	if a == 65 :
		return ('prior_omega_65', 'prior_difference', 'prior_difference')
	if a == 80 :
		return ('prior_omega_80', 'prior_difference', 'prior_difference')
	assert False
def fun_mulcov_std(a, t) :
	return ('prior_zero', 'prior_difference', 'prior_difference')
def fun_mulcov_rate(a, t) :
	return ('prior_zero', 'prior_difference', 'prior_difference')
# ---------------------------------------------------------------------------
def data_list2dict(value_list) :
	key_list = [
		'integrand',  'density',    'node',      'weight',    'hold_out',
		'meas_value', 'meas_std',   'age_lower', 'age_upper',
		'time_lower', 'time_upper', 'sex',       'one'
	]
	assert len(value_list) == len(key_list)
	dict_out = dict()
	for i in range( len(value_list) ) :
		dict_out[key_list[i]] = value_list[i]
	return copy.copy( dict_out )
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ----------------------------------------------------------------------
	# age table
	#             [ 4 ages for omega, 2 more for rates  ]
	age_list    = [ 10.0, 40.0, 65.0, 80.0, 0.0, 100.0 ]
	#
	# time table
	time_list   = [ 1990.0, 2015.0 ]
	#
	# integrand table
	integrand_dict = [
		{ 'name':'Sincidence',  'eta':1e-6 },
		{ 'name':'prevalence',  'eta':1e-6 },
		{ 'name':'mtexcess',    'eta':1e-6 },
		{ 'name': 'relrisk',    'eta':1e-6}
	]
	#
	# node table: world
	node_dict = [
		{ 'name':'world',         'parent':'' }
	]
	#
	# weight table: The constant function 1.0 (one age and one time point)
	weight_dict = [
		{ 'name':'weight_one',  'age_id':[1], 'time_id':[1], 'fun':fun_one }
	]
	#
	# covariate table:
	covariate_dict = [
		{'name':'sex', 'reference':0.0, 'max_difference':0.6 },
		{'name':'one', 'reference':0.0, 'max_difference':None}
	]
	#
	# mulcov table
	mulcov_dict = list()
	for integrand in integrand_dict :
		mulcov_dict.append( {
			'covariate': 'one',
			'type':      'meas_std',
			'effected':  integrand['name'],
			'smooth':    'smooth_mulcov_std'
		} )
	for rate in ['iota', 'chi'] :
		mulcov_dict.append( {
			'covariate': 'sex',
			'type':      'rate_value',
			'effected':  rate,
			'smooth':    'smooth_mulcov_rate'
		} )
	#
	# --------------------------------------------------------------------------
	# data table: order for value_list
	#	'integrand',  'density',    'node',      'weight', '     hold_out',
	#	'meas_value', 'meas_std',   'age_lower', 'age_upper',
	#   'time_lower', 'time_upper', 'sex',       'one'
	data_dict = list()
	#
	# Sincidence
	value_list = [
		'Sincidence', 'log_gaussian', 'world',    'weight_one',  0,
		4.0e-3,       2.0e-3,         40,          40,
		1995,         1995,           0.0,         1
	]
	data_dict.append( data_list2dict( value_list ) );
	#
	value_list = [
		'Sincidence', 'log_gaussian', 'world',    'weight_one',  0,
		4.0e-2,       2.0e-2,         80,          80,
		1995,         1995,           0.0,         1
	]
	data_dict.append( data_list2dict( value_list ) );
	#
	# mtexcess
	value_list = [
		'mtexcess',   'log_gaussian', 'world',    'weight_one',  0,
		1.6e-4,       0.8e-4,         40,          40,
		1995,         1995,           0.0,         1
	]
	data_dict.append( data_list2dict( value_list ) );
	value_list = [
		'mtexcess',   'log_gaussian', 'world',    'weight_one',  0,
		1.6e-3,       0.8e-3,         80,          80,
		1995,         1995,           0.0,         1
	]
	data_dict.append( data_list2dict( value_list ) );
	#
	# prevalence
	value_list = [
		'prevalence', 'log_gaussian', 'world',    'weight_one',  0,
		1.0e-1,       0.5e-1,         40,          40,
		1995,         1995,           0.0,         1
	]
	data_dict.append( data_list2dict( value_list ) );
	value_list = [
		'prevalence', 'log_gaussian', 'world',    'weight_one',  0,
		5.0e-1,       2.5e-1,         80,          80,
		1995,         1995,           0.0,         1
	]
	data_dict.append( data_list2dict( value_list ) );
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
		},{ # prior_rate
			'name':     'prior_rate',
			'density':  'log_gaussian',
			'lower':    1e-5,
			'upper':    1e+0,
			'mean':     1e-2,
			'std':      1e-0,
			'eta':      1e-5
		},{ # prior_difference
			'name':     'prior_difference',
			'density':  'log_gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      1.0,
			'eta':      1e-5
		},{ # prior_omega_10
			'name':     'prior_omega_10',
			'density':  'uniform',
			'lower':    0.0001,
			'upper':    0.0001,
			'mean':     0.0001,
			'std':      None,
			'eta':      None
		},{ # prior_omega_40
			'name':     'prior_omega_40',
			'density':  'uniform',
			'lower':    0.001,
			'upper':    0.001,
			'mean':     0.001,
			'std':      None,
			'eta':      None
		},{ # prior_omega_65
			'name':     'prior_omega_65',
			'density':  'uniform',
			'lower':    0.01,
			'upper':    0.01,
			'mean':     0.01,
			'std':      None,
			'eta':      None
		},{ # prior_omega_80
			'name':     'prior_omega_80',
			'density':  'uniform',
			'lower':    0.04,
			'upper':    0.04,
			'mean':     0.04,
			'std':      None,
			'eta':      None
		}
	]
	# --------------------------------------------------------------------------
	# smooth table
	smooth_dict = [
		{
		# smooth_rate
			'name':                     'smooth_rate',
			'age_id':                   [ 0, 1, 2, 3, 4, 5],
			'time_id':                  [0],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                      fun_rate
		},{
		# smooth_omega
			'name':                     'smooth_omega',
			'age_id':                   [0, 1, 2, 3],
			'time_id':                  [0],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                      fun_omega
		},{
		# smooth_mulcov_std
			'name':                     'smooth_mulcov_std',
			'age_id':                   [0],
			'time_id':                  [0],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                      fun_mulcov_std
		},{
		# smooth_mulcov_rate
			'name':                     'smooth_mulcov_rate',
			'age_id':                   [0],
			'time_id':                  [0],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                      fun_mulcov_rate
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
			'parent_smooth': 'smooth_rate',
			'child_smooth':  None
		},{
			'name':          'rho',
			'parent_smooth': None,
			'child_smooth':  None
		},{
			'name':          'chi',
			'parent_smooth': 'smooth_rate',
			'child_smooth':  None
		},{
			'name':          'omega',
			'parent_smooth': 'smooth_omega',
			'child_smooth':  None
		}
	]
	# ------------------------------------------------------------------------
	# option_dict
	option_dict = [
		{ 'name':'parent_node_name',       'value':'world'             },
		{ 'name':'number_simulate',        'value':'1'                 },
		{ 'name':'fit_simulate_index',     'value':None                },
		{ 'name':'ode_step_size',          'value':'10.0'              },
		{ 'name':'random_seed',            'value':'0'                 },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },
		#
		{ 'name':'quasi_fixed',            'value':'false'             },
		{ 'name':'derivative_test_fixed',  'value':'first-order'       },
		{ 'name':'max_num_iter_fixed',     'value':'100'               },
		{ 'name':'print_level_fixed',      'value':'0'                 },
		{ 'name':'tolerance_fixed',        'value':'1e-8'              },
		{ 'name':'fixed_bound_frac',       'value':'1e-2'              },
		#
		{ 'name':'random_bound',           'value':None                },
		{ 'name':'derivative_test_random', 'value':'second-order'      },
		{ 'name':'max_num_iter_random',    'value':'100'               },
		{ 'name':'print_level_random',     'value':'0'                 },
		{ 'name':'tolerance_random',       'value':'1e-10'             }
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
for command in [ 'init', 'start', 'fit', 'sample' ] :
	cmd = [ program, file_name, command ]
	if command == 'start' :
		cmd.append('prior_mean')
	if command == 'sample' :
		cmd.append('asymptotic')
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
# -----------------------------------------------------------------------------
print('diabetes.py: OK')
# END PYTHON
