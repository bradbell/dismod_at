#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-19 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_fit_random.py$$ $newlinech #$$
# $spell
#	dismod
#	exp
#	init
#	var
# $$
#
# $section Fitting Just Random Effects$$
#
# $head Purpose$$
# This example demonstrates using the command
# $codei%
#	dismod_at %database% fit random
# %$$
# See the $cref/random/fit_command/variables/random/$$ option
# to fit the fit command.
#
# $head Discussion$$
# The following describes the model and data for this example:
#
# $list number$$
# The $cref age_table$$ has values
# $code 0.0$$, $code 50.0$$, $code 100.0$$.
# The $cref time_table$$ has values
# $code 1995.0$$, $code 2005.0$$, $code 2015.0$$.
# $lnext
# The parent node is North America, the child nodes are
# Canada and the United States.
# $lnext
# The only $cref model_variables$$ in this example are
# $cref/iota/rate_table/rate_name/iota/$$ for the parent and the
# corresponding random effects for two children.
# These rates are modeled as constant with respect to age and
# linear between time 1995 and 2015.
# The true iota is
# $table
# 0.01             $pre  $$ $cnext North America $rnext
# 0.01 * exp(+0.5) $pre  $$ $cnext United States $rnext
# 0.01 * exp(-0.5) $pre  $$ $cnext Canada
# $tend
# Note that the random effect for the United States is +0.5
# and for Canada it is -0.5.
# $lnext
# There are three measurements, one for each node.
# All the measurements are at age 50 and time 2000
# (there is no age or time interval for the data points).
# The measurement value is exactly equal to the true value of $icode iota$$
# for the corresponding node.
# The measurement noise is modeled to have a 10 percent coefficient
# of variation (even though there is no noise in the actual measurements).
# $lnext
# The prior for North America is a uniform with mean equal to the
# true value for the United States.
# The prior for Canada and the United States is a Gaussian with mean zero and
# standard deviation 100.
# The large standard deviation is so that it does not have much effect.
# $lnext
# The prior for the difference in $icode iota$$ between time 1995
# and time 2015 for the children (parent) is a Gaussian (log Gaussian)
#  with mean zero and standard deviation 0.1.
# $lnext
# The init command is used to set the
# $cref/start_var_table/init_command/start_var_table/$$ equal to the
# prior mean.
# The prior means for North America (the fixed effects) are not their
# true values and the optimal values for the random effects
# compensate for this.
# $lend
#
#
# $head Source Code$$
# $srcfile%example/user/fit_random.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
#
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
iota_parent_true            = 1e-2
united_states_random_effect = +0.5
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import copy
from math import exp
test_program = 'example/user/fit_random.py'
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
	def fun_rate_child(a, t) :
		return ('prior_rate_child',  None, 'prior_child_diff')
	def fun_rate_parent(a, t) :
		return ('prior_rate_parent', None, 'prior_parent_diff')
	import dismod_at
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
		{ 'name':'north_america', 'parent':'world' },
		{ 'name':'united_states', 'parent':'north_america' },
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
	# write out data
	row = {
		'density':     'gaussian',
		'weight':      '',
		'hold_out':     False,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    50.0,
		'age_upper':    50.0,
		'integrand':    'Sincidence',
	}
	row['node']        = 'north_america'
	row['meas_value']  = iota_parent_true
	row['meas_std']    = row['meas_value'] * 1e-1
	data_table.append( copy.copy(row) )
	#
	row['node']        = 'united_states'
	row['meas_value']  = iota_parent_true * exp( united_states_random_effect )
	row['meas_std']    = row['meas_value'] * 1e-1
	data_table.append( copy.copy(row) )
	#
	row['node']        = 'canada'
	row['meas_value']  = iota_parent_true * exp(- united_states_random_effect )
	row['meas_std']    = row['meas_value'] * 1e-1
	data_table.append( copy.copy(row) )
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    iota_parent_true / 100.,
			'upper':    iota_parent_true * 100.,
			# set prior so north_amaerica is set to value for united_states
			'mean':     iota_parent_true * exp( united_states_random_effect )
		},{ # prior_rate_child
			'name':     'prior_rate_child',
			'density':  'gaussian',
			'mean':     0.0,
			# std is very large so like a uniform distribution
			'std':      100.0,
		},{ # prior_parent_diff
			'name':     'prior_parent_diff',
			'density':  'log_gaussian',
			'mean':     0.0,
			'std':      0.1,
			'eta':      1e-8
		},{ # prior_child_diff
			'name':     'prior_child_diff',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      0.1,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	last_time_id   = 2
	smooth_table = [
		{ # smooth_rate_child
			'name':                     'smooth_rate_child',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0, last_time_id ],
			'fun':                      fun_rate_child
		},{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0, last_time_id ],
			'fun':                      fun_rate_parent
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{	'name':          'iota',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child',
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'north_america' },

		{ 'name':'derivative_test_random', 'value':'second-order'  },
		{ 'name':'max_num_iter_random',    'value':'100'           },
		{ 'name':'tolerance_random',       'value':'1e-10'         },
		{ 'name':'method_random',          'value':'ipopt_solve'   }
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
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'random' ])
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# check the zero random effects solution
#
# get variable and fit_var tables
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
rate_table    = dismod_at.get_table_dict(connection, 'rate')
node_table    = dismod_at.get_table_dict(connection, 'node')
#
# one age and two times for each of north_america, canada, unites_states
n_var = len(var_table)
assert n_var == 6
#
# check of values uses the fact that the data density is Gaussian
for var_id in range( n_var ) :
	var_type = var_table[var_id]['var_type']
	assert( var_type == 'rate' )
	#
	rate_id = var_table[var_id]['rate_id']
	assert( rate_table[rate_id]['rate_name'] == 'iota' )
	#
	value   = fit_var_table[var_id]['fit_var_value']
	#
	node_id  = var_table[var_id]['node_id']
	if node_table[node_id]['node_name'] == 'north_america' :
		# not fitting parent so check that north_america has value in prior
		check  = iota_parent_true * exp( united_states_random_effect )
		relerr = value / check - 1.0
		assert abs(relerr) < 1e-10
	elif node_table[node_id]['node_name'] == 'canada' :
		# fitting canada so check its random effects
		# compensate for error in paraent value.
		check  = - 2 * united_states_random_effect
		relerr = value / check - 1.0
		assert abs(relerr) < 1e-5
	else :
		assert node_table[node_id]['node_name'] == 'united_states'
		# fitting unites_states so check its random effects
		# compensate for error in paraent value.
		check  = 0
		relerr = ( value - check ) / united_states_random_effect
		assert abs(relerr) < 1e-5
# -----------------------------------------------------------------------
print('fit_random: OK')
# END PYTHON
