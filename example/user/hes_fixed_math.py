# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin user_hes_fixed_math.py$$ $newlinech #$$
# $spell
#	cppad
# $$
#
# $section Check the Hessian of the Fixed Effects Objective$$
#
# $head Purpose$$
# This is a detailed mathematical explanation of computing Hessian of the
# fixed effects objective used by the
# $cref/asymptotic/sample_command/asymptotic/$$ sampling method.
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
# (sometimes written $icode theta$$) the incidence level for the world.
# The random effects are $latex u_0$$ and $latex u_1$$.
#
# $head Problem Parameters$$
# $srccode%py%
import math
import time
theta_true     = 0.5
u_true         = [ 0.5, -0.5 ]
y_0            = theta_true * math.exp( u_true[0] )
y_1            = theta_true * math.exp( u_true[1] )
standard_dev   = theta_true # the standard deviation s
random_seed    = int( time.time() )
number_sample  = 1000
eta_in_prior   = 1e-6 # if None, the fixed effects are not scaled
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
# $head Gradient w.r.t. Fixed Effects$$
# $latex \[
#	f_\theta ( \theta, u )
#	=
#	\frac{1}{s^2} [
#		\theta \exp( 2 u_0 ) - y_0 \exp( u_0 ) +
#		\theta \exp( 2 u_1 ) - y_1 \exp( u_1 )
#	]
# \] $$
#
# $head Gradient w.r.t. Random Effects$$
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
# $head Hessian w.r.t. Fixed Effects$$
# $latex \[
#	f_{\theta,\theta} ( \theta, u )
#	=
#	\frac{1}{s^2} [ \exp( 2 u_0 ) + \exp( 2 u_1 ) ]
# \] $$
#
# $head Hessian w.r.t. Random Effects$$
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
#	\hat{u}_i^{(1)} ( \theta ) = - \frac{
#		2 \theta \exp[ 2 \hat{u}_i ( \theta) ] -
#		y_i \exp[ \hat{u}_i ( \theta) ]
#	}{
#		2 \theta^2 \exp[ 2 \hat{u}_i ( \theta) ] -
#		y_i \theta \exp[ \hat{u}_i ( \theta) ] + 1
#	}
# \] $$
# We can compute $latex \hat{u}_i ( \theta )$$ by optimizing the
# random effects corresponding to the fixed effects being $latex \theta$$.
# We define $latex g_i ( \theta )$$ by the equation
# $latex \[
#	g_i ( \theta ) = 2 \theta \exp[ 2 \hat{u}_i ( \theta) ]
#	            - y_i \exp[ \hat{u}_i ( \theta ) ]
# \] $$
# Give $latex \hat{u}_i ( \theta )$$ we can compute $latex g_i ( \theta )$$.
# Given $latex g_i ( \theta )$$, we can compute
# the derivative $latex \hat{u}_i^{(1)} ( \theta )$$ using
# $latex \[
#	\hat{u}_i^{(1)} ( \theta ) = -
#		\frac{ g_i ( \theta ) }{ \theta g_i ( \theta ) + 1}
# \] $$
# Given $latex \hat{u}^{(1)} ( \theta )$$, we can compute
# the derivative $latex g_i^{(1)} ( \theta )$$ using
# $latex \[
#	g_i^{(1)} ( \theta ) =
#	2 \exp[ 2 \hat{u}_i ( \theta) ]  +
#	\left(
#		4 \theta \exp [ 2 \hat{u}_i ( \theta ) ] -
#		y_i \exp [ \hat{u}_i ( \theta ) ]
#	\right) \hat{u}_i^{(1)} ( \theta )
# \] $$
# Given $latex g_i^{(1)} ( \theta )$$, we can compute
# the second derivative $latex \hat{u}_i^{(2)} ( \theta )$$ using
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
# Given $latex \hat{u}^{(2)} ( \theta )$$, we can compute
# the second derivative $latex g_i^{(2)} ( \theta )$$ using
# $latex \[
#	\begin{array}{rcl}
#	g_i^{(2)} ( \theta ) & = &
#	8 \exp[ 2 \hat{u}_i ( \theta) ] \hat{u}_i^{(1)} ( \theta )
#	\\ & + &
#	\left(
#		8 \theta \exp [ 2 \hat{u}_i ( \theta ) ]  -
#		y_i \exp [ \hat{u}_i ( \theta ) ]
#	\right) \hat{u}_i^{(1)} ( \theta )^2
#	\\ & + &
#	\left(
#		4 \theta \exp [ 2 \hat{u}_i ( \theta ) ] -
#		y_i \exp [ \hat{u}_i ( \theta ) ]
#	\right) \hat{u}_i^{(2)} ( \theta )
#	\end{array}
# \] $$
#
# $head Laplace Approximation$$
# Up to a constant, the Laplace approximation (fixed effects objective),
# as a function of the fixed effects, is:
# $latex \[
#	L ( \theta ) = F( \theta ) + G( \theta )
# \] $$
# where $latex F( \theta )$$ and $latex G( \theta )$$ are defined by
# $latex \[
#	F( \theta ) = f[ \theta , \hat{u} ( \theta ) ]
# \] $$
# $latex \[
#	G( \theta ) =\frac{1}{2} \log \det f_{u,u} [ \theta , \hat{u} ( \theta ) ]
# \] $$
# The derivative of $latex F( \theta )$$ is given by
# $latex \[
#	F^{(1)} ( \theta ) =
#	f_\theta [ \theta , \hat{u} ( \theta ) ] +
#	f_u [ \theta , \hat{u} ( \theta ) ]  \hat{u}^{(1)} ( \theta )
# \] $$
# It follows from the definition of $latex \hat{u} ( \theta )$$ that
# $latex f_u [ \theta , \hat{u} ( \theta ) ] = 0$$ and
# $latex \[
#	F^{(1)} ( \theta ) = f_\theta [ \theta , \hat{u} ( \theta ) ]
# \] $$
# Taking the derivative of the equation above we obtain
# $latex \[
#	F^{(2)} ( \theta ) =
#	f_{\theta,\theta} [ \theta , \hat{u} ( \theta ) ] +
#	f_{\theta,u} [ \theta , \hat{u} ( \theta ) ] \hat{u}^{(1)} ( \theta )
# \] $$
# Combining the definition of $latex G( \theta )$$, $latex g_i ( \theta )$$
# and the formula for $latex f_{u,u} ( \theta , u )$$ we have
# $latex \[
#	G( \theta )
#	=
#	\frac{1}{2} \log \det \left(
#	\begin{array}{cc}
#		[ \theta g_0 ( \theta ) + 1 ] / s^2 & 0
#		\\
#		0   & [ \theta g_1 ( \theta ) + 1 ] / s^2
#	\end{array}
#	\right)
# \] $$
# Defining $latex h_i ( \theta )$$ by
# $latex \[
#	h_i ( \theta ) = \theta g_i ( \theta ) + 1
# \]$$
# we obtain the representation
# $latex \[
#	G ( \theta ) =
#	+ \frac{1}{2} \left(
#	\log [ h_0 ( \theta ) ] + \log [ h_1 ( \theta ) ] - \log ( s^4 )
#	\right)
# \] $$
# The first and second derivative of $latex h_i ( \theta )$$
# and $latex G( \theta )$$ are given by
# $latex \[
#	h_i^{(1)} ( \theta ) = g_i ( \theta ) + \theta g_i^{(1)} ( \theta )
# \] $$
# $latex \[
# G^{(1)} ( \theta ) =
#	\frac{1}{2} \left(
#		\frac{ h_0^{(1)} ( \theta ) }{ h_0 ( \theta ) }
#		+
#		\frac{ h_1^{(1)} ( \theta ) }{ h_1 ( \theta ) }
#	\right)
# \] $$
# $latex \[
#	h_i^{(2)} ( \theta ) = 2 g_i^{(1)} ( \theta ) + \theta g_i^{(2)} ( \theta )
# \] $$
# $latex \[
# G^{(2)} ( \theta ) =
#	\frac{1}{2} \left(
#		\frac{ h_0 ( \theta ) h_0^{(2)} ( \theta ) - h_0^{(1)} ( \theta )^2 }
#		{ h_0 ( \theta )^2 }
#		+
#		\frac{ h_1 ( \theta ) h_1^{(2)} ( \theta ) - h_1^{(1)} ( \theta )^2 }
#		{ h_1 ( \theta )^2 }
#	\right)
# \] $$
#
# $head Asymptotic Statistics$$
# The asymptotic posterior distribution for the optimal estimate of
# $latex \theta$$ give the data $latex y$$
# is a normal with variance equal to the inverse of
# $latex \[
#	L^{(2)} ( \theta ) = F^{(2)} ( \theta ) + G^{(2)} ( \theta )
# \] $$
#
# $head Scaling Fixed Effects$$
# If $cref/eta/prior_table/eta/$$ is not null,
# the Hessian is with respect to $latex \alpha = \log( \theta + \eta )$$.
# Inverting this relation we define
# $latex \theta ( \alpha ) = \exp( \alpha ) - \eta$$.
# The fixed effects objective as a function of $latex \alpha$$ is
# $latex \[
#	H( \alpha ) =
#	L[ \theta ( \alpha ) ] =
#	F[ \theta( \alpha ) ] + G[ \theta( \alpha ) ]
# \] $$
# Taking the first and second derivatives, we have
# $latex \[
#	H^{(1)}( \alpha ) =  \left(
#		F^{(1)}[ \theta( \alpha ) ] + G^{(1)}[ \theta( \alpha ) ]
#	\right) \exp( \alpha )
# \] $$
# $latex \[
#	\begin{array}{rcl}
#	H^{(2)}( \alpha ) & = & \left(
#		F^{(1)}[ \theta( \alpha ) ] + G^{(1)}[ \theta( \alpha ) ]
#	\right) \exp( \alpha )
#	\\ & + &
#	\left(
#		F^{(2)}[ \theta( \alpha ) ] + G^{(2)}[ \theta( \alpha ) ]
#	\right) \exp( 2 \alpha )
#	\end{array}
# \] $$
#
# $subhead Optimal Fixed Effects$$
# The first order necessary conditions for
# $latex \hat{\alpha}$$
# to be a minimizer of the fixed effects object is
# $latex H^{(1)} ( \hat{\alpha} ) = 0$$.
# In this case we can simplify the Hessian scaling as follows:
# $latex \[
#	\begin{array}{rcl}
#	H^{(2)}( \hat{\alpha} ) & = & \left(
#		F^{(2 }( \hat{\theta} ) + G^{(2)}( \hat{\theta} )
#	\right) \exp( 2 \hat{\alpha} )
#	\\ & = &
#	L^{(2)} ( \hat{\theta} ) \exp( 2 \hat{\alpha} )
#	\end{array}
# \] $$
# where $latex \hat{\theta} = \theta( \hat{\alpha} )$$.
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
test_program = 'example/user/hes_fixed_math.py'
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
			'eta':      eta_in_prior,
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
		{ 'name':'print_level_fixed',      'value':'0'                  },
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
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
dismod_at.system_command_prc(
	[ program, file_name, 'sample', 'asymptotic', 'both', str(number_sample) ]
)
# -----------------------------------------------------------------------
# get tables
new           = False
connection    = dismod_at.create_connection(file_name, new)
var_table     = dismod_at.get_table_dict(connection, 'var')
node_table    = dismod_at.get_table_dict(connection, 'node')
rate_table    = dismod_at.get_table_dict(connection, 'rate')
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
sample_table  = dismod_at.get_table_dict(connection, 'sample')
hes_fixed_table = dismod_at.get_table_dict(connection, 'hes_fixed')
connection.close()
dismod_at.db2csv_command(file_name)
# -----------------------------------------------------------------------
# node_name2var_id
node_name2var_id = dict()
assert len(var_table) == 3
for var_id in range(len(var_table) ) :
	assert var_id < 3
	row = var_table[var_id]
	assert row['var_type'] == 'rate'
	assert rate_table[row['rate_id']]['rate_name']  == 'iota'
	node_name = node_table[row['node_id']]['node_name']
	node_name2var_id[node_name] = var_id
