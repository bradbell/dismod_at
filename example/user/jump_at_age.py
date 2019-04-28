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
# $begin user_jump_at_age.py$$ $newlinech #$$
# $spell
#	init
#	Sincidence
# $$
#
# $section Jump in Rate Value at a Known Age$$
#
# $head Purpose$$
# Usually the prior for the rate is smooth.
# This requires lots of data, at a fine age spacing,
# to resolve a jump in a rate at an unknown age.
# If one know the age at which a jump occurs, it is possible to resolve
# the jump with less data by specifying a prior that has this knowledge.
#
# $head Model Variables$$
# This example's variables are all
# $cref/parent rates/model_variables/Fixed Effects, theta/Parent Rates/$$
# for $cref/iota/rate_table/rate_name/iota/$$.
#
# $head Truth$$
# For this example the rate $icode iota$$ is one constant
# for ages less that 20, and a different constant for ages greater than 20.
#
# $head Simulated Data$$
# For this example, the simulated data is all
# $cref/Sincidence/avg_integrand/Integrand, I_i(a,t)/Sincidence/$$; i.e.
# direct measurements of the value of $icode iota$$.
# There is no noise simulated with the data; i.e., it is equal to the
# 'true' value of $icode iota$$.
# On the other hand, its is modeled as if there is a 10% coefficient
# of variation in the data; i.e., as if there were measurement noise with
# standard deviation equal to 10% of the measurement value.
#
# $head Model Ages$$
# The value of $icode iota$$ is modeled at the following ages:
# 0, 20, 21, 100. The prior for $icode iota$$ is modeled as being smooth
# all ages except age 20. This is done using a Log-Gaussian for the differences
# except the difference from age 20 to age 21 where a uniform prior is used.
#
# $head Age Table$$
# The $cref age_table$$ does not need to be monotone increasing.
# For this example, it is the same as the table of ages at which
# $icode iota$$ is modeled and is called $code model_age_list$$ below.
# You can change the order of this table and the example / test
# will still run correctly.
#
# $head Time Table$$
# The $cref time_table$$ does not need to be monotone increasing.
# You can the order of $code time_table$$ below and it will
# not affect the results.
#
# $head Source Code$$
# $srcfile%
#	example/user/jump_at_age.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
iota_before_20  = 1e-4
iota_after_20   = 1e-1
model_age_list  = [ 0.0, 100.0, 21.0, 20.0 ]
time_table      = [ 2015.0, 1995.0 ]
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
test_program = 'example/user/jump_at_age.py'
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
# ----------------------------------------------------------------------------
# run a system command
def system_command(command) :
	print( ' '.join(command) )
	flag = subprocess.call( command )
	if flag != 0 :
		sys.exit('command failed: flag = ' + str(flag))
	return
# ------------------------------------------------------------------------
def iota_true(age) :
	if age <= 20.0 :
		return iota_before_20
	else :
		return iota_after_20
# ------------------------------------------------------------------------
# Note that the a, t values are used for this example
def example_db (file_name) :
	def constant_weight_fun(a, t) :
		return 1.0
	#
	def fun_iota_parent(a, t) :
		if 19.5 <= a and a <= 20.5 :
			return ('prior_rate_parent', 'prior_none', 'prior_difference')
		else :
			return ('prior_rate_parent', 'prior_difference', 'prior_difference')
	# ----------------------------------------------------------------------
	# age table (in age_list above)
	age_list = model_age_list
	#
	# time table
	time_list   = time_table
	#
	# integrand table
	integrand_table = [
		{ 'name':'Sincidence' }
	]
	#
	# node table: world
	node_table = [ { 'name':'world',         'parent':'' } ]
	#
	# weight table: The constant function 1.0 (one age and one time point)
	fun = constant_weight_fun
	weight_table = [
		{ 'name':'constant',  'age_id':[0], 'time_id':[0], 'fun':fun }
	]
	#
	# covariate table:
	covariate_table = list()
	#
	# mulcov table
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
	#
	# values that are the same for all data rows
	row = {
		'node':        'world',
		'density':     'log_gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   time_list[0],
		'time_upper':   time_list[0]
	}
	# Sincidence data
	data_age_list  = [ 0.0, 1.0, 5.0, 10.0, 20.0, 40.0, 80.0, 100.0 ]
	for age in data_age_list :
		meas_value = iota_true(age)
		row['age_lower']    = age
		row['age_upper']    = age
		row['integrand']    = 'Sincidence'
		row['meas_value']   = meas_value
		row['meas_std']     = meas_value * 0.1
		row['eta']          = 1e-6;
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_none
			'name':     'prior_none',
			'density':  'uniform',
			'mean':     0.0,
		},{ # prior_difference
			'name':     'prior_difference',
			'density':  'log_gaussian',
			'mean':     0.0,
			'std':      0.1,
			'eta':      1e-4
		},{ # prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    1e-5,
			'upper':    1.0,
			'mean':     0.1,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	#
	smooth_table = [
		{ # smooth_iota_parent
			'name':                     'smooth_iota_parent',
			'age_id':                   range( len(model_age_list) ),
			'time_id':                  [0],
			'fun':                      fun_iota_parent
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'world'             },
		{ 'name':'ode_step_size',          'value':'1.0'               },
		{ 'name':'random_seed',            'value':'0'                 },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },

		{ 'name':'quasi_fixed',            'value':'true'              },
		{ 'name':'derivative_test_fixed',  'value':'first-order'       },
		{ 'name':'max_num_iter_fixed',     'value':'200'               },
		{ 'name':'print_level_fixed',      'value':'0'                 },
		{ 'name':'tolerance_fixed',        'value':'1e-6'              },

		{ 'name':'derivative_test_random', 'value':'second-order'      },
		{ 'name':'max_num_iter_random',    'value':'100'               },
		{ 'name':'print_level_random',     'value':'0'                 },
		{ 'name':'tolerance_random',       'value':'1e-10'             }
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
# ===========================================================================
file_name = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
system_command([ program, file_name, 'init' ])
system_command([ program, file_name, 'fit', 'both' ])
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# Results for fitting with no noise
var_table     = dismod_at.get_table_dict(connection, 'var')
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
#
# check rates values
iota_rate_id      = 1
max_err           = 0.0
tolerance         = 1e-3
age_list          = model_age_list
for var_id in range( len(var_table) ) :
	row   = var_table[var_id]
	assert row['var_type'] == 'rate'
	assert row['node_id']  == 0
	age    = age_list[ row['age_id'] ]
	rate_id = row['rate_id']
	value  = fit_var_table[var_id]['fit_var_value']
	assert rate_id == iota_rate_id
	value_true = iota_true(age)
	rate       = 'iota'
	max_err = max(max_err, abs( value / value_true - 1.0 ) )
	if( abs(value / value_true - 1.0) > tolerance ) :
		print(rate, age, value / value_true - 1.0 )
assert max_err <= tolerance
# -----------------------------------------------------------------------------
print('jump_at_age.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
