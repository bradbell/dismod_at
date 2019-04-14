#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_ill_condition.py$$ $newlinech #$$
# $spell
#	std
# $$
#
# $section An Ill Conditioned Example Where Re-Scaling is Helpful$$
#
# $head Notation$$
# $table
# $latex \bar{\chi}$$
#	$cnext measured value for excess mortality at age zero $rnext
# $latex \sigma$$     $cnext std for the measurement noise $rnext
# $latex \delta$$     $cnext std for age difference for $latex \chi$$ $rnext
# $latex \chi_0$$     $cnext estimate for $latex \chi$$ at age zero $rnext
# $latex \chi_1$$     $cnext estimate for $latex \chi$$ at age 100  $rnext
# $tend
#
# $head Objective$$
# For this case the negative log likelihood is, not counting terms
# that are constant w.r.t $latex ( \chi_0, \chi_1 )$$,
# $latex \[
#	f( \chi_0 , \chi_1 )
#	=
#	\frac{1}{2} \left( \frac{\chi_0 - \bar{\chi}}{\sigma} \right)^2
#	+
#	\frac{1}{2} \left( \frac{\chi_1 - \chi_0}{\delta} \right)^2
# \] $$
#
# $head Derivative$$
# The partial of the objective w.r.t. $latex \chi_0$$ is
# $latex \[
#	\partial_0 f ( \chi_0 , \chi_1 )
#	=
#	\frac{\chi_0 - \bar{\chi}}{ \sigma^2 }
#	-
#	\frac{\chi_1 - \chi_0}{\delta^2}
# \] $$
# The partial of the objective w.r.t. $latex \chi_1$$ is
# $latex \[
#	\partial_1 f ( \chi_0 , \chi_1 )
#	=
#	\frac{\chi_1 - \chi_0}{\delta^2}
# \] $$
#
#
# $head Ill-Conditioning$$
# If $latex \sigma$$ is much smaller than $latex \delta$$,
# then the optimization problem will weight the measurement equation
# $latex \chi_0 - \bar{\chi} = 0$$ much more than the difference equation
# $latex \chi_1 - \chi_0 = 0$$.
# If $latex \sigma$$ is much larger than $latex \delta$$,
# then the optimization problem will weight the difference equation
# much more than the measurement equation.
#
# $head Scaling$$
# Note that the initial scaling, in the example below, uses the
# mean values for $latex \chi_0$$, $latex \chi_1$$ which are zero.
#
# $head Source Code$$
# $srcfile%
#	example/user/ill_condition.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# You can changed the values below and rerun this program
chi_bar          = 0.01
sigma            = 1e-5 * chi_bar
delta            = 1e-1 * chi_bar
# You can changed the values above and rerun this program
# ---------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
test_program = 'example/user/ill_condition.py'
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
def weight_fun(a, t) :
	return 1.0
def fun_chi(a, t) :
	return ('prior_chi', 'prior_dchi', None)
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ 0.0, 100.0 ]
	#
	# time table:
	time_list   = [ 1990.0, 2010.0 ]
	#
	# integrand table:
	integrand_table = [
		 { 'name': 'mtexcess' }
	]
	#
	# node table:
	node_table = [ { 'name':'world', 'parent':'' } ]
	#
	# weight table:
	# The constant function 1.0, note any valid age and time id would work
	name    = 'constant'
	fun     = weight_fun
	age_id  = 0
	time_id = 0
	weight_table = [
		{ 'name':name,  'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	]
	#
	# covariate table: empty
	covariate_table = list()
	#
	# mulcov table: empty
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
	#
	# values that are the same for all data rows
	row = {
		'meas_value':  chi_bar ,
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'age_lower':    age_list[0],
		'age_upper':    age_list[0],
		'time_lower':   2000.,
		'time_upper':   2000.,
		'integrand':   'mtexcess',
		'meas_std':     sigma,
		'node':        'world'
	}
	data_table.append(row)
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_chi
			'name':     'prior_chi',
			'density':  'uniform',
			'lower':    0.0,
			'upper':    1.0,
			'mean':     0.0
		},{ # prior_dchi
			'name':     'prior_dchi',
			'density':  'gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      delta
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	name           = 'smooth_chi'
	fun            = fun_chi
	smooth_table = [
		{'name':name, 'age_id':[0,1], 'time_id':[0], 'fun':fun }
	]
	# ----------------------------------------------------------------------
	# rate table:
	rate_table = [
		{	'name':          'chi',
			'parent_smooth': 'smooth_chi',
			'child_smooth':  None
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'rate_case',              'value':'iota_zero_rho_zero' },
		{ 'name':'parent_node_name',       'value':'world'             },
		{ 'name':'random_seed',            'value':'0'                 },
		{ 'name':'quasi_fixed',            'value':'false'             },
		{ 'name':'derivative_test_fixed',  'value':'second-order'      },
		{ 'name':'max_num_iter_fixed',     'value':'100'               },
		{ 'name':'print_level_fixed',      'value':'0'                 },
		{ 'name':'tolerance_fixed',        'value':'1e-8'              }
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
# create example.db
file_name      = 'example.db'
example_db(file_name)
#
# init command
program        = '../../devel/dismod_at'
cmd            = [ program, file_name, 'init' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at init command failed')
#
# fit command
cmd            = [ program, file_name, 'fit', 'fixed' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at fit command failed')
#
# read database
new             = False
connection      = dismod_at.create_connection(file_name, new)
age_table       = dismod_at.get_table_dict(connection, 'age')
rate_table      = dismod_at.get_table_dict(connection, 'rate')
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
integrand_table = dismod_at.get_table_dict(connection, 'integrand')
#
for var_id in range( len(var_table) ) :
	row        = var_table[var_id]
	rate_id    = row['rate_id']
	age_id     = row['age_id']
	time_id    = row['time_id']
	fit_value  = fit_var_table[var_id]['fit_var_value']
	assert row['var_type'] == 'rate'
	assert rate_table[rate_id]['rate_name'] == 'chi'
	assert time_id == 0
	if age_id == 0 :
		# this point is informed by the data so it fits
		assert abs( 1.0 - fit_value / chi_bar ) < 1e-6
	else :
		# this point is only informed by prior which has a large
		# standard deviation compared to the data
		assert abs( 1.0 - fit_value / chi_bar ) > 1.0
#
# rescale
cmd            = [ program, file_name, 'set', 'scale_var', 'fit_var' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at set command failed')
#
# fit command
cmd            = [ program, file_name, 'fit', 'fixed' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at fit command failed')
#
# read new fit values
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
for var_id in range( len(var_table) ) :
	age_id     = row['age_id']
	fit_value  = fit_var_table[var_id]['fit_var_value']
	if age_id == 0 :
		# this point is informed by the data so it fits
		assert abs( 1.0 - fit_value / chi_bar ) < 1e-10
	else :
		# this point is only informed by prior which has a large
		# standard deviation compared to the data
		assert abs( 1.0 - fit_value / chi_bar ) < 1e-5

# -----------------------------------------------------------------------------
print('ill_condition.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
