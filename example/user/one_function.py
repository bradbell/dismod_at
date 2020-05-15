# --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_one_function.py$$ $newlinech #$$
# $spell
#	covariates
#	covariate
#	mulcov
# $$
#
# $section Fitting One Function of Two Variables$$
#
# $head Purpose$$
# This example shows how to fit one function of two variables given
# direct measurements of the function.
# For our example, we are give measurements of prevalence,
# as a function of age and time, and fit a function to the measurements.
#
# $head rho$$
# We use $icode rho$$
# as a surrogate for prevalence because our model for prevalence
# does not use $icode rho$$ (we could have used any other rate
# and its corresponding integrand).
#
#
# $head Random Effects$$
# To keep this example simple, there is only one node $code world$$
# in the $cref node_table$$, and the
# $cref/subgroup_smooth_id/mulcov_table/subgroup_smooth_id/$$
# is null in the mulcov_table.
# Hence, there are not random effects in this example.
#
# $head Covariates$$
# Income is the only covariate for this example and it has been normalized
# to be between zero and one.  The income reference value corresponds to
# no effect. The income multiplier is the true value used to simulate the
# data.
# $srccode%py%
income_reference   =   0.5
income_multiplier  = - 0.2
income_mulcov_type = "meas_value"
# %$$
# The $icode income_mulcov_type$$ can be either $code "meas_value"$$ or
# $code "rate_value"$$.
# It should not make a difference in the results which
# $cref/mulcov_type/mulcov_table/mulcov_type/$$ is used because
# there is only one rate (one function) being fit, and the
# $cref/ode/integrand_table/integrand_name/ODE/$$ is not needed to compute
# the integrand.
# You can test this by changing $icode income_mulcov_type$$ to
# $code "rate_value"$$
# and uses the same $cref/random_seed/user_one_function.py/random_seed/$$.
#
# $head Simulated Data$$
#
# $subhead Data Density$$
# A direct measurement of $icode rho$$
# (which we are using to represent prevalence) corresponds to the
# $cref/remission integrand/avg_integrand/Integrand, I_i(a,t)/remission/$$.
# We use a $cref/log_gaussian/density_table/density_name/log_gaussian/$$
# model with the following parameters:
# $srccode%py%
prevalence_sigma  = 0.1
prevalence_eta    = 1e-6
# %$$
#
# $subhead True Prevalence$$
# For simulating our prevalence data we consider the case where
# all the rates are constant and $icode rho$$, $icode chi$$ are zero; i.e.,
# the differential equation is
# $latex \[
#	\begin{array}{rcl}
#	S'(a) = -( \iota + \omega ) S(a)
#	\\
#	C'(a) = + \iota S(a) - \omega C(a)
#	\end{array}
# \] $$
# where $latex S(0) = 1$$ and $latex C(0) = 0$$.
# It follows that
# $latex \[
#	S(a) = \exp[ - ( \iota + \omega ) a ]
# \] $$
# $latex \[
#	C(a)
#	= \int_0^a \exp[ \omega (s - a) ] \iota S(s) ds
# \] $$
# You can check the formula for $latex C(a)$$ as follows:
# differentiating w.r.t. $latex a$$ inside the integral yields
# $latex - \omega C(a)$$ and differentiating the upper limit
# w.r.t. $latex a$$ yields $latex \iota S(a)$$.
# It follows that
# $latex \[
#	C(a) = \iota \int_0^a \exp[ \omega (s - a) - ( \iota + \omega) s ] ds
# \] $$
# $latex \[
#	C(a) = \iota \exp( - \omega a) \int_0^a \exp( - \iota s ) ds
# \] $$
# $latex \[
#	C(a) = \exp( - \omega a) - \exp[ - ( \iota + \omega) a ]
# \] $$
#
# $srccode%py%
def Prevalence(age) :
	# rho and chi are zero for this simulation of prevalence data
	iota      = 1e-4 # incidence used to simulate prevalence data
	omega     = 1e-2 # other cause mortality used to simulate prevalence data
	exp_omega = exp(-omega * age)
	exp_sum   = exp(-(iota + omega) * age)
	S         = exp_sum
	C         = exp_omega - exp_sum
	return C / (S + C)
# %$$
#
# $head Computing delta$$
# Once we have simulated a measurement value,
# we can solve for $latex \delta$$ are follows; see
# $cref/sigma/statistic/Log-Transformed Standard Deviation, sigma/$$:
# $latex \[
#	\sigma = \log( y + \eta + \delta ) - \log(y + \eta )
# \] $$
# $latex \[
#	\exp ( \sigma ) = \frac{ y + \eta + \delta }{ y + \eta }
# \] $$
# $latex \[
#	\exp ( \sigma ) ( y + \eta ) = y + \eta + \delta
# \] $$
# $latex \[
#	\delta = [ \exp ( \sigma ) - 1 ] ( y + \eta )
# \] $$
#
# $head Prevalence Prior$$
# Prevalence must always be between zero and one so this limits are used
# in the prior for prevalence.
# Some functions might allow for negative values and the lower limit
# for the rate would be negative in that case.
#
# $head random_seed$$
# We use the clock to choose a seed for the random number generator.
# If an error occurs, the seed will be printed so that the error can be
# reproduced. You can also use a fixed value for the random seed to see
# how changing other parameters changes the results.
# $srccode%py%
import time
random_seed = int( time.time() )
# %$$
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
#
# $end
# ------------------------------------------------------------------------
# BEGIN PYTHON
import random
from math import exp
import sys
import os
import distutils.dir_util
import copy
test_program = 'example/user/one_function.py'
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
random.seed(random_seed)
# ------------------------------------------------------------------------
def example_db (file_name) :
	def fun_prevalence(a, t) :
		return ('prior_prevalence', 'prior_dage', 'prior_dtime')
	def fun_mulcov(a, t):
		return('prior_mulcov', None, None)
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ 0.0, 5.0, 10.0, 20.0, 40.0, 60.0, 80.0, 100.0 ]
	#
	# time table:
	time_list   = [ 1990.0, 2200.0 ]
	#
	# integrand table:
	integrand_table = [ { 'name':'remission' } ]
	#
	# node table:
	node_table = [ { 'name':'world', 'parent':'' } ]
	#
	# weight table:
	weight_table = list()
	#
	# covariate table:
	covariate_table = [ {'name':'income', 'reference':income_reference} ]
	#
	# mulcov table:
	if income_mulcov_type == 'rate_value' :
		effected = 'rho'
	else :
		assert income_mulcov_type == 'meas_value'
		effected = 'remission'
	mulcov_table = [
		{
			'covariate': 'income',
			'type':      income_mulcov_type,
			'effected':  effected,
			'group':     'world',
			'smooth':    'smooth_mulcov'
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
	#
	# values that are the same for all data rows
	row = {
		'node':        'world',
		'subgroup':    'world',
		'integrand':   'remission',
		'density':     'log_gaussian',
		'weight':      '',
		'hold_out':     False,
		'time_lower':   2000.,
		'time_upper':   2000.,
		'eta':          prevalence_eta
	}
	n_data   = 2000
	n_income = 30
	for data_id in range(n_data) :
		age           = 100.0 * data_id / float(n_data - 1)
		income        = (data_id % n_income)  / float(n_income - 1)
		income_effect = (income - income_reference) * income_multiplier
		eta           = prevalence_eta
		sigma         = prevalence_sigma
		log_noise     = random.gauss(0, sigma)
		y             = exp(log_noise) * Prevalence(age) * exp(income_effect)
		delta         = ( exp(sigma) - 1 ) * ( y + eta )
		row['age_lower']  = age
		row['age_upper']  = age
		row['income']     = income
		row['meas_value'] = y
		row['meas_std']   = delta
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_prevalence
			'name':     'prior_prevalence',
			'density':  'uniform',
			'lower':    0.0,
			'upper':    1.0,
			'mean':     0.01,
		},{ # prior_dage
			'name':     'prior_dage',
			'density':  'log_gaussian',
			'mean':     0.0,
			'std':      2.0,
			'eta':      prevalence_eta,
		},{ # prior_dtime
			'name':     'prior_dtime',
			'density':  'log_gaussian',
			'mean':     0.0,
			'std':      0.1,
			'eta':      prevalence_eta,
		}, { # prior_mulcov
			'name':     'prior_mulcov',
			'density':  'uniform',
			'lower':    -2.0,
			'upper':    +2.0,
			'mean':     0.0,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	smooth_table = list()
	#
	name    = 'smooth_prevalence'
	fun     = fun_prevalence
	age_id  = range( len(age_list) )
	time_id = range( len(time_list) )
	smooth_table.append(
		{'name':name, 'age_id':age_id, 'time_id':time_id, 'fun':fun }
	)
	name = 'smooth_mulcov'
	fun  = fun_mulcov
	smooth_table.append(
		{'name':name, 'age_id':[0], 'time_id':[0], 'fun':fun }
	)
	# ----------------------------------------------------------------------
	# rate table:
	rate_table = [
		{	'name':          'rho',
			'parent_smooth': 'smooth_prevalence',
			'child_smooth':  None,
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'rate_case',              'value':'no_ode'       },
		{ 'name':'parent_node_name',       'value':'world'        },
		{ 'name':'ode_step_size',          'value':'5.0'          },
		{ 'name':'random_seed',            'value':random_seed    },

		{ 'name':'quasi_fixed',            'value':'false'        },
		{ 'name':'derivative_test_fixed',  'value':'first-order'  },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-8'         },

		{ 'name':'derivative_test_random', 'value':'second-order' },
		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-8'         }
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
	return
# ===========================================================================
# create database
file_name = 'example.db'
example_db(file_name)
#
# fun a fit
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
#
# read database
new           = False
connection    = dismod_at.create_connection(file_name, new)
var_table     = dismod_at.get_table_dict(connection, 'var')
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
age_table     = dismod_at.get_table_dict(connection, 'age')
# -----------------------------------------------------------------------
# check fit results
#
for var_id in range( len(var_table) ) :
	fit_var_value = fit_var_table[var_id]['fit_var_value']
	var_type      = var_table[var_id]['var_type']
	true_value    = None
	if var_type == 'mulcov_' + income_mulcov_type :
		true_value = income_multiplier
	else :
		assert var_type == 'rate'
		age_id     = var_table[var_id]['age_id']
		age        = age_table[age_id]['age']
		true_value = Prevalence(age)
	if true_value > 10.0 * prevalence_eta  :
		rel_error = fit_var_value / true_value - 1.0
	else :
		rel_error = fit_var_value - true_value
	if abs(rel_error) > 0.05 :
		print('fit_var_value = ', var_fit_value)
		print('true_value    = ', true_value)
		print('random_seed   = ', random_seed)
	assert abs(rel_error) < 0.05
# -----------------------------------------------------------------------------
connection.close()
print('one_function.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