# -----------------------------------------------------------------------
# uhat(theta)
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
# ------------------------------------------------------------------------
# f(theta, u)
def random_likelihood(fixed_effect, random_effect) :
	theta    = fixed_effect
	u        = random_effect
	y        = numpy.array( [y_0, y_1] )
	residual = y - theta * numpy.exp(u)
	residual = numpy.append(residual, u)
	sum_sq   = numpy.sum( residual * residual )
	return sum_sq / (2.0 * standard_dev * standard_dev)
# -----------------------------------------------------------------------
# log det f_{u,u} ( theta , u )
def log_det_random_hessian(fixed_effect, random_effect) :
	theta    = fixed_effect
	u        = random_effect
	exp_u    = numpy.exp( u )
	exp_2u   = numpy.exp( 2.0 * u )
	g        = 2.0 * theta * exp_2u - y * exp_u
	h        = theta * g + 1.0
	s_sq     = standard_dev * standard_dev
	log_det  = numpy.sum( numpy.log(h / s_sq) )
	return log_det
# -----------------------------------------------------------------------
def check_rel_error(check, value, tolerance) :
	rel_error = abs( check / value - 1.0 )
	# print(rel_error, tolerance)
	if numpy.isscalar( rel_error ) :
		ok = rel_error < tolerance
	else :
		ok = all( rel_error < tolerance )
	if not ok :
		print("random_seed =", random_seed)
	assert ok
