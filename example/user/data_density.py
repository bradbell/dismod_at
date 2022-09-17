# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin user_data_density.py$$ $newlinech #$$
# $spell
#	init
#	Sincidence
#	cv
# $$
#
# $section Fit With Outliers Using Data Density Command$$
#
# $head Purpose$$
# This example first fits using a Gaussian data density.
# It uses the first fit as a starting point for another fit
# with a Student's-t data density.
# The $cref data_density_command$$ is used to change the data density.
#
# $head Problem Parameters$$
# The following values are used to simulate and model the data:
# $srcthisfile%
#	0%# begin problem parameters%# end problem parameters%1
# %$$
#
# $head Age and Time Values$$
# The age and time values do not matter for this problem
# because all the functions are constant in age and time.
# This can be seen by the fact that all of the smoothing has one age
# and one time point.
#
# $head Variables$$
# The constant value used to model
# $cref/iota/avg_integrand/Rate Functions/iota_i(a,t)/$$
# for the parent node is only one model variable in this example.
#
# $head Data Table$$
# For this example, all the data is
# $cref/Sincidence/avg_integrand/Integrand, I_i(a,t)/Sincidence/$$.
# The good data is Gaussian with mean $icode iota_true$$
# and standard deviation $icode%meas_cv%*%iota_true%$$.
# The outlier data has mean $codei%10*%iota_true%$$
# and standard deviation $codei%2*%iota_true%$$.
#
# $head Rate Table$$
# The $cref rate_table$$ only specifies that $icode iota$$ for the parent
# is the only nonzero rate for this example.
# In addition, it specifies the smoothing for that rate which has only
# one grid point. Hence there is only one model variable corresponding to
# the rates.
#
# $head Prior Table$$
# The prior for $icode iota$$ is uniform with lower limit 1e-4,
# upper limit 1.0 and mean 0.1.
# Note that the mean is not really the mean of this uniform distribution
# and it is only used to get the initial starting and scaling point
# for the optimization; see $cref init_command$$.
#
# $head Fitting$$
# A first fit is done using a Gaussian density for the data.
# This is used to get a better starting point for the optimization.
# All the density values in the data table are changed to be Students-t
# and a second fit is done.
# The results of the second fit are check for accuracy of the estimate
# and for proper detection of the outliers.
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
# begin problem parameters
iota_true    = 1e-2 # true value of iota used for simulating data
meas_cv      = 0.1  # coefficient of variation for good data points
n_data       = 50   # total number of data points
n_outlier    = 5    # number of data points that are outliers
nu           = 5.0  # degrees of freedom in data students-t density
cutoff       = 3.5  # students-t weighted residual cutoff for outlier
random_seed  = 0    # if zero, seed off the clock
# end problem parameters
# ------------------------------------------------------------------------
import time
if random_seed == 0 :
	random_seed = int( time.time() )
#
import sys
import os
import copy
import math
import random
test_program = 'example/user/data_density.py'
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
if not os.path.exists('build/example/user') :
    os.makedirs('build/example/user')
os.chdir('build/example/user')
# ------------------------------------------------------------------------
def example_db (file_name) :
	def fun_rate_parent(a, t) :
		return ('prior_iota_parent', None, None)
	import dismod_at
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
	# node table: north_america -> (united_states, canada)
	node_table = [
		{ 'name':'north_america', 'parent':'' },
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
	# avgint table: same order as list of integrands
	avgint_table = list()
	#
	# nslist_table:
	nslist_table = dict()
	# ----------------------------------------------------------------------
	# data table: same order as list of integrands
	data_table = list()
	meas_std = meas_cv * iota_true
	row = {
		'density':     'gaussian',
		'weight':      '',
		'hold_out':     False,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    50.0,
		'age_upper':    50.0,
		'integrand':    'Sincidence',
		'meas_std':     meas_std,
		'nu':           nu
	}
	random.seed(random_seed)
	for data_id in range( n_data ):
		row['node'] = 'north_america'
		meas_value  = random.gauss(iota_true, meas_std)
		#
		if data_id < n_outlier :
			meas_value = random.gauss( 10.0 * iota_true, 2.0 * iota_true )
		row['meas_value']  = meas_value
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_iota_parent
			'name':     'prior_iota_parent',
			'density':  'uniform',
			'lower':    1e-4,
			'upper':    1.0,
			'mean':     0.1,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	smooth_table = [
		{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   [ 0 ],
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
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'north_america'     },
		{ 'name':'ode_step_size',          'value':'10.0'              },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },

		{ 'name':'quasi_fixed',            'value':'false'             },
		{ 'name':'max_num_iter_fixed',     'value':'100'               },
		{ 'name':'print_level_fixed',      'value':'0'                 },
		{ 'name':'tolerance_fixed',        'value':'1e-11'             },
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
	# ----------------------------------------------------------------------
# ===========================================================================
# Create database and run init, start, fit with just fixed effects
file_name = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
#
# set start_var table equal to fit_var table
dismod_at.system_command_prc(
	[ program, file_name, 'set', 'start_var', 'fit_var' ]
)
#
# change the density from gaussian to students
integrand_name = 'Sincidence'
density_name   = 'students'
eta_str        = 'null'
nu_str         = str(nu)
dismod_at.system_command_prc([
	program, file_name, 'data_density',
	integrand_name, density_name, eta_str, nu_str
])
#
# fit with Students-t (now that we have a better starting point)
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
#
# get second fit information
new             = False
connection      = dismod_at.create_connection(file_name, new)
node_table      = dismod_at.get_table_dict(connection, 'node')
rate_table      = dismod_at.get_table_dict(connection, 'rate')
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
fit_data_subset = dismod_at.get_table_dict(connection, 'fit_data_subset')
data_table      = dismod_at.get_table_dict(connection, 'data')
#
n_var = len( fit_var_table )
assert n_var == 1
for var_id in range( n_var ) :
	var_type = var_table[var_id]['var_type']
	rate_id = var_table[var_id]['rate_id']
	node_id = var_table[var_id]['node_id']
	#
	assert( var_type == 'rate' )
	assert( rate_table[rate_id]['rate_name'] == 'iota' )
	assert node_table[node_id]['node_name'] == 'north_america'
	#
	value   = fit_var_table[var_id]['fit_var_value']
	err = value / iota_true - 1.0
	if abs(err) > 3.0 * meas_cv / math.sqrt(n_data - n_outlier):
		print('random_seed = ', random_seed)
		assert False
#
# check that bad data (and only bad data) has large residuals
ok = True
for data_id in range(n_data) :
	residual = fit_data_subset[data_id]['weighted_residual']
	if abs(residual) > cutoff :
		if data_id > n_outlier :
			print('large residual at good data point: data_id = ', data_id)
			ok = False
	if abs(residual) < cutoff :
		if data_id < n_outlier :
			print('small residual at bad data point: data_id = ', data_id)
			ok = False
if not ok :
	print('random_seed = ', random_seed)
assert ok
# -----------------------------------------------------------------------
print('data_density.py: OK')
# END PYTHON
