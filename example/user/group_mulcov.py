# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin user_group_mulcov.py$$ $newlinech #$$
# $spell
#	init
#	covariates
#	covariate
#	integrands
#	Sincidence
#	mulcov
# $$
#
# $section Using Group Covariate Multipliers$$
#
# $head See Also$$
# $cref user_lasso_covariate.py$$.
#
# $head Purpose$$
# This example demonstrates using
# $cref/group covariate multipliers
#	/model_variables
#	/Fixed Effects, theta
#	/Group Covariate Multipliers
# /$$.
#
# $head True Value of Variables$$
# The values of the unknown variables that is used to
# simulate the data are
# $srcthisfile%
#	0%# BEGIN True values used to simulate data%# END True values used to simulate data%1
# %$$
#
# $head Integrand$$
# There are only two integrands in this example,
# $cref/Sincidence/avg_integrand/Integrand, I_i(a,t)/Sincidence/$$ and
# $cref/remission/avg_integrand/Integrand, I_i(a,t)/remission/$$.
#
# $head Node Tables$$
# The node table for this example is
# $pre
#                world
#               /     \
#  north_america       south_america
# $$
#
# $head Subgroup Table$$
# For this example there are two groups, north_america and south_america,
# and only one element in each group.
# Thus we use the same name for the subgroup as for the group.
#
# $head Covariates$$
# There are two covariates in this example, $icode income$$ and $icode sex$$.
# Both these covariates are scaled so their lowest value is -0.5 and highest
# value is +0.5.
#
# $head Covariate Multipliers$$
# There are two covariate multipliers in this example.
# The first multiples $icode income$$ and effects the Sincidence measurements,
# but only in north_america.
# The second multiples $icode sex$$ and effects the remission measurements,
# but only in south_america.
# Both are group covariate multipliers and hence fixed effects.
#
# $head Simulated Data$$
# The data is simulated using the true value for the variables,
# and the covariate effects mentioned above. No noise is added to the data,
# but it is modeled as having a ten percent coefficient of variation.
#
# $head Rate Variables$$
# The rate variables define the functions
# $cref/iota/avg_integrand/Rate Functions/iota_i(a,t)/$$ and
# $cref/rho/avg_integrand/Rate Functions/rho_i(a,t)/$$ using
# $cref/bilinear interpolation/bilinear/$$ of a rectangular grid.
# The grid's minimum age and time is (0, 1995). Its maximum age and time
# is (100, 2015). Thus there are four iota variables and four rho variables.
# The value prior for both these variables is uniform with lower (upper) limit
# 0.001 (1.0). The mean is the true value of iota divided by 3.
# This mean effects the starting and scaling
# points during the optimization, but not the statistics (for a uniform).
# The difference priors for the rate variables are gaussian
# with mean zero and standard deviation 0.01.
#
# $head Covariate Multipliers Variables$$
# The covariate multiplier functions for this example are constant.
# Hence there is one variable for each function.
# These are group covariate multipliers so there is only one
# function for each row of the mulcov_table (for which the group smoothing
# is not null); i.e., there are two group covariate multiplier variables.
# The first multiplies income and affects the measurement of
# Sincidence for north_america.
# The second multiplies sex and affects the measurement of
# remission for south_america.
# The prior for both these variables is uniform with lower (upper) limit
# -5.0 (+5.0). The mean is the true value of the incidence covariate
# multiplier divided by 3. This mean effects the starting and scaling
# points during the optimization, but not the statistics (for a uniform).
#
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# BEGIN True values used to simulate data
iota_true        = 0.05  # incidence rate
rho_true         = 0.10  # remission rate
mulcov_incidence = 1.0   # covariate multiplier for indicence data
mulcov_remission = 2.0;  # covariate multiplier for remission data
# END True values used to simulate data
# ------------------------------------------------------------------------
import sys
import os
import copy
import math
test_program = 'example/user/group_mulcov.py'
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
# Note that the a, t values are not used for this example
def example_db (file_name) :
	def fun_rate_parent(a, t) :
		return ('prior_value_parent', 'prior_diff_parent', 'prior_diff_parent')
	def fun_mulcov(a, t) :
		return ('prior_mulcov', None, None)
	# ----------------------------------------------------------------------
	# age table
	age_list    = [    0.0, 50.0,    100.0 ]
	#
	# time table
	time_list   = [ 1995.0, 2005.0, 2015.0 ]
	#
	# integrand table
	integrand_table = [
		{ 'name':'Sincidence' },
		{ 'name':'remission' }
	]
	#
	# node table: world -> (north_america, south_america)
	node_table = [
		{ 'name':'world',         'parent':'' },
		{ 'name':'north_america', 'parent':'world' },
		{ 'name':'south_america', 'parent':'world' },
	]
	#
	# subgroup_table
	subgroup_table = [
		{ 'subgroup':'north_america', 'group':'north_america' },
		{ 'subgroup':'south_america', 'group':'south_america' },
	]
	#
	# weight table:
	weight_table = list()
	#
	# covariate table:
	covariate_table = [
		{'name':'income', 'reference':0.5},
		{'name':'sex',    'reference':0.0, 'max_difference':0.6}
	]
	#
	# mulcov table
	# income has been scaled the same as sex so man use same smoothing
	mulcov_table = [
		{	# income effects north american incidence
			'covariate': 'income',
			'type':      'meas_value',
			'effected':  'Sincidence',
			'group':     'world',
			'group':     'north_america',
			'smooth':    'smooth_mulcov'
		},{ # sex effects south american remission
			'covariate': 'sex',
			'type':      'meas_value',
			'effected':  'remission',
			'group':     'world',
			'group':     'south_america',
			'smooth':    'smooth_mulcov'
		},{ # example covariate that is not being used
			'covariate': 'income',
			'type':      'rate_value',
			'effected':  'rho',
			'group':     'world',
			'group':     'north_america',
			'smooth':    None # not used because smoothing is null
		}
	]
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
		'node':        'world',
		'subgroup':    'world',
		'density':     'gaussian',
		'weight':      '',
		'hold_out':     False,
		'time_lower':   1995.0,
		'time_upper':   1995.0,
		'age_lower':    0.0,
		'age_upper':    0.0
	}
	# values that change between rows:
	income_reference = 0.5
	sex_reference    = 0.0
	n_data           = 4
	for data_id in range( n_data ) :
		if data_id % 4 == 0 or data_id % 4 == 1 :
			subgroup = 'north_america'
		else :
			subgroup = 'south_america'
		income      = data_id / float(n_data-1)
		sex         = ( data_id % 3 - 1.0 ) / 2.0
		integrand   = integrand_table[ data_id % 2 ]['name']
		if integrand == 'Sincidence' and subgroup == 'north_america' :
			effect      = (income - income_reference) * mulcov_incidence
			meas_value  = math.exp(effect) * iota_true
		elif integrand == 'remission' and subgroup == 'south_america' :
			effect      = (sex - sex_reference) * mulcov_remission
			meas_value  = math.exp(effect) * rho_true
		elif integrand == 'Sincidence' :
			meas_value = iota_true
		else :
			assert integrand == 'remission'
			meas_value = rho_true
		meas_std    = 0.1 * meas_value
		row['meas_value'] = meas_value
		row['meas_std']   = meas_std
		row['integrand']  = integrand
		row['income']     = income
		row['sex']        = sex
		row['subgroup']   = subgroup
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_diff_parent
			'name':     'prior_diff_parent',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      0.01,
		},{ # prior_value_parent
			'name':     'prior_value_parent',
			'density':  'uniform',
			'lower':    0.001,
			'upper':    1.00,
			'mean':     iota_true / 3.0,
		},{ # prior_mulcov
			'name':     'prior_mulcov',
			'density':  'uniform',
			'lower':    -5.0,
			'upper':     5.0,
			'mean':     mulcov_incidence / 3.0
		}
	]
	assert -5.0 < mulcov_incidence and mulcov_incidence < +5.0
	assert -5.0 < mulcov_remission and mulcov_remission < +5.0
	# ----------------------------------------------------------------------
	# smooth table
	last_age_id    = len(age_list) - 1
	last_time_id   = len(time_list) - 1
	smooth_table = [
		{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   [ 0, last_age_id ],
			'time_id':                  [ 0, last_time_id ],
			'fun':                       fun_rate_parent
		},{ # smooth_mulcov
			'name':                     'smooth_mulcov',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'fun':                       fun_mulcov
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'iota',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  None,
		},{
			'name':          'rho',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  None,
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'world'        },
		{ 'name':'ode_step_size',          'value':'10.0'         },
		{ 'name':'random_seed',            'value':'0'            },
		{ 'name':'rate_case',              'value':'iota_pos_rho_pos' },

		{ 'name':'quasi_fixed',            'value':'true'         },
		{ 'name':'derivative_test_fixed',  'value':'first-order'  },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-8'         },

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
file_name = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
#
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
#
# Results for fit fixed command
var_table     = dismod_at.get_table_dict(connection, 'var')
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
#
# parent node and tolerance
parent_node_id = 0
tol            = 5e-7
#
# check rate fixed effects
count             = 0
iota_rate_id      = 1
remission_rate_id = 2
for var_id in range( len(var_table) ) :
	row   = var_table[var_id]
	value = fit_var_table[var_id]['fit_var_value']
	rate_fixed_effect = row['node_id'] == parent_node_id
	rate_fixed_effect = rate_fixed_effect and row['var_type'] == 'rate'
	if rate_fixed_effect and row['rate_id'] == iota_rate_id :
		count += 1
		assert abs( value / iota_true - 1.0 ) < tol
	if rate_fixed_effect and row['rate_id'] == remission_rate_id :
		count += 1
		assert abs( value / rho_true - 1.0 ) < 5.0 * tol
assert count == 8
#
# check covariate multiplier values
count                   = 0
Sincidence_integrand_id = 0
remission_integrand_id  = 1
for var_id in range( len(var_table) ) :
	row   = var_table[var_id]
	value = fit_var_table[var_id]['fit_var_value']
	mulcov_var = row['var_type'] == 'mulcov_meas_value'
	if mulcov_var and row['integrand_id'] == Sincidence_integrand_id :
		count  += 1
		assert abs( value / mulcov_incidence - 1.0 ) < tol
	if mulcov_var and row['integrand_id'] == remission_integrand_id :
		count  += 1
		assert abs( value / mulcov_remission - 1.0 ) < tol
assert count == 2
# -----------------------------------------------------------------------------
print('group_mulcov.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