# -----------------------------------------------------------------------
# Some values
#
# s_sq
s_sq         = standard_dev * standard_dev
#
# y
y            = numpy.array( [ y_0, y_1 ] )
#
# optimal theta (from the fit_var table)
var_id       = node_name2var_id['world']
theta        = fit_var_table[var_id]['fit_var_value']
#
# delta_theta
delta_theta  = theta / 1000.0
theta_plus   = theta + delta_theta
theta_minus  = theta - delta_theta
#
# uhat
uhat         = optimal_random_effect(theta)
uhat_plus    = optimal_random_effect(theta_plus)
uhat_minus   = optimal_random_effect(theta_minus)
#
# exp_u
exp_u        = numpy.exp( uhat )
exp_u_plus   = numpy.exp( uhat_plus )
exp_u_minus  = numpy.exp( uhat_minus )
#
# exp_2u
exp_2u       = numpy.exp( 2.0 * uhat )
exp_2u_plus  = numpy.exp( 2.0 * uhat_plus )
exp_2u_minus = numpy.exp( 2.0 * uhat_minus )
#
# g(theta)
g            = 2.0 * theta * exp_2u - y * exp_u
g_plus       = 2.0 * theta_plus * exp_2u_plus - y * exp_u_plus
g_minus      = 2.0 * theta_minus * exp_2u_minus - y * exp_u_minus
#
# h(theta)
h = theta * g + 1
#
# F(theta)
F       = random_likelihood(theta, uhat)
F_plus  = random_likelihood(theta_plus, uhat_plus)
F_minus = random_likelihood(theta_minus, uhat_minus)
#
# G(theta)
G       = log_det_random_hessian(theta, uhat) / 2.0
G_plus  = log_det_random_hessian(theta_plus, uhat_plus) / 2.0
G_minus = log_det_random_hessian(theta_minus, uhat_minus) / 2.0
#
# L(theta)
L       = F + G
L_plus  = F_plus + G_plus
L_minus = F_minus + G_minus
#
# f_u (theta, uhat)
f_u     = ( theta * theta * exp_2u - theta * y  * exp_u  + uhat ) / s_sq
#
# f_theta (theta, uhat)
f_theta = numpy.sum(theta * exp_2u - y * exp_u) / s_sq
#
# f_{theta,theta} ( theta , uhat )
f_theta_theta = numpy.sum( exp_2u ) / s_sq
#
# f_{theta, u} ( theta , uhat )
f_theta_u = ( 2 * theta * exp_2u - y * exp_u ) / s_sq
#
# dF_dtheta = F^{(1)} ( theta )
dF_dtheta = f_theta
#
# duhat_dtheta = uhat^{(1)} ( theta )
duhat_dtheta = - g / (theta * g + 1)
#
# dg_dtheta = g^{(1)} ( theta )
dg_dtheta  = 2.0 * exp_2u + (4.0 * theta * exp_2u - y * exp_u) * duhat_dtheta
#
# d2uhat_dtheta = uhat^{(2)} ( theta )
d2uhat_d2theta = (g * g - dg_dtheta) / ( (theta * g + 1) * (theta * g + 1) )
#
# d2F_d2theta = F^{(2)} ( theta )
d2F_d2theta  = f_theta_theta
d2F_d2theta += numpy.dot( f_theta_u, duhat_dtheta )
#
# dh_dtheta = h^{(1)} ( theta )
dh_dtheta = g + theta * dg_dtheta
#
# dG_dtheta = G^{(1)} ( theta )
dG_dtheta = numpy.sum( dh_dtheta / h ) / 2.0
#
# d2g_d2theta = g^{(2)} ( theta )
duhat_dtheta_sq = duhat_dtheta * duhat_dtheta
d2g_d2theta     = 8.0 * exp_2u * duhat_dtheta
d2g_d2theta    += (8.0 * theta * exp_2u - y * exp_u) * duhat_dtheta_sq
d2g_d2theta    += (4.0 * theta * exp_2u - y * exp_u) * d2uhat_d2theta
#
# dh2_d2theta = h^{(2)} ( theta )
d2h_d2theta = 2.0 * dg_dtheta + theta * d2g_d2theta
#
# d2G_d2theta = G^{(2)} ( theta )
d2G_d2theta = (h * d2h_d2theta - dh_dtheta * dh_dtheta) / (2.0 * h * h)
d2G_d2theta = numpy.sum( d2G_d2theta )
# ----------------------------------------------------------------------------
# check that f_u ( theta , uhat ) = 0
assert all( abs(f_u) < 1e-13 )
#
# check that dF_dtheta + dG_dtheta = 0
assert abs( dF_dtheta + dG_dtheta ) < 1e-11
#
# check duhat_dtheta
check      = (uhat_plus - uhat_minus) / (2.0 * delta_theta)
check_rel_error(check, duhat_dtheta, 1e-5)
#
# check dg_dtheta
check      = (g_plus - g_minus) / (2.0 * delta_theta)
check_rel_error(check, dg_dtheta, 1e-6)
#
# check d2uhat_d2theta
check = (uhat_plus - 2.0 * uhat + uhat_minus) / (delta_theta * delta_theta)
check_rel_error(check, d2uhat_d2theta, 1e-6)
#
# check d2F_d2theta
check = (F_plus - 2.0 * F + F_minus) / (delta_theta * delta_theta)
check_rel_error(check, d2F_d2theta, 1e-6)
#
# check dG_dtheta
check   = (G_plus - G_minus) / (2.0 * delta_theta)
check_rel_error(check, dG_dtheta, 1e-6)
#
check        = (g_plus - 2.0 * g + g_minus) / (delta_theta * delta_theta)
check_rel_error(check, d2g_d2theta, 1e-6)
#
# check d2G_d2theta
check = (G_plus - 2.0 * G + G_minus) / (delta_theta * delta_theta)
check_rel_error(check, d2G_d2theta, 1e-5)
# ============================================================================
# check the Hessian of the fixed effects objective
#
# The world rate for incidence is the only fixed effect
world_var_id = node_name2var_id['world']
assert len(hes_fixed_table) == 1
row = hes_fixed_table[0]
assert row['row_var_id'] == world_var_id
assert row['col_var_id'] == world_var_id
hes_fixed_value = row['hes_fixed_value']
if eta_in_prior == None :
	dL2_d2theta     = d2F_d2theta + d2G_d2theta
	check_rel_error(dL2_d2theta, hes_fixed_value, 1e-14)
