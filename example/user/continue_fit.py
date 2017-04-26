#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_continue_fit.py$$ $newlinech #$$
# $spell
#	num_iter
#	stderr
#	py
#	const
# $$
#
# $section Continuing a Fit From Where it Left Off$$
#
# $head Option Table$$
# In the option table defined below,
# $icode%max_num_iter_fixed% = 5%$$.
# This fit will terminate when
# the maximum number of iterations is reached.
# The corresponding warning is suppressed by setting
# $icode%warn_on_stderr% = false%$$.
# The second fit will start where the first left off.
# To see this, set $icode%print_level_fixed% = 5%$$ (in the option table)
# and execute the command
# $codei%
#	%python% example/user/continue_fit.py
# %$$
# $icode python$$ is your
# $cref/python3_executable/run_cmake.sh/python3_executable/$$.
#
# $code
# $srcfile%
#	example/user/continue_fit.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# values used to simulate data
iota_true                 = 0.01
chi_true                  = 0.1
n_data                    = 51
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
test_program = 'example/user/continue_fit.py'
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
def constant_weight_fun(a, t) :
	return 1.0
# note that the a, t values are not used for this case
def fun_iota(a, t) :
	return ('prior_iota', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_chi(a, t) :
	return (chi_true, 'prior_gauss_zero', 'prior_gauss_zero')
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ 0.0, 5.0, 15.0, 35.0, 50.0, 75.0, 90.0, 100.0 ]
	#
	# time table:
	time_list   = [ 1990.0, 2000.0, 2010.0, 2200.0 ]
	#
	# integrand table:
	integrand_list = [ 'prevalence' ]
	#
	# node table:
	node_table = [ { 'name':'world', 'parent':'' } ]
	#
	# weight table:
	# The constant function 1.0, note any valid age and time id would work
	name    = 'constant'
	fun     = constant_weight_fun
	age_id  = int( len(age_list) / 2 )
	time_id = int( len(time_list) / 2 )
	weight_table = [
		{ 'name':name,  'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	]
	#
	# covariate table:
	covariate_table = list()
	#
	# mulcov table:
	mulcov_table = list()
	#
	# avgint table: empty
	avgint_table = list()
	#
	# nslist_table:
	nslist_table = dict()
	# ----------------------------------------------------------------------
	# data table:
	data_table = list()
	# values that are the same for all data rows
	row = {
		'meas_value':  0.0,             # not used (will be simulated)
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   2000.,
		'time_upper':   2000.
	}
	# values that change between rows:
	for data_id in range( n_data ) :
		fraction         = data_id / float(n_data-1)
		age              = age_list[0] + (age_list[-1] - age_list[0])*fraction
		row['age_lower'] = age
		row['age_upper'] = age
		row['node']      = 'world'
		row['integrand'] = 'prevalence'
		row['meas_std']  = 0.01
		row['eta']       = None;
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_gauss_zero
			'name':     'prior_gauss_zero',
			'density':  'gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      0.01,
			'eta':      None
		},{ # prior_iota
			'name':     'prior_iota',
			'density':  'uniform',
			'lower':    iota_true / 10.,
			'upper':    iota_true * 10.,
			'mean':     iota_true * 2.0,
			'std':      None,
			'eta':      None
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	name           = 'smooth_iota'
	fun            = fun_iota
	age_id         = int( len( age_list ) / 2 )
	time_id        = int( len( time_list ) / 2 )
	smooth_table = [
		{	'name':name,
			'age_id':[age_id],
			'time_id':[time_id],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':fun
		}
	]
	name           = 'smooth_chi'
	fun            = fun_chi
	age_id         = int( len( age_list ) / 2 )
	time_id        = int( len( time_list ) / 2 )
	smooth_table .append(
		{	'name':name,
			'age_id':[age_id],
			'time_id':[time_id],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':fun
		}
	)
	# ----------------------------------------------------------------------
	# rate table:
	rate_table = [
		{	'name':          'pini',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		},{	'name':          'iota',
			'parent_smooth': 'smooth_iota',
			'child_smooth':  None,
			'child_nslist':  None
		},{	'name':          'rho',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		},{	'name':          'chi',
			'parent_smooth': 'smooth_chi',
			'child_smooth':  None,
			'child_nslist':  None
		},{	'name':          'omega',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero'   },
		{ 'name':'parent_node_name',       'value':'world'               },
		{ 'name':'ode_step_size',          'value':'10.0'                },
		{ 'name':'random_seed',            'value':'0'                   },
		{ 'name':'warn_on_stderr',         'value':'false'               },

		{ 'name':'quasi_fixed',            'value':'true'                },
		{ 'name':'max_num_iter_fixed',     'value':'5'                   },
		{ 'name':'print_level_fixed',      'value':'0'                   },
		{ 'name':'tolerance_fixed',        'value':'1e-7'                },

		{ 'name':'max_num_iter_random',    'value':'50'                  },
		{ 'name':'print_level_random',     'value':'0'                   },
		{ 'name':'tolerance_random',       'value':'1e-10'               }
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
	return
# ===========================================================================
# Run the init command to create the var table
file_name      = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
cmd            = [ program, file_name, 'init' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at init command failed')
# -----------------------------------------------------------------------
# read database
new           = False
connection    = dismod_at.create_connection(file_name, new)
var_table     = dismod_at.get_table_dict(connection, 'var')
rate_table    = dismod_at.get_table_dict(connection, 'rate')
# -----------------------------------------------------------------------
# truth table:
tbl_name     = 'truth_var'
col_name     = [ 'truth_var_value' ]
col_type     = [ 'real' ]
row_list     = list()
var_id2true  = list()
for var_id in range( len(var_table) ) :
	var_info        = var_table[var_id]
	truth_var_value = None
	var_type = var_info['var_type']
	assert var_type == 'rate'
	rate_id   = var_info['rate_id']
	rate_name = rate_table[rate_id]['rate_name']
	if rate_name == 'iota' :
		value = iota_true
	elif rate_name == 'chi' :
		value = chi_true
	else :
		assert False
	row_list.append( [ value ] )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
connection.close()
# -----------------------------------------------------------------------
# Simulate one data set, start at prior mean fit, start at fit results, fit
fit_count = 0
for command in [ 'simulate', 'start', 'fit', 'start', 'fit' ] :
	cmd = [ program, file_name, command ]
	if command == 'simulate' :
		number_simulate = '1'
		cmd.append(number_simulate)
	if command == 'start' :
		if fit_count == 0 :
			cmd.append('prior_mean')
		else :
			cmd.append('fit_var')
	if command == 'fit' :
		variables = 'both'
		cmd.append(variables)
		fit_count += 1
	if command == 'fit' :
		simulate_index = '0';
		cmd.append(simulate_index)
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# check fit results
new           = False
connection    = dismod_at.create_connection(file_name, new)
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
log_table     = dismod_at.get_table_dict(connection, 'log' )
connection.close()
#
# check that we got one warning
warning_count = 0
for row in log_table :
	if row['message_type'] == 'warning' :
		warning_count += 1
assert warning_count == 1
#
max_error    = 0.0
for var_id in range( len(var_table) ) :
	fit_value = fit_var_table[var_id]['variable_value']
	var_row   = var_table[var_id]
	rate_id   = var_row['rate_id']
	rate_name = rate_table[rate_id]['rate_name']
	if rate_name == 'iota' :
		ok = abs( fit_value / iota_true - 1.0 ) < .05
		if not ok :
			print( "iota relative error = ", fit_value / iota_true )
			assert abs( fit_value / iota_true - 1.0 ) < .05
	else :
		assert fit_value == chi_true
# -----------------------------------------------------------------------------
print('continue_fit.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
