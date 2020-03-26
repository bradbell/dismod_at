#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_sample_simple.py$$ $newlinech #$$
# $spell
#	init
#	dismod
#	var
#	exp
# $$
#
# $section Sampling From Then Posterior Distribution$$
#
# $head Purpose$$
# This example demonstrates using the commands
# $codei%
#	dismod_at %database% set truth_var prior_mean
#	dismod_at %database% simulate %number_sample%
#	dismod_at %database% sample simulate %number_sample%
# %$$
# This obtains the specified number of samples from the posterior distribution;
# see $cref/simulation/posterior/Simulation/$$.
#
# $head Discussion$$
# We use $latex \bar{\omega}$$ to denote the prior mean for omega.
# The following describes the model and data for this example:
#
# $list number$$
# The $cref age_table$$ has values
# $code 0.0$$, $code 100.0$$.
# The $cref time_table$$ has values
# $code 1995.0$$, $code 2015.0$$.
# $lnext
# The only node is the world.
# $lnext
# The only $cref model_variables$$ in this example are
# $cref/omega/rate_table/rate_name/omega/$$ for the world.
# This rate is modeled as constant with respect to age and
# linear between time 1995 and 2015.
# $lnext
# There are no measurements for this example.
# $lnext
# The prior for the world omega is a gaussian with mean $latex \bar{\omega}$$
# and standard deviation $latex \bar{\omega}$$.
# $lnext
# The prior for the difference in omega between time 1995
# and time 2015 is a Gaussian
# with mean zero and standard deviation $latex \bar{\omega}$$.
# $lend
#
# $head Posterior Variance$$
# We use $latex \omega_0$$ and $latex \omega_1$$ for the value
# of omega at times 1995 and 2015 respectively.
# Dropping terms that are constant with respect to $latex \omega$$
# the negative log likelihood $latex \ell ( \omega )$$ for this case is
# given by
# $latex \[
# \ell ( \omega )
# = \frac{1}{2} \bar{\omega}^{-2} \left[
#		( \omega_0 - \bar{\omega} )^2
#		+ ( \omega_1 - \omega_0 )^2
#		+ ( \omega_1 - \bar{\omega} )^2
# \right]
# \] $$
# The Hessian of this function is given by
# $latex \[
# \ell^{(2)} ( \omega ) = \bar{\omega}^{-2}
# \left( \begin{array}{cc}
#	2 & -1 \\
#	-1 & 2
# \end{array} \right)
# \] $$
# It follows that the variance of the maximum likelihood estimator is
# $latex \[
#	[ \ell^{(2)} ( \omega ) ]^{-1} = \bar{\omega}^2
# \left( \begin{array}{cc}
#	2/3 &  1/3 \\
#	1/3 &  2/3
# \end{array} \right)
# \] $$
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
#
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
omega_world_mean  = 1e-2
number_sample     = 30
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import copy
from math import exp
test_program = 'example/user/sample_simple.py'
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
	def fun_rate_world(a, t) :
		return ('prior_rate_world', None, 'prior_world_diff')
	import dismod_at
	# ----------------------------------------------------------------------
	# age table
	age_list    = [    0.0, 100.0 ]
	#
	# time table
	time_list   = [ 1995.0, 2015.0 ]
	#
	# integrand table
	integrand_table = list()
	#
	# node table:
	node_table = [ { 'name':'world',  'parent':'' } ]
	#
	# weight table
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
	#
	# data table:
	data_table = list()
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_rate_world
			'name':     'prior_rate_world',
			'density':  'gaussian',
			'mean':     omega_world_mean,
			'std':      omega_world_mean
		},{ # prior_world_diff
			'name':     'prior_world_diff',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      omega_world_mean
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	last_time_id = len(time_list) - 1
	smooth_table = [
		{ # smooth_rate_world
			'name':                     'smooth_rate_world',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0, last_time_id ],
			'fun':                       fun_rate_world
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'omega',
			'parent_smooth': 'smooth_rate_world',
			'child_smooth':  None,
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'world'             },
		{ 'name':'rate_case',              'value':'iota_zero_rho_zero'},

		{ 'name':'quasi_fixed',            'value':'true'              },
		{ 'name':'derivative_test_fixed',  'value':'first-order'       },
		{ 'name':'max_num_iter_fixed',     'value':'100'               },
		{ 'name':'print_level_fixed',      'value':'0'                 },
		{ 'name':'tolerance_fixed',        'value':'1e-11'             },

		{ 'name':'derivative_test_random', 'value':'second-order'      },
		{ 'name':'max_num_iter_random',    'value':'100'               },
		{ 'name':'print_level_random',     'value':'0'                 },
		{ 'name':'tolerance_random',       'value':'1e-11'             }
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
# Create database and run init, start, fit with just fixed effects
file_name = 'example.db'
example_db(file_name)
program   = '../../devel/dismod_at'
#
# init
dismod_at.system_command_prc([ program, file_name, 'init' ])
#
# set truth_var prior_mean
dismod_at.system_command_prc([ program, file_name, 'set', 'truth_var', 'prior_mean' ])
#
# simulate number_sample
dismod_at.system_command_prc([ program, file_name, 'simulate', str(number_sample) ])
#
# sample simulate number_sample
ns_str = str(number_sample)
dismod_at.system_command_prc([ program, file_name, 'sample', 'simulate', ns_str ])
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# check the zero random effects solution
#
# read tables
var_table     = dismod_at.get_table_dict(connection, 'var')
sample_table  = dismod_at.get_table_dict(connection, 'sample')
#
# check omega_0 and omega_1 are only variables
number_var = len(var_table)
assert number_var == 2
#
# check have proper number of rows in sample table
assert len(sample_table) == number_sample * number_var
#
# compute and check sample means
sample_mean = [ 0.0 , 0.0 ]
for row in sample_table :
	var_id     = row['var_id']
	var_value  = row['var_value']
	sample_mean[var_id] = sample_mean[var_id] + var_value
for var_id in range( number_var ) :
	sample_mean[var_id] = sample_mean[var_id] / number_sample
	assert abs ( sample_mean[var_id] / omega_world_mean - 1.0) < 0.5
#
# compute and check sample variance divided by omega_world_mean^2
variance_00 = 0.0
variance_01 = 0.0
variance_11 = 0.0
for sample_index in range(number_sample) :
	omega_0  = sample_table[ sample_index * number_var + 0 ]['var_value']
	omega_1  = sample_table[ sample_index * number_var + 1 ]['var_value']
	res_0    = ( omega_0 - omega_world_mean ) / omega_world_mean
	res_1    = ( omega_1 - omega_world_mean ) / omega_world_mean
	variance_00 += res_0 * res_0
	variance_01 += res_0 * res_1
	variance_11 += res_1 * res_1
variance_00 = variance_00 / number_sample
variance_01 = variance_01 / number_sample
variance_11 = variance_11 / number_sample
abs( variance_00 * 3.0 / 2.0 - 1.0 ) < 0.5
abs( variance_11 * 3.0 / 2.0 - 1.0 ) < 0.5
abs( variance_01 * 3.0 / 1.0 - 1.0 ) < 0.5
# -----------------------------------------------------------------------
print('posterior: OK')
# END PYTHON
