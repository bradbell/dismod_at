#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-19 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_censor.py$$ $newlinech #$$
# $spell
#	init
#	avgint
#	Covariates
#	covariate
#	smoothings
#	mulcov
#	exp
#	Sincidence
#	std
#	cen
# $$
#
# $section Fitting Data That Has Negative Values Censored$$
#
# $head Purpose$$
# This example uses the
# $cref/cen_gaussian/density_table/density_name/cen_gaussian/$$
# density to represent data that is has its negative value replaced by zero.
#
# $head Problem Parameters$$
# The following values are used to simulate the data and set the priors
# for fitting the data:
# $srcfile%example/user/censor.py%
#	0%# begin problem parameters%# end problem parameters%1
# %$$
#
# $head Age and Time Values$$
# The age and time values do not matter for this problem
# because all the functions are constant in age and time.
# This can be seen by the fact that all of the smoothings have one age
# and one time point.
#
# $head Variables$$
# There is one model variable for this example $icode iota_true$$
# $cref/iota/avg_integrand/Rate Functions/iota_i(a,t)/$$
# the true value used to simulate the data.
#
# $head Rate Table$$
# The $cref rate_table$$ only specifies that $icode iota$$ for the parent
# is the only nonzero rate for this example.
# In addition, it specifies the smoothing for that rate which has only
# one grid point. Hence there is only one model variable corresponding to
# the rates.
#
# $head Data Table$$
# For this example, all the data is
# $cref/Sincidence/avg_integrand/Integrand, I_i(a,t)/Sincidence/$$,
# with a Gaussian density, with mean $icode iota_true$$
# and standard deviation $codei%2*%iota_true%$$.
# The data is then censored, to be specific,
# values below zero are replaced by the value zero.
#
# $head Prior Table$$
# There is one prior in the $cref prior_table$$ for the only
# model variable.
#
# $head Discussion$$
# This fits two data sets.
# The first is simulated using python and put in the $cref data_table$$.
# The second is simulated using the $cref simulate_command$$
# and put in the $cref data_sim_table$$.
#
# $head Source Code$$
# $srcfile%
#	example/user/censor.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# begin problem parameters
iota_true        = 0.05
n_data           = 2000      # number of simulated data points
random_seed      = 0         # if zero, seed off the clock
# end problem parameters
# ------------------------------------------------------------------------
import time
if random_seed == 0 :
	random_seed = int( time.time() )
#
import sys
import os
import distutils.dir_util
import subprocess
import copy
import random
import math
test_program = 'example/user/censor.py'
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
# Note that the a, t values are not used for this example
def example_db (file_name) :
	def constant_weight_fun(a, t) :
		return 1.0
	def fun_iota_parent(a, t) :
		return ('prior_iota_parent', None, None)
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
	]
	#
	# weight table: The constant function 1.0 (one age and one time point)
	fun = constant_weight_fun
	weight_table = [
		{ 'name':'constant',  'age_id':[1], 'time_id':[1], 'fun':fun }
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
	# Values that are the same for all data rows
	# If you change cen_gaussian to gaussian, the test should fail
	# (because the simultion censors the data)
	meas_std = 2.0 * iota_true;
	row = {
		'node':        'world',
		'integrand':   'Sincidence',
		'density':     'cen_gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'age_lower':    0.0,
		'age_upper':    0.0,
		'time_lower':   1995.0,
		'time_upper':   1995.0,
		'meas_std':     meas_std,
	}
	random.seed(random_seed)
	for data_id in range( n_data ) :
		# simulate the data using a Gaussian with mean iota_true
		# and standard deviation meas_std
		meas_value        = random.gauss(iota_true, meas_std)
		# censor the data; i.e., replace negative values by zero values.
		meas_value        = max(meas_value, 0.0)
		row['meas_value'] = meas_value
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_iota_parent
			'name':     'prior_iota_parent',
			'density':  'uniform',
			'lower':    0.001,
			'upper':    1.00,
			'mean':     0.1,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	smooth_table = [
		{ # smooth_iota_parent
			'name':                     'smooth_iota_parent',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
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
		{ 'name':'random_seed',            'value':str(random_seed)    },
		{ 'name':'parent_node_name',       'value':'world'             },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },

		{ 'name':'quasi_fixed',            'value':'false'        },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-13'        },
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
	return
# ----------------------------------------------------------------------------
# Fit to estimate iota from censored data
# ----------------------------------------------------------------------------
#
file_name = 'example.db'
example_db(file_name)
#
# init and fit fixed
program = '../../devel/dismod_at'
system_command([ program, file_name, 'init' ])
system_command([ program, file_name, 'fit', 'fixed' ])
#
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
# check result of the fit fixed
assert len(var_table) == 1
first_estimate = fit_var_table[0]['fit_var_value']
if abs( 1.0 - first_estimate / iota_true ) > 1e-1 :
	print("random_seed = ", random_seed)
	print(1.0 - first_estimate / iota_true)
	assert False
# -----------------------------------------------------------------------------
# Simulate a censored data set and fit it
# ----------------------------------------------------------------------------
# set truth_var table to contain iota_true
tbl_name = 'truth_var'
col_name = [ 'truth_var_value' ]
col_type = [ 'real' ]
row_list = [ [iota_true] ]
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
#
# simulate a new data set and sample using it
system_command([ program, file_name, 'set', 'avgint', 'data' ])
system_command([ program, file_name, 'simulate', '1' ])
system_command([ program, file_name, 'fit', 'fixed', '0' ])
#
# check result of the second fit fixed
connection       = dismod_at.create_connection(file_name, new)
fit_var_table    = dismod_at.get_table_dict(connection, 'fit_var')
second_estimate  = fit_var_table[0]['fit_var_value']
if abs( 1.0 - second_estimate / iota_true ) > 1e-1 :
	print("random_seed = ", random_seed)
	print(1.0 - second_estimate / iota_true)
	assert False
#
# check that the estimates were different; i,e., used the second data set
assert abs( (first_estimate - second_estimate) / iota_true ) > 1e-2
#
# check all the simulated data values were non-negative
data_sim_table = dismod_at.get_table_dict(connection, 'data_sim')
for row in data_sim_table :
	assert row['data_sim_value'] >= 0.0
# -----------------------------------------------------------------------------
print('censor.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON