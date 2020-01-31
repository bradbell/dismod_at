#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_lasso_covariate.py$$ $newlinech #$$
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
# $section Using Lasso on Covariate Multiplier$$
#
# $head See Also$$
# $cref user_group_mulcov.py$$
#
# $head Purpose$$
# This example uses a $cref/laplace/density_table/density_name/laplace/$$
# prior distribution on two covariate multipliers
# to detect which covariate is present in the data.
# It then refits the data with a uniform on the multiplier that is present
# and the other multiplier constrained to be zero.
#
# $head Problem Parameters$$
# The following values are used to simulate the data and set the priors
# for fitting the data:
# $srcthisfile%
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
# The are three model variables in this example:
# $table
# $icode iota_reference$$
#	$cnext $cref/iota/avg_integrand/Rate Functions/iota_i(a,t)/$$
#	corresponding to reference value of covariates
# $rnext
# $icode alpha_income$$ $cnext $cref/covariate multiplier/mulcov_table/$$
#	corresponding to income.
# $rnext
# $icode alpha_sex$$ $cnext
#	covariate multiplier corresponding to sex.
# $tend
#
# $head Covariate Table$$
# There are two $cref/covariates/covariate_table/$$,
# $code income$$ and $code sex$$, in this example.
# Income is a linear function of $cref/data_id/data_table/data_id/$$.
# It starts with 0.0 and ends with 1.0.
# To be specific,
# $codei%
#	%income% = %data_id% / (%n_data% - 1)
# %$$
# Sex cycles between the values -0.5, 0.0, +0.5
# as a function of $icode data_id$$.
# Note that income and sex have range 1.0.
# This makes the scaling of the covariate multipliers simpler.
# The reference value for each covariate has been set to the midpoint
# of its range.
# We use $icode x_income$$ ($icode x_sex$$) to denote the difference
# of income (sex) minus its reference value.
#
# $head Mulcov Table$$
# The $cref/covariate multiplier table/mulcov_table/$$ has two rows,
# one for each covariate multiplier.
# Both multipliers affects the value of
# $cref/iota/avg_integrand/Rate Functions/iota_i(a,t)/$$,
# but one multiplies income and the other sex.
# We use $icode alpha_income$$ ($icode alpha_sex$$) to denote the
# covariate multiplier corresponding to income (sex).
# The model for the value of $icode iota$$ is
# $codei%
#	%iota_reference% * exp( %alpha_income%*%x_income% + %alpha_sex%*%x_sex% )
# %$$
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
#
# $head Prior Table$$
# There are three priors in the $cref prior_table$$, one for each
# model variable.
#
# $subhead iota$$
# The prior for fitting the reference value of $icode iota$$
# is uniform with lower limit 0.001, and upper limit 1.
# Its mean value 0.1 is only used as a starting and scaling point for the
# fitting process; see $cref start_var_table$$ and $cref scale_var_table$$.
#
# $subhead alpha$$
# There is a separate prior for $icode alpha_income$$ and $icode alpha_sex$$.
# For the first fit, they are both a Laplace density with
# mean zero and standard deviation $icode laplace_std$$.
# The first fit is used to decide that $icode alpha_income$$ is nonzero
# and $icode alpha_sex$$ is zero.
# The prior for $icode alpha_income$$ is changed to a uniform
# and the prior for $icode alpha_sex$$ is change to have upper and lower
# limit zero.
# A second fit is done to recover the value of $icode alpha_income$$
# without shrinkage due to the Laplace prior.
#
# $head Source Code$$
# $srcfile%
#	example/user/lasso_covariate.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# begin problem parameters
alpha_income     = 1.0       # income covariate multiplier during simulation
iota_reference   = 0.05      # iota corresponding to reference for covariates
n_data           = 301       # number of simulated data points
laplace_std      = 0.1/17.34 # 0.1 / sqrt(n_data)
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
import copy
import random
import math
test_program = 'example/user/lasso_covariate.py'
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
	def fun_iota_parent(a, t) :
		return ('prior_iota_parent', None, None)
	def fun_income(a, t) :
		return ('prior_income', None, None)
	def fun_sex(a, t) :
		return ('prior_sex', None, None)
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
	# weight table:
	weight_table = list()
	#
	# covariate table:
	covariate_table = [
		{'name':'income', 'reference':0.5},
		{'name':'sex',    'reference':0.0}
	]
	#
	# mulcov table
	# income has been scaled the same as sex so man use same smoothing
	mulcov_table = [
		{
			'covariate': 'income',
			'type':      'rate_value',
			'effected':  'iota',
			'group':     'world',
			'smooth':    'smooth_income'
		},{
			'covariate': 'sex',
			'type':      'rate_value',
			'effected':  'iota',
			'group':     'world',
			'smooth':    'smooth_sex'
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
		'integrand':   'Sincidence',
		'density':     'gaussian',
		'weight':      '',
		'hold_out':     False,
		'age_lower':    0.0,
		'age_upper':    0.0,
		'time_lower':   1995.0,
		'time_upper':   1995.0,
	}
	# values that change between rows:
	income_reference = 0.5
	random.seed(random_seed)
	for data_id in range( n_data ) :
		income         = data_id / float(n_data-1)
		sex            = ( data_id % 3 - 1.0 ) / 2.0
		x_income       = income - income_reference
		avg_integrand  = iota_reference * math.exp( alpha_income * x_income )
		meas_value     = random.gauss(avg_integrand, 1e-1 * avg_integrand)
		meas_std       = 0.1 * meas_value
		row['meas_value'] = meas_value
		row['meas_std']   = meas_std
		row['income']     = income
		row['sex']        = sex
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
		},{ # prior_income
			'name':     'prior_income',
			'density':  'laplace',
			'mean':     0.0,
			'std':      laplace_std,
		},{ # prior_sex
			'name':     'prior_sex',
			'density':  'laplace',
			'mean':     0.0,
			'std':      laplace_std,
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
		},{ # smooth_income
			'name':                     'smooth_income',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'fun':                      fun_income
		},{ # smooth_sex
			'name':                     'smooth_sex',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'fun':                      fun_sex
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
# Fit to determine nonzero covariate multipliers
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
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
density_table   = dismod_at.get_table_dict(connection, 'density')
#
# density_id for a uniform distribution
uniform_id = None
for density_id in range( len(density_table) ) :
	if density_table[density_id]['density_name'] == 'uniform' :
		uniform_id = density_id
#
# check covariate multiplier values
nonzero_mulcov  = list()
for var_id in range( len(var_table) ) :
	row   = var_table[var_id]
	match = row['var_type'] == 'mulcov_rate_value'
	if match :
		fit_var_value  = fit_var_table[var_id]['fit_var_value']
		covariate_id   = row['covariate_id']
		#
		nonzero = abs(fit_var_value) > laplace_std
		if covariate_id == 0 :
			if( not nonzero ) :
				print('random_seed = ', random_seed)
			assert nonzero
		else :
			if( nonzero ) :
				print('random_seed = ', random_seed)
			assert not nonzero
		nonzero_mulcov.append( nonzero )
assert len(nonzero_mulcov) == 2
# -----------------------------------------------------------------------------
# Remove laplace prior on nonzero multipliers and re-fit
prior_name = [ 'prior_income', 'prior_sex' ]
for covariate_id in range(2):
	if nonzero_mulcov[covariate_id] :
		command = 'UPDATE prior SET density_id = '+ str(uniform_id)
		command += ' WHERE prior_name == '
		command += '"' + prior_name[covariate_id] + '"'
		dismod_at.sql_command(connection, command)
	else :
		command = 'UPDATE prior SET lower=0.0, upper=0.0 WHERE prior_name == '
		command += '"' + prior_name[covariate_id] + '"'
		dismod_at.sql_command(connection, command)
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
#
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
# check covariate multiplier values
for var_id in range( len(var_table) ) :
	row   = var_table[var_id]
	match = row['var_type'] == 'mulcov_rate_value'
	if match :
		fit_var_value  = fit_var_table[var_id]['fit_var_value']
		covariate_id   = row['covariate_id']
		#
		if covariate_id == 0 :
			ok = abs( 1.0 - fit_var_value / alpha_income ) < 0.1
			if not ok :
				 print('random_seed = ', random_seed)
			assert ok
		else :
			assert fit_var_value == 0.0
print('lasso_covariate.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
