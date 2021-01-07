# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_residual.py$$ $newlinech #$$
# $spell
#	mtother
#	std
#	cv
#	covariates
#	var
#	mulstd
#	dage
# $$
#
# $section Weighted Residuals$$
#
# $head Example Parameters$$
# The following values are used to simulate the data and define the priors:
# $srccode%py%
omega_true      = [ 0.01, 0.02 ]
omega_mean      = 0.15
minimum_meas_cv = 0.10
# %$$
#
# $head Model$$
# The only non-zero for this example is
# other cause mortality, $latex \omega$$, for the parent area.
# The parent omega grid has two points,
# one at age zero and the other at age 100.
# The corresponding true values for $latex \omega$$ are
# $code omega_true[0]$$ at age zero and $code omega_true[1]$$ at age 100.
#
# $head Data$$
# The integrand for this data is
# $cref/mtother/avg_integrand/Integrand, I_i(a,t)/mtother/$$; i.e.,
# a direct measurement of $latex \omega$$.
# Both ages are included in the data for this example.
# Both the Gaussian and Log-Gaussian $cref/densities/density_table/$$
# are included.
# In addition, both the case where the
# $cref/meas_std/data_table/meas_std/$$ is above and below the bound specified
# by $cref/minimum_meas_cv/integrand_table/minimum_meas_cv/$$ are included.
#
# $subhead delta$$
# We use the notation
# $cref/Delta/data_like/Notation/Minimum CV Standard Deviation, Delta_i/$$ for
# the standard deviation adjusted by the minimum measurement cv.
# The adjusted standard deviation
# $cref/delta/data_like/Adjusted Standard Deviation, delta_i/$$
# is equal to $latex \Delta$$ because there are no measurement
# standard deviation covariates multipliers for this example.
#
# $subhead Gaussian Residuals$$
# In the Gaussian case, the residual is
# $latex \[
#	(y - \mu) / \delta
# \]$$,
# where $latex y$$ is the measured value and $latex \mu$$ is the
# model value for the
# $cref/average integrand/avg_integrand/Average Integrand, A_i/$$.
#
# $subhead Log-Gaussian Residuals$$
# In the Log-Gaussian case, we the
# $cref/
#   log-transformed standard deviation/
#   statistic/
#   Log-Transformed Standard Deviation, sigma
# /$$
# $latex \[
#	\sigma = \log ( y + \eta + \delta ) - \log( y + \eta )
# \] $$
# where $latex y$$ is the measured value
# and $latex \eta$$ is the offset in the log transform.
# The residual is
# $latex \[
#	\frac{ \log ( y + \eta ) - \log ( \mu + \eta ) } { \sigma }
# \] $$
# where $latex \mu$$ is the model value for the average integrand.
#
#
# $head Priors$$
#
# $subhead Value Residual$$
# There are two value residuals, one for $latex \omega$$ at age zero
# and the other at age 100.
# The density used for the value residuals is Log-Gaussian.
# The mean value used in the prior for the value residuals $latex \mu$$ is
# $code omega_mean$$.
# The standard deviation used for the value residuals in
# $code omega_mean * 0.1$$
# The log transformed standard deviation is
# $latex \[
#	\sigma = \log ( \mu + \eta + \delta ) - \log( \mu + \eta )
# \] $$
# The residual is
# $latex \[
#	\frac{ \log ( y + \eta ) - \log ( \mu + \eta ) } { \sigma }
# \] $$
# where $latex y$$ is the $cref/fit_var_value/fit_var_table/fit_var_value/$$
# for the model variable.
#
# $subhead Difference Residual$$
# There is one difference residuals for the difference of
# $latex \omega$$ at age zero and age 100.
# The density used for the value residuals is Log-Gaussian.
# The mean value used in the prior for the difference residual
# $latex \mu = 0$$.
# The standard deviation used for the difference residual is $code 0.1$$.
# (This corresponds to a coefficient of variation of $latex e^{0.1} - 1$$.
# which is approximately equal to $code 0.1$$; i.e., 10 percent.)
# The age difference smoothing multiplier prior id
# $cref/mulstd_dage_prior_id/smooth_table/$$ for this example is null,
# so $latex \delta$$ is equal to the standard deviation $code 0.1$$.
# The residual is
# $latex \[
#	\frac{ \log ( z + \eta ) - \log ( y + \eta ) - \mu } { \delta }
# \] $$
# where $latex y$$ ($latex z$$)
# is the $cref/fit_var_value/fit_var_table/fit_var_value/$$
# at age zero (age 100).
#
# $end
# ---------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import math
import csv
import copy
import numpy
#
test_program = 'example/user/residual.py'
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
#
def fun_omega_parent(a, t):
	return ('prior_omega_value', 'prior_omega_dage', None)
def fun_omega_eta() :
	return omega_mean * 1e-4
def fun_omega_value_std() :
	return omega_mean * 0.1
def fun_omega_dage_std() :
	return 0.1
