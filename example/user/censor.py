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
# $$
#
# $section Fitting Data That Has Negative Values Censored$$
#
# $head Under Construction$$
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
# with a Gaussian density, with mean value
# $codei%
#	%iota_reference% * exp( %alpha_income% * x_income )
# %$$
# and with standard deviation equal 10% of the mean.
# The data is then censored, to be specific,
# values below zero are replaced by the value zero.
#
# $head Prior Table$$
# There is one prior in the $cref prior_table$$ for the only
# model variable.
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
n_data           = 1000      # number of simulated data points
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
	# values that are the same for all data rows
	meas_std = iota_true;
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
	n_smooth  = len( smooth_table )
	rate_true = []
	for rate_id in range( len( data_table ) ) :
		# for this particular example
		data_id    = rate_id
		meas_value = data_table[data_id]['meas_value']
		rate_true.append(meas_value)
	#
	return
# ===========================================================================
# Fit to estimate iota
file_name      = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
for command in [ 'init', 'fit' ] :
	cmd = [ program, file_name, command ]
	if command == 'fit' :
		variables = 'fixed'
		cmd.append(variables)
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
#
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
assert len(var_table) == 1
estimate   = fit_var_table[0]['fit_var_value']
# check result of the fit
print(iota_true, estimate, 1.0 - estimate / iota_true)
print('censor.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
