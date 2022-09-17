# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin user_sum_residual.py$$ $newlinech #$$
# $spell
#	Sincidence
# $$
#
# $section Sum of Residuals at Optimal Estimate$$
#
# $head Problem$$
# For this case the only data is we a sequence of  positive measurements of
# $cref/Sincidence/avg_integrand/Integrand, I_i(a,t)/Sincidence/$$
# which we denote by $latex y_i1$$ for $latex i = 0 , \ldots , N-1$$.
# We model $latex y_i$$ as independent and Gaussian with mean equal
# to the true value of iota $latex \iota$$,
# and standard deviation $latex \sigma_i$$.
# The negative log likelihood, up to a constant w.r.t $latex \iota$$, is
# $latex \[
#	f( \iota ) =
#	\frac{1}{2} \sum_{i=0}^{N-1} \left( \frac{y_i - \iota}{\sigma_i} \right)^2
# \] $$
#
# $head Optimal Solution$$
# The optimal estimator for $latex \iota$$ satisfies the equation
# $latex \[
#	0 = f'( \hat{\iota} ) =
#		- \sum_{i=1}^{N-1} \frac{y_i - \hat{\iota} }{\sigma_i^2}
# \] $$
#
# $head Weighted Residuals$$
# We use the notation $latex r_i$$ for the weighted residuals
# $latex \[
#	r_i = \frac{y_i - \hat{\iota}}{\sigma_i}
# \] $$
# If $latex \hat{\iota}$$ were the true value for $latex \iota$$,
# the weighted residuals would be mean zero and variance one.
# But $latex \hat{\iota}$$ is instead the maximum likelihood estimator and
# $latex \[
#	0 = \sum_{i=1}^{N-1} \frac{y_i - \hat{\iota} }{\sigma_i^2}
# \] $$
# $latex \[
#	0 = \sum_{i=1}^{N-1} \frac{r_i}{\sigma_i}
# \] $$
# Note that if $latex \sigma_i$$ were the same for all $latex i$$,
# the sum of the weighted residuals $latex \sum_i r_i$$ would be zero.
#
# $head CV Standard Deviations$$
# We consider the case were we a coefficient of variation $latex \lambda$$
# is used to model the measurement noise; $latex \sigma_i = \lambda y_i$$.
# In this case
# $latex \[
#	0 = \sum_{i=1}^{N-1} \frac{r_i}{y_i}
# \] $$
#
# $head Weighted Average of Weighted Residuals$$
# We define the weight $latex w_i$$ by
# $latex \[
#	w_i = \sigma_i^{-1} / \sum_{i=0}^{N-1} \sigma_i^{-1}
# \] $$
# The corresponding weighted average of the weighted residuals is zero; i.e,
# $latex \[
#	0 = \sum_{i=1}^{N-1} w_i r_i
# \] $$
#
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
#
# BEGIN PYTHON
# ------------------------------------------------------------------------
iota_true = 1e-4
# ------------------------------------------------------------------------
import sys
import os
import time
import numpy
import copy
import math
#
test_program = 'example/user/sum_residual.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
	usage  = 'python3 ' + test_program + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
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
	def fun_iota_parent(a, t) :
		return ('prior_iota_parent', 'prior_gauss_zero', 'prior_gauss_zero')
	# ----------------------------------------------------------------------
	# age_list
	age_list = [ 0.0, 100.0 ]
	#
	# time_list
	time_list = [ 1990.0, 2200.0 ]
	#
	# integrand table:
	integrand_table = [
		{ 'name':'Sincidence' },
	]
	#
	# node table:
	node_table = [ { 'name':'world', 'parent':'' } ]
	#
	# weight table:
	weight_table = list()
	#
	# covariate table:
	covariate_table = list()
	#
	# mulcov table:
	mulcov_table = list()
	# -----------------------------------------------------------------------
	# data table:
	data_table = list()
	#
	# values that are the same for all data rows
	row = {
		'integrand':   'Sincidence',
		'density':     'gaussian',
		'weight':      '',
		'hold_out':     False,
		'age_lower':    50.0,
		'age_upper':    50.0,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'node':         'world',
		'subgroup':     'world',
	}
	# N = 2, y_0 = iota_true - noise , y1 = iota_true + noise
	noise = 0.9 * iota_true
	Sincidence = [
		iota_true - noise,
		iota_true + noise,
	]
	for meas_value in Sincidence :
		row['meas_value'] = meas_value
		row['meas_std']   = meas_value   # 100 % coefficient of variation
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_gauss_zero
			'name':     'prior_gauss_zero',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      0.01,
		},{ # prior_iota_parent
			'name':     'prior_iota_parent',
			'density':  'uniform',
			'lower':    1e-19,
			'upper':    1.0,
			'mean':     1.01e-06,
		}
	]
	#
	# smooth table
	name      = 'smooth_iota_parent'
	fun       = fun_iota_parent
	age_grid  = [0]
	time_grid = [0]
	smooth_table = [ {
		'name':name, 'age_id':age_grid, 'time_id':time_grid, 'fun':fun
	} ]
	#
	# rate table:
	rate_table = [
		{	'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
			'child_smooth':   None,
		}
	]
	#
	# option_table
	option_table = [
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },
		{ 'name':'parent_node_name',       'value':'world'             },
		{ 'name':'quasi_fixed',            'value':'false'             },
		{ 'name':'print_level_fixed',      'value':'0'                 },
		{ 'name':'tolerance_fixed',        'value':'1e-9'              },
		{ 'name':'tolerance_random',       'value':'1e-10'             },

	]
	#
	# avgint table: empty
	avgint_table = list()
	#
	# nslist_table:
	nslist_table = dict()
	#
	# subgroup_table
	subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
	#
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
	return
# ===========================================================================
# create database
database  = 'example.db'
example_db(database)
#
# init
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, database, 'init' ])
#
# fit fixed
dismod_at.system_command_prc([ program, database, 'fit', 'fixed' ])
# -----------------------------------------------------------------------------
# read tables
#
new        = False
connection = dismod_at.create_connection(database, new)
#
name_list = [
	'fit_var',
	'data',
]
table_list = dict()
for table_name in name_list :
	table_list[table_name] = dismod_at.get_table_dict(connection, table_name)
#
connection.close()
# ----------------------------------------------------------------------------
#
# iota_hat
table = table_list['fit_var']
assert len(table) == 1
iota_hat = table[0]['fit_var_value']
#
# y, sigma
table = table_list['data']
assert len(table) == 2
y     = numpy.array( [ table[0]['meas_value'], table[1]['meas_value'] ] )
sigma = numpy.array( [ table[0]['meas_std'],   table[1]['meas_std']   ] )
#
# weighted residuals
r = (y - iota_hat) / sigma
#
# w
w = (1.0 / sigma) / numpy.sum( 1. / sigma)
#
# weighted average of weighted residuals
avg = numpy.sum( w * r )
#
assert abs(avg) < 1e-5
# -----------------------------------------------------------------------------
print('sum_residual.py: OK')
# END PYTHON
