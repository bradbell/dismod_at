# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-19 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_sample_sim.py$$ $newlinech #$$
# $spell
#	dismod
#	Sincidence
#	mcmc
#	exp
# $$
#
# $section Sample Posterior Using Simulated Data$$
#
# $head Purpose$$
# The command
# $codei%
#	dismod_at %database% sample simulate %number_sample%
# %$$
# samples form the posterior distribution of the $cref model_variables$$
# using simulated data.
#
# $head Node Table$$
# For this example, the $cref node_table$$ is
# $pre
#          north_america
#          /          \
#       mexico      canada
# $$
#
# $head Model Variables$$
# All of the rates are constant w.r.t. age an time in this example.
#
# $subhead iota$$
# We use $latex \iota_n$$ to denote the incidence rate for
# the parent region north_america which is a
# $cref/fixed effect/model_variables/Fixed Effects, theta/$$.
#
# $subhead u$$
# We use $latex u_m$$ ($latex u_c$$) to denote the
# $cref/child rate effect
#	/model_variables
#	/Random Effects, u
#	/Child Rate Effects
# /$$
# for mexico (canada).
#
# $subhead x$$
# The model variables for this example are grouped into a single vector
# $latex x \in \B{R}^3$$ where
# $latex x_0 = \iota_n$$,
# $latex x_1 = u_m$$, and
# $latex x_2 = u_c$$.
#
# $head Model Parameters$$
#
# $subhead y$$
# We use $latex y_n, y_m, y_c$$ to denote the measured
# $cref/Sincidence/avg_integrand/Integrand, I_i(a,t)/Sincidence/$$
# for north_america, mexico, and canada respectively.
#
# $subhead s$$
# We use $latex s_n, s_m, s_c$$ to denote the
# standard deviation of the measured noise
# for north_america, mexico, and canada respectively.
#
# $subhead delta$$
# We use $latex \delta$$ to denote the standard deviation of
# the child rate effects.
# (The mean for the child rate effects is zero.)
#
# $subhead Values$$
# $srccode%py%
import math
y_n    = 1e-2
y_m    = 2.0 * y_n
y_c    = y_n / 2.0
s_n    = y_n / 10.0
s_m    = y_m / 10.0
s_c    = y_c / 10.0
delta  = math.log( 2.0 )
# %$$
#
# $head Likelihood$$
# We define $latex h(y, \mu , \sigma)$$
# to be the log-density for a $latex \B{N}( \mu, \sigma^2 )$$ distribution;
# i.e.,
# $latex \[
#	h(y, \mu, \sigma) =
#		- \frac{ ( y - \mu )^2 }{ \sigma^2 }
#		- \log \left( \sigma \sqrt{ 2 \pi } \right)
# \] $$
# The total log-likelihood for this example is
# $latex \[
#	h( y_n, \iota_n, s_n ] +
#	h[ y_m, \exp( u_m ) \iota_n, s_m ] +
#	h[ y_c, \exp( u_c ) \iota_n, s_c ] +
#	h( u_m, 0, \delta ) + h( u_c , 0 , \delta )
# \] $$
#
# $head number_sample$$
# This is the number of samples in the
# $cref/number_sample/sample_command/number_sample/$$
# specified by the sample command.
# $srccode%py%
number_sample = 30
# %$$
#
# $head number_metropolis$$
# This is the number of mcmc samples generated by the
# $cref metropolis$$ mcmc method.
# These samples are used to check the simulate method.
# $srccode%py%
number_metropolis = 500 * number_sample
# %$$
#
# $head Truth Table$$
# For the data simulation, we use the following model variable values:
# $srccode%py%
iota_n_true = y_n
u_m_true    = math.log( y_m / y_n )
u_c_true    = math.log( y_c / y_n )
# %$$
# It follows that $icode%y_m% = exp(%u_m_true%) * %iota_n_true%$$ and
# $icode%y_c% = exp(%u_c_true%) * %iota_n_true%$$.
#
# $head Source Code$$
# $srcfile%
#	example/user/sample_sim.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import time
import numpy
import sys
import os
import copy
import distutils.dir_util
import subprocess
test_program = 'example/user/sample_sim.py'
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
# ---------------------------------------------------------------------------
random_seed = int( time.time() )
# ---------------------------------------------------------------------------
# run a system command
def system_command(command) :
	print( ' '.join(command) )
	flag = subprocess.call( command )
	if flag != 0 :
		sys.exit('command failed: flag = ' + str(flag))
	return
#
# no need to include sqrt{2 \pi} term (it does not depend on model variables)
def h(y, mu, sigma ) :
	if sigma <= 0.0 :
		return - float("inf")
	res = (y - mu ) / sigma
	return - res * res - math.log( sigma )
#
def log_f(x) :
	iota_n = x[0]
	u_m    = x[1]
	u_c    = x[2]
	#
	ret    = h(y_n, iota_n, s_n)
	ret   += h(y_m, math.exp(u_m) * iota_n, s_m )
	ret   += h(y_c, math.exp(u_c) * iota_n, s_c )
	ret   += h(u_m, 0.0, delta) + h(u_c, 0.0, delta)
	return ret
