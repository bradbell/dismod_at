# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_age_avg_split.py$$ $newlinech #$$
# $spell
#	init
# $$
#
# $section Non-uniform Age Average Grid$$
#
# $head Purpose$$
# This example demonstrate one reason for using the
# $icode age_avg_split$$ option to create an
# $cref/age average grid/option_table/age_avg_split/Age Average Grid/$$
# that is not uniformly spaced.
#
# $head Variables$$
# For this case there is only one rate $icode omega$$ for the parent node
# and there are no other $cref model_variables$$.
#
# $head Prior$$
# The prior for $icode omega$$ is mean $code 0.1$$ and standard deviation
# $code .01$$ for ages $code 0.0$$ and $code 0.9$$.
# The prior for $icode omega$$ is mean $code 0.01$$ and standard deviation
# $code 0.001$$ for ages $code 1.1$$ and $code 100.0$$.
#
# $head Data$$
# There is no data for this model; i.e., the prior is the only
# information available.
#
# $head Fit$$
# A fit is done, and this should make $cref fit_var_table$$
# equal to the mean of the prior.
#
# $head Predict$$
# A predict is done for other cause mortality for two cases.
# The first case averages over the age interval $code [ 0.0, 0.9 ]$$.
# The second case averages over the age interval $code [ 1.1, 100.0 ]$$.
#
# $head ode_step_size$$
# The $cref/ode_step_size/option_table/ode_step_size/$$ is
# $code 50.0$$ for this example.
# If there were no $icode age_avg_split$$, a linear approximation
# would be used from age 0.0 to age 50.
#
# $head age_avg_split$$
# The age average grid is split at age $code 1.0$$; i.e.,
# The age average grid points are $code 0.0$$, $code 1.0$$, $code 50.0$$, and
# $code 100.0$$.
# A piecewise linear function of age is used between these grid points.
# (Note everything is constant w.r.t time for this case.)
#
# $head Source Code$$
# $srcfile%example/user/age_avg_split.py
#	%0%# BEGIN PYTHON%# END PYTHON%1
# %$$
#
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# true values used to simulate data
omega_0_1    = 1e-1
omega_1_100  = 1e-2
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
test_program = 'example/user/age_avg_split.py'
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
# Note that the a, t values are used for this example
def constant_weight_fun(a, t) :
	return 1.0
#
def fun_omega_parent(a, t) :
	if  a <= 1.0 :
		return ('prior_0_1', 'prior_none', 'prior_none')
	else :
		return ('prior_1_100', 'prior_none', 'prior_none')
#
def omega_true(a) :
	if a < 1.0 :
		return omega_0_1
	else :
		return omega_1_100
#
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ----------------------------------------------------------------------
	# age table
	age_list    = [ 0.0, 0.9, 1.1, 100.0 ]
	#
	# time table
	time_list   = [ 1995.0, 2015.0 ]
	#
	# integrand table
	integrand_table = [
		{ 'name':'mtother' }
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
	row = {
		'integrand':  'mtother',
		'node':       'world',
		'weight':     'constant'
	}
	row['age_lower'] = 0.0
	row['age_upper'] = 0.9
	row['time_lower'] = 2000.0
	row['time_upper'] = 2000.0
	avgint_table.append( copy.copy(row) )
	row['age_lower'] = 1.1
	row['age_upper'] = 100.0
	row['time_lower'] = 2000.0
	row['time_upper'] = 2000.0
	avgint_table.append( copy.copy(row) )
	#
	# nslist_table:
	nslist_table = dict()
	#
	# data table:
	data_table = list()
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_none
			'name':     'prior_none',
			'density':  'uniform',
			'mean':     0.0,
		},{ # prior_0_1
			'name':     'prior_0_1',
			'density':  'gaussian',
			'mean':     omega_0_1,
			'std':      1e-1 * omega_0_1
		},{ # prior_1_100
			'name':     'prior_1_100',
			'density':  'gaussian',
			'mean':     omega_1_100,
			'std':      1e-1 * omega_1_100
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	#
	smooth_table = [
		{ # smooth_omega_parent
			'name':                     'smooth_omega_parent',
			'age_id':                   [0, 1, 2, 3],
			'time_id':                  [0, 1],
			'fun':                      fun_omega_parent
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'omega',
			'parent_smooth': 'smooth_omega_parent'
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'world'               },
		{ 'name':'ode_step_size',          'value':'50.0'                },
		{ 'name':'age_avg_split',          'value':'1.0'                 },
		{ 'name':'random_seed',            'value':'0'                   },
		{ 'name':'rate_case',              'value':'iota_zero_rho_zero'  }
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
# ===========================================================================
file_name      = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
for command in [ 'init', 'fit', 'predict' ] :
	cmd = [ program, file_name, command ]
	if command == 'fit' :
		variables = 'both'
		cmd.append(variables)
	if command == 'predict' :
		cmd.append('fit_var')
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# Results for fitting with no noise
var_table     = dismod_at.get_table_dict(connection, 'var')
predict_table = dismod_at.get_table_dict(connection, 'predict')
#
parent_node_id = 1
eps            = 1e-4
#
# check rates values
tolerance         = 1e-10
for row in predict_table :
	avgint_id = row['avgint_id']
	if avgint_id == 0 :
		value_true = omega_0_1
	else :
		value_true = omega_1_100
	value      = row['avg_integrand']
	assert(abs(1.0 - value / value_true) < tolerance)
# -----------------------------------------------------------------------------
print('age_avg_split.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
