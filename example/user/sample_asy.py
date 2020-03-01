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
# $begin user_sample_asy.py$$ $newlinech #$$
# $spell
#	init
#	dismod
# $$
#
# $section Sample from Asymptotic Distribution for Model Variables$$
#
# $head Purpose$$
# The command
# $codei%
#	dismod_at %database% sample asymptotic
# %$$
# samples form an asymptotic approximation for the posterior distribution
# of the $cref model_variables$$.
#
# $head Notation$$
# $table
# $icode iota_n$$ $cnext model incidence for $code north_america$$ $rnext
# $icode u_m$$ $cnext incidence random effect for $code mexico$$ $rnext
# $icode u_c$$ $cnext incidence random effect for $code canada$$ $rnext
# $icode y_n$$ $cnext measured incidence for $code north_america$$ $rnext
# $icode y_m$$ $cnext measured incidence for $code mexico$$ $rnext
# $icode y_c$$ $cnext measured incidence for $code canada$$ $rnext
# $icode s_n$$ $cnext standard deviation for $icode y_n$$   $rnext
# $icode s_m$$ $cnext standard deviation for $icode y_m$$   $rnext
# $icode s_c$$ $cnext standard deviation for $icode y_c$$   $rnext
# $icode s_r$$ $cnext standard deviation for random effects $rnext
# $tend
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
#	h[ y_n, \iota_n, s_n ] +
#	h[ y_m, \exp( u_m ) \iota_m, s_m ] +
#	h[ y_c, \exp( u_c ) \iota_c, s_c ] +
#	h( u_m, 0, s_r ) + h( u_c , 0 , s_r )
# \] $$
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
measure = {
	'north_america' : 1.0e-2 ,
	'mexico'        : 2.0e-2 ,
	'canada'        : 0.5e-2
}
standard = {
	'north_america' : 1.0e-3 ,
	'mexico'        : 2.0e-3 ,
	'canada'        : 0.5e-3
}
standard_random_effect = 1.0
number_sample          = 500
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import time
import math
import numpy
import sys
import os
import copy
import distutils.dir_util
test_program = 'example/user/sample_asy.py'
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
	y_n    = measure['north_america']
	y_m    = measure['mexico']
	y_c    = measure['canada']
	#
	s_n    = standard['north_america']
	s_m    = standard['mexico']
	s_c    = standard['canada']
	s_r    = standard_random_effect
	#
	ret    = h(y_n, iota_n, s_n)
	ret   += h(y_m, math.exp(u_m) * iota_n, s_m )
	ret   += h(y_c, math.exp(u_c) * iota_n, s_c )
	ret   += h(u_m, 0.0, s_r ) + h(u_c, 0.0, s_r)
	return ret
# ---------------------------------------------------------------------------
# Note that the a, t values are not used for this example
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
	integrand_table = [
		{ 'name':'Sincidence' }
	]
	#
	# node table: world -> north_america
	#             north_america -> (mexico, canada)
	node_table = [
		{ 'name':'world',         'parent':''              },
		{ 'name':'north_america', 'parent':'world'         },
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
		'subgroup':    'world',
		'density':     'gaussian',
		'weight':      '',
		'hold_out':     False,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    0.0,
		'age_upper':    100.0,
		'integrand':   'Sincidence',
		'meas_value':   measure['north_america'],
		'meas_std':     standard['north_america'],
	}
	data_table.append( copy.copy(row) )
	#
	row['node']       = 'mexico';
	row['meas_value'] = measure['mexico']
	row['meas_std']   = standard['mexico']
	data_table.append( copy.copy(row) )
	#
	row['node']       = 'canada';
	row['meas_value'] = measure['canada']
	row['meas_std']   = standard['canada']
	data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    1e-2 * measure['north_america'],
			'upper':    1e+2 * measure['north_america'],
			'mean':     measure['north_america'],
		},{ # prior_gauss_zero
			'name':     'prior_gauss_zero',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      standard_random_effect,
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
		{ 'name':'parent_node_name',       'value':'north_america'      },
		{ 'name':'ode_step_size',          'value':'10.0'               },
		{ 'name':'random_seed',            'value':str(random_seed)     },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero'  },

		{ 'name':'quasi_fixed',            'value':'true'         },
		{ 'name':'derivative_test_fixed',  'value':'first-order'  },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-12'        },

		{ 'name':'derivative_test_random', 'value':'second-order' },
		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-12'        }
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
ns_string = str(number_sample)
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
dismod_at.system_command_prc([ program, file_name, 'sample', 'asymptotic', ns_string ])
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# get variable and fit_var tables
var_table     = dismod_at.get_table_dict(connection, 'var')
node_table  = dismod_at.get_table_dict(connection, 'node')
rate_table  = dismod_at.get_table_dict(connection, 'rate')
sample_dict  = dismod_at.get_table_dict(connection, 'sample')
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
#
# convert samples to a numpy array
sample_array = numpy.zeros( (number_sample, 3), dtype = float )
for row in sample_dict :
	var_id                              = row['var_id']
	sample_index                        = row['sample_index']
	sample_array[sample_index, var_id ] = row['var_value']
#
# compute statistics
var_avg = numpy.average(sample_array, axis=0);
var_std = numpy.std(sample_array, axis=0);
# -----------------------------------------------------------------------
# now use MCMC to calculate the same values
numpy.random.seed( seed = random_seed )
m          = 10 * number_sample
x0         = numpy.array( [ 1e-2, 0.0, 0.0 ] )
s          = numpy.array( [ 1e-3, 1e-1, 1e-1] )
(a, c)     = dismod_at.metropolis(log_f, m, x0, s)
burn_in    = int( 0.1 * m )
c          = c[burn_in :, :]
x_avg_mcmc = numpy.average(c, axis=0)
x_std_mcmc = numpy.std(c, axis=0)
mcmc_order = [ 'north_america', 'mexico', 'canada' ]
# -----------------------------------------------------------------------
# now check values
for i in range(3) :
	node_name = mcmc_order[i]
	value     = var_avg[ node_name2var_id[node_name] ]
	check     = x_avg_mcmc[i]
	avg_diff  = check / value - 1.0
	if abs( avg_diff ) > 0.05 :
		print('avg_diff = ', avg_diff, ', random_seed = ', random_seed )
		assert(False)
	value     = var_std[ node_name2var_id[node_name] ]
	check     = x_std_mcmc[i]
	std_diff  = check / value - 1.0
	# This is a small sample case (only three data points)
	# so we do not expect the asymptotic statistics to be correct.
	# Note that in this case, the asymptotics are an over estimate.
	if std_diff >= 0.0 or abs(std_diff) > 0.5 :
		print('std_diff = ', std_diff, ', random_seed = ', random_seed )
		assert(False)
print('asymptotic.py: OK')
# END PYTHON
