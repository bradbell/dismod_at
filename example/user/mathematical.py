# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_mathematical.py$$ $newlinech #$$
# $spell
#	cppad
# $$
#
# $section A Simple Example With Detailed Mathematical Explanation$$
#
# $head Purpose$$
# This is a detailed mathematical explanation of a very simple example.
#
# $head Reference$$
# See the
# $href%https://bradbell.github.io/cppad_mixed/doc/theory.htm%theory%$$
# section of the $code cppad_mixed$$ documentation.
#
# $head Notation$$
# $table
# $latex \theta$$ $cnext model incidence for the parent region $rnext
# $latex u_0$$    $cnext incidence random effect for first child    $rnext
# $latex u_1$$    $cnext incidence random effect for second child   $rnext
# $latex y_0$$    $cnext measured incidence for the first child $rnext
# $latex y_1$$    $cnext measured incidence for the second child $rnext
# $latex s$$      $cnext standard deviation for data and random effects $rnext
# $tend
# The only fixed effect in this model is $latex \theta$$
# (sometimes written $icode theta$$)
# the incidence level for north_america.
#  The random effects are $latex u_0$$ and $latex u_1$$.
#
# $head Problem Parameters$$
# $srccode%py%
import math
import time
theta_true   = 1.0
u_true       = [ 0.5, -0.5 ]
y_0          = theta_true * math.exp( u_true[0] )
y_1          = theta_true * math.exp( u_true[1] )
standard_dev = theta_true # the standard deviation s
random_seed = int( time.time() )
# %$$
#
# $head Random Likelihood$$
# The negative log-likelihood for this example, ignoring the constant
# of integration, is
# $latex \[
#	f( \theta, u )
#	= \frac{1}{2 s^2} \left(
#		[ y_0 - \theta \exp( u_0 ) ]^2 +
#		[ y_1 - \theta \exp( u_1 ) ]^2 +
#		u_0^2 +
#		u_1^2
#	\right)
# \] $$
#
# $head Gradient W.R.T. Random Effects$$
# $latex \[
#	f_u ( \theta, u )
#	=
#	\frac{1}{s^2} \left(
#	\begin{array}{c}
#		\theta^2 \exp( 2 u_0 ) - y_0 \theta \exp( u_0 )  + u_0
#		\\
#		\theta^2 \exp( 2 u_1 ) - y_1 \theta \exp( u_1 )  + u_1
#	\end{array}
#	\right)
# \] $$
#
# $head Hessian W.R.T. Random Effects$$
# $latex \[
#	f_{u,u} ( \theta, u )
#	=
#	\frac{1}{s^2} \left(
#	\begin{array}{cc}
#		2 \theta^2 \exp( 2 u_0 ) - y_0 \theta \exp( u_0 ) + 1   & 0
#		\\
#		0   & 2 \theta^2 \exp( 2 u_1 ) - y_1 \theta \exp( u_1 ) + 1
#	\end{array}
#	\right)
# \] $$
#
# $head Hessian Cross Term$$
# $latex \[
#	f_{u,\theta} ( \theta, u )
#	=
#	\frac{1}{s^2} \left(
#	\begin{array}{c}
#		2 \theta \exp( 2 u_0 ) - y_0 \exp( u_0 )
#		\\
#		2 \theta \exp( 2 u_1 ) - y_1 \exp( u_1 )
#	\end{array}
#	\right)
# \] $$
#
# $head Optimal Random Effects$$
#
# $subhead Implicit Function Definition$$
# The optimal random effects $latex \hat{u} ( \theta )$$
# solve the equation
# $latex \[
#	f_u [ \theta , \hat{u} ( \theta ) ] = 0
# \] $$
#
# $subhead  Derivatives of Optimal Random Effects$$
# Using the implicit function theorem we have
# $latex \[
#	\hat{u}^{(1)} ( \theta )= -
#		f_{u,u} [ \theta , \hat{u} ( \theta) ]^{-1}
#			f_{u,\theta} [ \theta , \hat{u} ( \theta) ]
# \] $$
# Substituting in the formulas above for the Hessian terms on the
# right hand side we obtain:
# $latex \[
#	\hat{u}_i^{(1)} ( \theta ) = -
#		\frac{ 2 \theta \exp( 2 u_i ) - y_i \exp( u_i )}
#		{ 2 \theta^2 \exp( 2 u_i ) - y_i \theta \exp( u_i ) + 1}
# \] $$
# evaluated at $latex u_i = \hat{u}_i ( \theta )$$.
# Defining $latex g_i ( \theta )$$ by
# $latex \[
#	g_i ( \theta ) = 2 \theta \exp[ 2 \hat{u}_i ( \theta) ]
#	            - y_i \exp[ \hat{u}_i ( \theta ) ]
# \] $$
# we can rewrite the derivative above as
# $latex \[
#	\hat{u}_i^{(1)} ( \theta ) = -
#		\frac{ g_i ( \theta ) }{ \theta g_i ( \theta ) + 1}
# \] $$
# $latex \[
#	g_i^{(1)} ( \theta ) =
#	2 \exp[ 2 \hat{u}_i ( \theta) ]  +
#	4 \theta \exp [ 2 \hat{u}_i ( \theta ) ] \hat{u}_i^{(1)} ( \theta ) -
#	y_i \exp [ \hat{u}_i ( \theta ) ] \hat{u}_i^{(1)} ( \theta )
# \] $$
# $latex \[
#	\hat{u}_i^{(2)} ( \theta ) =
#	\frac{ g_i ( \theta ) [ g_i ( \theta ) + \theta g_i ^{(1)} ( \theta ) ] }
#	{ [ \theta g_i ( \theta ) + 1 ]^2 }
#	-
#	\frac{ g_i ^{(1)}( \theta ) }{ \theta g_i ( \theta ) + 1}
# \] $$
# $latex \[
#	\hat{u}_i^{(2)} ( \theta ) =
#	\frac{ g_i ( \theta )^2 - g_i ^{(1)}( \theta )}
#	{ [ \theta g_i ( \theta ) + 1 ]^2 }
# \] $$
#
# $head Laplace Approximation$$
# The Laplace approximation (up to a constant), as a function of the
# fixed effects, is:
# $latex \[
#	L ( \theta ) = f[ \theta , \hat{u} ( \theta ) ] +
#	\frac{1}{2} \log \det f_{u,u} [ \theta , \hat{u} ( \theta ) ]
# \] $$
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import numpy
import sys
import os
import copy
import distutils.dir_util
test_program = 'example/user/mathematical.py'
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
def example_db (file_name) :
	def fun_rate_child(a, t) :
		return ('prior_gauss_zero', None, None)
	def fun_rate_parent(a, t) :
		return ('prior_rate_parent', None, None)
	# ----------------------------------------------------------------------
	# age table
	age_list    = [    0.0,   100.0 ]
	#
	# time table
	time_list   = [ 1995.0,  2015.0 ]
	#
	# integrand table
	integrand_table = [ { 'name':'Sincidence' } ]
	#
	node_table = [
		{ 'name':'world',    'parent':''       },
		{ 'name':'child_0',  'parent':'world'  },
		{ 'name':'child_1',  'parent':'world'  },
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
	# avgint table:
	avgint_table = list()
	#
	# nslist_table:
	nslist_table = dict()
	# ----------------------------------------------------------------------
	# data table:
	data_table = list()
	row = {
		'node':        'child_0',
		'subgroup':    'world',
		'density':     'gaussian',
		'weight':      '',
		'hold_out':     False,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    50.0,
		'age_upper':    50.0,
		'integrand':   'Sincidence',
		'meas_value':   y_0,
		'meas_std':     standard_dev,
	}
	data_table.append( copy.copy(row) )
	#
	row['node']       = 'child_1'
	row['meas_value'] = y_1
	data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    1e-2 * theta_true,
			'upper':    1e+2 * theta_true,
			'mean':     theta_true / 3.0, # only used for start and scale
		},{ # prior_gauss_zero
			'name':     'prior_gauss_zero',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      standard_dev,
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
		}, { # smooth_rate_child
			'name':                     'smooth_rate_child',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'fun':                      fun_rate_child
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'iota',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child',
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'world'              },
		{ 'name':'ode_step_size',          'value':'10.0'               },
		{ 'name':'random_seed',            'value':str(random_seed)     },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero'  },

		{ 'name':'quasi_fixed',            'value':'true'               },
		{ 'name':'derivative_test_fixed',  'value':'none'               },
		{ 'name':'max_num_iter_fixed',     'value':'100'                },
		{ 'name':'print_level_fixed',      'value':'5'                  },
		{ 'name':'tolerance_fixed',        'value':'1e-12'              },

		{ 'name':'derivative_test_random', 'value':'none'               },
		{ 'name':'max_num_iter_random',    'value':'100'                },
		{ 'name':'print_level_random',     'value':'0'                  },
		{ 'name':'tolerance_random',       'value':'1e-12'              }
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
# ===========================================================================
file_name  = 'example.db'
example_db(file_name)
#
program   = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
# -----------------------------------------------------------------------
# get tables
new           = False
connection    = dismod_at.create_connection(file_name, new)
var_table     = dismod_at.get_table_dict(connection, 'var')
node_table    = dismod_at.get_table_dict(connection, 'node')
rate_table    = dismod_at.get_table_dict(connection, 'rate')
connection.close()
dismod_at.db2csv_command(file_name)
# -----------------------------------------------------------------------
# node_name2var_id
node_name2var_id = dict()
for var_id in range(len(var_table) ) :
	assert var_id < 3
	row = var_table[var_id]
	assert row['var_type'] == 'rate'
	assert rate_table[row['rate_id']]['rate_name']  == 'iota'
	node_name = node_table[row['node_id']]['node_name']
	node_name2var_id[node_name] = var_id