# ---------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def example_db (file_name) :
	def fun_iota_child(a, t) :
		return ('prior_iota_child', None, None)
	def fun_iota_parent(a, t) :
		return ('prior_iota_parent', None, None)
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
	# node table: north_america -> (mexico, canada)
	node_table = [
		{ 'name':'north_america', 'parent':''              },
		{ 'name':'mexico',        'parent':'north_america' },
		{ 'name':'canada',        'parent':'north_america' }
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
		'node':        'north_america',
		'density':     'gaussian',
		'weight':      '',
		'hold_out':     False,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    50.0,
		'age_upper':    50.0,
		'integrand':   'Sincidence',
		'meas_value':   y_n,
		'meas_std':     s_n,
	}
	data_table.append( copy.copy(row) )
	#
	row['node']       = 'mexico';
	row['meas_value'] = y_m
	row['meas_std']   = s_m
	data_table.append( copy.copy(row) )
	#
	row['node']       = 'canada';
	row['meas_value'] = y_c
	row['meas_std']   = s_c
	data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_iota_parent
			'name':     'prior_iota_parent',
			'density':  'uniform',
			'lower':    y_n / 100.0 ,
			'upper':    y_n * 100.0 ,
			'mean':     y_n
		},{ # prior_iota_child
			'name':     'prior_iota_child',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      delta,
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
		}, { # smooth_iota_child
			'name':                     'smooth_iota_child',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'fun':                      fun_iota_child
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
			'child_smooth':  'smooth_iota_child',
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'north_america'      },
		{ 'name':'ode_step_size',          'value':'10.0'               },
		{ 'name':'random_seed',            'value':str(random_seed)     },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero'  },

		{ 'name':'quasi_fixed',            'value':'true'         },
		{ 'name':'derivative_test_fixed',  'value':'first-order'  },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-12'        },

		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-12'        },
		{ 'name':'zero_sum_random',        'value':'iota'         },
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
# ===========================================================================
file_name  = 'example.db'
example_db(file_name)
#
program   = '../../devel/dismod_at'
na_string = str(number_sample)
system_command([ program, file_name, 'init' ])
#
# -----------------------------------------------------------------------
# create truth_table
# -----------------------------------------------------------------------
# truth table:
new         = False
connection  = dismod_at.create_connection(file_name, new)
var_table   = dismod_at.get_table_dict(connection, 'var')
rate_table  = dismod_at.get_table_dict(connection, 'rate')
node_table  = dismod_at.get_table_dict(connection, 'node')
#
tbl_name         = 'truth_var'
col_name         = [ 'truth_var_value' ]
col_type         = [ 'real' ]
row_list         = list()
node_name2var_id = dict()
for var_id in range( len(var_table) ) :
	var_info  = var_table[var_id]
	rate_id   = var_info['rate_id']
	node_id   = var_info['node_id']
	rate_name = rate_table[rate_id]['rate_name']
	node_name = node_table[node_id]['node_name']
	assert( rate_name == 'iota' )
	#
	if node_name == 'north_america' :
		truth_var_value = iota_n_true
	elif node_name == 'mexico' :
		truth_var_value = u_m_true
	else :
		assert node_name == 'canada'
		truth_var_value = u_c_true
	#
	row_list.append( [ truth_var_value ] )
	node_name2var_id[node_name] = var_id
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
connection.close()
# -----------------------------------------------------------------------
# sample simulate results
#
ns_string = str(number_sample)
system_command([ program, file_name, 'simulate', ns_string ])
system_command([ program, file_name, 'sample', 'simulate', ns_string ])
#
new          = False
connection   = dismod_at.create_connection(file_name, new)
sample_table = dismod_at.get_table_dict(connection, 'sample')
#
# convert samples to a numpy array
sample_array = numpy.zeros( (number_sample, 3), dtype = float )
for row in sample_table :
	var_id                              = row['var_id']
	assert( var_id < 3 )
	sample_index                        = row['sample_index']
	sample_array[sample_index, var_id ] = row['var_value']
#
# compute statistics
sim_avg = numpy.average(sample_array, axis=0);
sim_std = numpy.std(sample_array, axis=0);
# -----------------------------------------------------------------------
# MCMC results
print('mcmc')
numpy.random.seed( seed = random_seed )
x_start    = numpy.array( [ y_n, 0.0, 0.0 ] )
scale      = numpy.array( [ y_n, delta, delta] ) * 0.2
(a, c)     = dismod_at.metropolis(log_f, number_metropolis, x_start, scale)
burn_in    = int( 0.1 * number_metropolis )
c          = c[burn_in :, :]
mcmc_avg   = numpy.average(c, axis=0)
mcmc_std   = numpy.std(c, axis=0)
mcmc_order = [ 'north_america', 'mexico', 'canada' ]
# -----------------------------------------------------------------------
# now compare values
for i in range(3) :
	# node that this model variable corresponds to
	node_name = mcmc_order[i]
	var_id    = node_name2var_id[node_name]
	sim       = sim_avg[var_id]
	mcmc      = mcmc_avg[i]
	err       = sim / mcmc - 1.0
	if abs(err) > 0.05 :
		print(node_name, '_avg (sim, mcmc, err) = ', sim, mcmc, err)
		assert(False)
	sim       = sim_std[var_id]
	mcmc      = mcmc_std[i]
	err       = sim / mcmc - 1.0
	if abs(err) > 0.5 :
		print(node_name, '_std (sim, mcmc, err) = ', sim, mcmc, err)
		assert(False)
# ----------------------------------------------------------------------------
print('sample_sim.py: OK')
# END PYTHON