# ------------------------------------------------------------------------
def example_db ():
	# ----------------------------------------------------------------------
	# age list
	age_list    = [ 0.0, 100.0 ]
	#
	# time list
	time_list   = [ 1995.0, 2015.0 ]
	#
	# only one integrand in this example
	integrand_table = [
		 { 'name':'mtother', 'minimum_meas_cv':minimum_meas_cv }
	]
	#
	# just the world (which has no parent)
	node_table = [ { 'name':'world', 'parent':'' } ]
	#
	# ---------------------------------------------------------------------
	weight_table    = list()
	covariate_table = list()
	avgint_table    = list()
	mulcov_table    = list()
	nslist_table    = dict()
	# ---------------------------------------------------------------------
	# data table
	row = {
		'integrand':   'mtother',
		'node':        'world',
		'subgroup':    'world',
		'weight':      '',
		'time_lower':  2000.0,
		'time_upper':  2000.0,
		'hold_out':    False,
		'eta':         fun_omega_eta(),
		'nu':          4.0
	}
	data_table = list()
	for age_id in range( len(age_list) ) :
		age               = age_list[age_id]
		row['age_lower']  = age
		row['age_upper']  = age
		meas_value        = omega_true[age_id]
		row['meas_value'] = meas_value
		for density in [ 'gaussian', 'log_gaussian' ] :
			row['density']  = density
			#
			meas_std        = meas_value * minimum_meas_cv / 2.0
			row['meas_std'] = meas_std
			data_table.append( copy.copy(row) )
			#
			meas_std        = meas_value * minimum_meas_cv * 2.0
			row['meas_std'] = meas_std
			data_table.append( copy.copy(row) )
	# ---------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_omega_value
			'name':     'prior_omega_value',
			'density':  'log_gaussian',
			'lower':    0.0,
			'upper':    1.0,
			'mean':     omega_mean,
			'std':      fun_omega_value_std(),
			'eta':      fun_omega_eta()
		},{
		  # prior_omega_dage
			'name':     'prior_omega_dage',
			'density':  'log_gaussian',
			'mean':     0.0,
			'std':      fun_omega_dage_std(),
			'eta':      fun_omega_eta()
		}
	]
	# ---------------------------------------------------------------------
	# smooth table:
	smooth_table = [
		{	# smooth_omega_parent
			'name':                     'smooth_omega_parent',
			'age_id':                   [0, 1],
			'time_id':                  [0],
			'fun':                      fun_omega_parent
		}
	]
	# ---------------------------------------------------------------------
	# rate table
	rate_table = [
		{	'name':          'omega',
			'parent_smooth': 'smooth_omega_parent',
		}
	]
	# -------------------------------------------------------------------
	# option_table
	# maximum number of iterations is -1 so compute residuals at start value
	option_table = [
		{'name':'parent_node_name',     'value':'world'             },
		{'name':'rate_case',            'value':'iota_zero_rho_zero'}
	]
	# -------------------------------------------------------------------
	# subgroup_table
	subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
	# -------------------------------------------------------------------
	# create database
	file_name = 'example.db'
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
# ---------------------------------------------------------------------------
example_db()
file_name = 'example.db'
program   = '../../devel/dismod_at'
#
dismod_at.system_command_prc([ program, file_name, 'init'] )
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed'] )
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
#
# get variable and fit_var tables
data_table            = dismod_at.get_table_dict(connection, 'data')
data_subset_table     = dismod_at.get_table_dict(connection, 'data_subset')
fit_data_subset_table = dismod_at.get_table_dict(connection, 'fit_data_subset')
density_table         = dismod_at.get_table_dict(connection, 'density')
var_table             = dismod_at.get_table_dict(connection, 'var')
fit_var_table         = dismod_at.get_table_dict(connection, 'fit_var')
smooth_gird_table     = dismod_at.get_table_dict(connection, 'smooth_grid')
#
# check data residuals
eps99    = 99.0 * numpy.finfo(float).eps
n_subset = len(data_subset_table)
assert n_subset == 8
for data_subset_id in range(n_subset) :
	data_id        = data_subset_table[data_subset_id]['data_id']
	data_row       = data_table[data_id]
	fit_row        = fit_data_subset_table[data_subset_id]
	avg_integrand  = fit_row['avg_integrand']
	residual       = fit_row['weighted_residual']
	meas_value     = data_row['meas_value']
	meas_std       = data_row['meas_std']
	density_id     = data_row['density_id']
	eta            = data_row['eta']
	density        = density_table[density_id]['density_name']
	Delta          = max( minimum_meas_cv * meas_value, meas_std)
	delta          = Delta
	check          = None
	if density == 'gaussian' :
		check = (meas_value - avg_integrand) / delta
	if density == 'log_gaussian' :
		log_y_eta_plus  = math.log(meas_value + eta + delta)
		log_y_eta       = math.log(meas_value + eta)
		log_mu_eta      = math.log(avg_integrand + eta)
		sigma           = log_y_eta_plus - log_y_eta
		check           = (log_y_eta - log_mu_eta) / sigma
	relerr = residual / check - 1.0
	assert abs(relerr) <= eps99
#
# check variable value residuals
n_var = len(var_table)
assert( n_var == 2)
omega_id         = 4
age_id2var_value = n_var * [0.0]
for var_id in range(n_var) :
	var_row = var_table[var_id]
	assert( var_row['var_type'] == 'rate' )
	assert( var_row['rate_id'] == omega_id )
	fit_row   = fit_var_table[var_id]
	value     = fit_row['fit_var_value']
	residual  = fit_row['residual_value']
	mu        = omega_mean
	std       = fun_omega_value_std()
	sigma     = math.log(mu + eta + std) - math.log(mu + eta)
	check     = (math.log(value + eta) - math.log(mu + eta)) / sigma
	relerr    = residual / check - 1.0
	assert abs(relerr) <= eps99
	#
	# mapping from age_id to fit_var_value
	age_id = var_row['age_id']
	age_id2var_value[age_id] = value
	#
	# residual_dage
	if age_id == 0 :
		residual_dage = fit_row['residual_dage']
#
# check dage residual
z        = age_id2var_value[1]
y        = age_id2var_value[0]
mu       = 0.0
delta    = fun_omega_dage_std()
check    = (math.log(z + eta) - math.log(y + eta) - mu) / delta
relerr   = residual_dage / check - 1.0
assert( abs(relerr) <= eps99 )

# -----------------------------------------------------------------------
print('residual.py: OK')