# -----------------------------------------------------------------------
# optimal_random_effect(fixed_effect)
def optimal_random_effect(fixed_effect) :
	#
	# set start_var value for the fixed effects
	var_id = node_name2var_id['world']
	sql_cmd  = 'UPDATE start_var SET start_var_value = ' + str(fixed_effect)
	sql_cmd += ' WHERE start_var_id == ' + str(var_id)
	new           = False
	connection    = dismod_at.create_connection(file_name, new)
	dismod_at.sql_command(connection, sql_cmd)
	connection.close()
	#
	# optimize the random effects
	dismod_at.system_command_prc([ program, file_name, 'fit', 'random' ])
	#
	# retrieve the optimal random effects
	new           = False
	connection    = dismod_at.create_connection(file_name, new)
	fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
	#
	var_id        = node_name2var_id['child_0']
	uhat_0        = fit_var_table[var_id]['fit_var_value']
	#
	var_id        = node_name2var_id['child_1']
	uhat_1        = fit_var_table[var_id]['fit_var_value']
	#
	uhat          = numpy.array( [ uhat_0, uhat_1 ] )
	return uhat
# -----------------------------------------------------------------------
# Some constants
#
# s_sq
s_sq      = standard_dev * standard_dev
#
# y
y         = numpy.array( [ y_0, y_1 ] )
#
# theta
theta = theta_true
#
# uhat
uhat = optimal_random_effect(theta)
#
# theta_sq
theta_sq = theta * theta
#
# exp_u
exp_u = numpy.exp( uhat )
#
# exp_2u
exp_2u = numpy.exp( 2.0 * uhat )
#
# check that f_u ( theta , uhat ) = 0
f_u = ( theta_sq * exp_2u - theta * y  * exp_u  + uhat ) / s_sq
assert all( abs(f_u) < 1e-13 )
#
# g(theta)
g = 2.0 * theta * exp_2u - y * exp_u
#
# duhat_dtheta
duhat_dtheta = - g / (theta * g + 1)
#
# delta_theta
delta_theta = theta_true / 100.0
#
# check duhat_dtheta
uhat_plus  = optimal_random_effect(theta + delta_theta)
uhat_minus = optimal_random_effect(theta - delta_theta)
check      = (uhat_plus - uhat_minus) / (2.0 * delta_theta)
assert all( abs( check / duhat_dtheta - 1.0 ) < 1e-4 )
#
# dg_dtheta
dg_dtheta  = 2.0 * exp_2u + (4.0 * theta * exp_2u - y * exp_u) * duhat_dtheta
#
# d2uhat_dtheta
d2uhat_d2theta = (g * g - dg_dtheta) / ( (theta * g + 1) * (theta * g + 1) )
#
# check d2uhat_d2theta
check = (uhat_plus - 2.0 * uhat + uhat_minus) / (delta_theta * delta_theta)
assert all( abs( check / d2uhat_d2theta - 1.0 ) < 1e-3 )
#
# -----------------------------------------------------------------------
print('mathematical.py: OK')
# END PYTHON