else :
	alpha        = math.log(theta + eta_in_prior)
	exp_alpha    = math.exp(alpha)
	exp_2alpha   = math.exp( 2.0 * alpha )
	dL_dtheta    = dF_dtheta + dG_dtheta
	d2L_d2theta  = d2F_d2theta + d2G_d2theta
	d2H_d2alpha  = dL_dtheta * exp_alpha + d2L_d2theta * exp_2alpha
	check_rel_error(d2H_d2alpha, hes_fixed_value, 1e-14)
#
# compute sample statistics
assert  len(sample_table) == number_sample * len(var_table)
sample_array  = numpy.zeros(number_sample, dtype = float)
for row in sample_table :
	if row['var_id'] == var_id :
		sample_index = row['sample_index']
		if eta_in_prior == None :
			sample_value = row['var_value']
		else :
			sample_value = math.log( row['var_value'] + eta_in_prior )
		sample_array[sample_index] = sample_value
sample_avg = numpy.average(sample_array)
sample_var = numpy.var(sample_array, ddof=1)
#
# check sample statistics
if eta_in_prior == None :
	check_rel_error(theta,   sample_avg,  1e-1)
else :
	check_rel_error(math.log(theta + eta_in_prior),   sample_avg,  1e-1)
check_rel_error(1.0 / hes_fixed_value, sample_var, 1e-1)
#
print('hes_fixed_math.py: OK')
# END PYTHON
