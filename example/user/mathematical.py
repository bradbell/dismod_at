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
#	init
#	dismod
# $$
#
# $section An Example With Mathematical Explanation$$
#
# $head Purpose$$
# This is a very simple example for which we check many of the
# dismod_at calculations with analytic formulas
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
# $head Negative Log-Likelihood$$
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
# $head Random Effect Gradient$$
# $latex \[
#	f_u ( \theta, u )
#	=
#	\frac{1}{s^2} \left(
#	\begin{array}{c}
#		\theta^2 \exp( 2 u_0 ) - \theta \exp( u_0 ) y_0  + u_0
#		\\
#		\theta^2 \exp( 2 u_1 ) - \theta \exp( u_1 ) y_1  + u_1
#	\end{array}
#	\right)
# \] $$
#
# $head Optimal Random Effects$$
# The optimal random effects $latex \hat{u} ( \theta )$$
# solve the equation
# $latex \[
#	f_u [ \theta , \hat{u} ( \theta ) ] = 0
# \] $$
#
# $end
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
dismod_at.system_command_prc([ program, file_name, 'fit', 'random' ])
# -----------------------------------------------------------------------
# get tables
new           = False
connection    = dismod_at.create_connection(file_name, new)
var_table     = dismod_at.get_table_dict(connection, 'var')
node_table    = dismod_at.get_table_dict(connection, 'node')
rate_table    = dismod_at.get_table_dict(connection, 'rate')
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
connection.close()
dismod_at.db2csv_command(file_name)
# -----------------------------------------------------------------------
# map from node name to variable id
node_name2var_id = dict()
for var_id in range(len(var_table) ) :
	assert var_id < 3
	row = var_table[var_id]
	assert row['var_type'] == 'rate'
	assert rate_table[row['rate_id']]['rate_name']  == 'iota'
	node_name = node_table[row['node_id']]['node_name']
	node_name2var_id[node_name] = var_id
# -----------------------------------------------------------------------
# Some valuse related to the fit
#
# theta
var_id    = node_name2var_id['world']
theta     = fit_var_table[var_id]['fit_var_value']
#
# u
var_id    = node_name2var_id['child_0']
u_0       = fit_var_table[var_id]['fit_var_value']
var_id    = node_name2var_id['child_1']
u_1       = fit_var_table[var_id]['fit_var_value']
u         = [ u_0, u_1 ]
#
# theta_sq
theta_sq  = theta * theta
#
# exp_u
exp_u     = [ math.exp( u[0] ), math.exp( u[1] ) ]
#
# exp_2u
exp_2u    = [ math.exp( 2.0 * u[0] ), math.exp( 2.0 * u[1] ) ]
#
# s_sq
s_sq      = standard_dev * standard_dev
#
# y
y         = [ y_0, y_1 ]
#
# tolerance
tolerance = 1e-12
# -----------------------------------------------------------------------
# check optimal random effects u
for i in range( len(u) ) :
	f_u_i = ( theta_sq * exp_2u[i] - theta * exp_u[i] * y[i] + u[i] ) / s_sq
	assert abs( f_u_i ) < tolerance
# -----------------------------------------------------------------------
print('mathematical.py: OK')
# END PYTHON
