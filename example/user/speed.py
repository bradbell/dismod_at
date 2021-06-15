# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_speed.py$$ $newlinech #$$
# $spell
#	py
#	init
#	mulcov
#	covariate
#	cv
# $$
#
# $section A Simulate Data Speed Test$$
#
# $head Syntax$$
# $icode%python3% example/user/speed.py \
#	%random_seed% %n_children% %quasi_fixed% %ode_step_size%$$
#
# $head python3$$
# This is the $cref/python3_executable/run_cmake.sh/python3_executable/$$
# on your system.
#
# $head random_seed$$
# is a non-negative integer specifying
# the $cref/random_seed/option_table/random_seed/$$ used during the simulation.
#
# $head n_children$$
# is a non-negative positive integer specifying the number of
# $cref/children/option_table/Parent Node/Children/$$.
#
# $head quasi_fixed$$
# This argument is $code true$$ or $code false$$ and specifies
# $cref/quasi_fixed/option_table/Fixed Only/quasi_fixed/$$
# in the option table.
#
# $head ode_step_size$$
# This argument is a floating point value and specifies the
# $cref/
#	ode_step_size/
#	option_table/
#	Age Average Grid/
#	ode_step_size
# /$$
# in the option table.
# The total work for the test increases with proportional to the
# square of this step size
#
# $head Fixed Effects$$
# $srccode%py%
iota_parent_true          = 0.05
rho_parent_true           = 0.2
mulcov_income_iota_true   = 1.0
mulcov_sex_rho_true       = -1.0
# %$$
#
# $subhead iota_parent_true$$
# the value of $cref/iota/rate_table/rate_name/iota/$$ corresponding
# to the parent node.
#
# $subhead rho_parent_true$$
# the value of $cref/rho/rate_table/rate_name/rho/$$ corresponding
# to the parent node.
#
# $head mulcov_income_iota_true$$
# value of the multiplier for the income covariate that affects $icode iota$$.
#
# $head mulcov_sex_rho_true$$
# value of the multiplier for the sex covariate that affects $icode rho$$.
#
# $head eta$$
# value of the offset $cref/eta/statistic/Notation/eta/$$ in the
# log transformation:
# $srccode%py%
eta = 1e-6
# %$$
#
# $head measure_cv$$
# the coefficient of variation for the simulated measurement noise.
# If you use a larger $icode measure_cv$$ you will probably need
# a larger number of data points; see $icode n_data$$ below.
# $srccode%py%
measure_cv = 0.05
# %$$
#
# $head n_data$$
# number of simulated data values.
# $srccode%py%
n_data = 200
# %$$
#
# $head age_list$$
# This following is both the $cref age_table$$ and the age
# grid points for the parent rate smoothing of $icode iota$$ and $icode rho$$.
# The child rate smoothing has a grid point at the minimum
# and maximum age below.
# $srccode%py%
age_list = [ 0.0, 5.0, 15.0, 35.0, 50.0, 75.0, 90.0, 100.0 ]
# %$$
#
# $head time_list$$
# This following is both the $cref time_table$$ and the time
# grid points for the parent rate smoothing of $icode iota$$ and $icode rho$$.
# The child rate smoothing has a grid point at the minimum
# and maximum time below.
# $srccode%py%
time_list = [ 1990.0, 2000.0, 2010.0, 2020.0 ]
# %$$
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import time
import distutils.dir_util
import numpy
test_program = 'example/user/speed.py'
if sys.argv[0] != test_program  or len(sys.argv) != 5 :
	usage  = 'python3 ' + test_program + '\\\n'
	usage += '\trandom_seed n_children quasi_fixed ode_step_size\n'
	usage += 'where working directory is dismod_at distribution directory\n'
	usage += 'python3:          the python 3 program on your system\n'
	usage += 'random_seed:      non-negative random seed; if zero, use clock\n'
	usage += 'n_children:       positive number of child nodes\n'
	usage += 'quasi_fixed:      true or false\n'
	usage += 'ode_step_size:    floating point step in age and time\n'
	sys.exit(usage)
#
start_time       = time.time();
random_seed_arg  = sys.argv[1]
n_children       = int( sys.argv[2] )
quasi_fixed      = sys.argv[3]
ode_step_size    = sys.argv[4]
#
if quasi_fixed != 'true' and quasi_fixed != 'false' :
	msg = 'quasi_fixed = "' + quasi_fixed + '" is not true or false'
	sys.exit(msg)
#
# import dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
	sys.path.insert(0, local_dir)
import dismod_at
#
# change into the build/speed directory
distutils.dir_util.mkpath('build/example/user')
os.chdir('build/example/user')
# ------------------------------------------------------------------------
def example_db (file_name) :
	# note that the a, t values are not used for this case
	def fun_rate_child(a, t) :
		return ('prior_gauss_zero', 'prior_gauss_zero', 'prior_gauss_zero')
	def fun_iota_parent(a, t) :
		return ('prior_iota_parent', 'prior_log_gauss_0', 'prior_log_gauss_0')
	def fun_rho_parent(a, t) :
		return ('prior_rho_parent', 'prior_log_gauss_0', 'prior_log_gauss_0')
	def fun_mulcov(a, t) :
		return ('prior_mulcov', 'prior_gauss_zero', 'prior_gauss_zero')
	import copy
	import dismod_at
	import math
	# ----------------------------------------------------------------------
	# age table: uses age_list defined above
	#
	# time table: uses time_list defined above
	#
	# integrand table:
	integrand_table = [
		{ 'name':'Sincidence' },
		{ 'name':'prevalence' }
	]
	#
	# node table:
	node_table = [ { 'name':'world', 'parent':'' } ]
	for i in range(n_children) :
		name = 'child_' + str(i + 1)
		node_table.append( { 'name':name, 'parent':'world' } )
	#
	# weight table:
	weight_table = list()
	#
	# covariate table:
	covariate_table = [
		{'name':'income', 'reference':0.5},
		{'name':'sex',    'reference':0.0, 'max_difference':0.6}
	]
	#
	# mulcov table:
	# income has been scaled the same as sex so man use same smoothing
	mulcov_table = [
		{
			'covariate': 'income',
			'type':      'rate_value',
			'effected':  'iota',
			'group':     'world',
			'smooth':    'smooth_mulcov'
		},{
			'covariate': 'sex',
			'type':      'rate_value',
			'effected':  'rho',
			'group':     'world',
			'smooth':    'smooth_mulcov'
		}
	]
	# ----------------------------------------------------------------------
	# data table:
	data_table = list()
	# values that are the same for all data rows
	row = {
		'meas_value':  0.0,             # not used (will be simulated)
		'density':     'gaussian',
		'weight':      '',
		'hold_out':     False,
		'age_lower':    50.,
		'age_upper':    50.,
		'time_lower':   2000.,
		'time_upper':   2000.,
		'subgroup':     'world',
	}
	# values that change between rows:
	for data_id in range( n_data ) :
		if n_children == 0 :
			row['node'] = 'world'
		else :
			row['node']      = 'child_' + str( (data_id % n_children) + 1 )
		row['income']    = data_id / float(n_data-1)
		row['sex']       = ( data_id % 3 - 1.0 ) / 2.0
		row['integrand'] = integrand_table[ data_id % 2 ]['name']
		if row['integrand'] == 'Sincidence' :
			row['meas_std']  = measure_cv * iota_parent_true
		elif row['integrand'] == 'prevalence' :
			row['meas_std']  = measure_cv * (iota_parent_true/rho_parent_true)
		else :
			assert(False)
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_gauss_zero
			'name':     'prior_gauss_zero',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      0.01,
		},{ # prior_log_gauss_0
			'name':     'prior_log_gauss_0',
			'density':  'log_gaussian',
			'mean':     0.0,
			'std':      0.1,
			'eta':      eta
		},{ # prior_iota_parent
			'name':     'prior_iota_parent',
			'density':  'uniform',
			'lower':    0.001,
			'upper':    1.0,
			'mean':     0.1,
			'eta':      eta
		},{ # prior_iota_parent
			'name':     'prior_rho_parent',
			'density':  'uniform',
			'lower':    0.001,
			'upper':    1.0,
			'mean':     0.1,
			'eta':      eta
		},{ # prior_mulcov
			'name':     'prior_mulcov',
			'density':  'uniform',
			'lower':    -2.0,
			'upper':    +2.0,
			'mean':     0.0,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	name       = 'smooth_mulcov'
	fun        = fun_mulcov
	age_grid   = [ 0 ]
	time_grid  = [ 0 ]
	smooth_table = [ {
		'name':name, 'age_id':age_grid, 'time_id':time_grid, 'fun':fun
	} ]
	name           = 'smooth_rate_child'
	fun            = fun_rate_child
	if len(age_list) > 1 :
		age_grid = [ 0 ,  len(age_list)-1 ]
	if len(time_list) > 1 :
		time_grid = [ 0 , len(time_list)-1 ]
	smooth_table.append( {
		'name':name, 'age_id':age_grid, 'time_id':time_grid, 'fun':fun
	} )
	name      = 'smooth_iota_parent'
	fun       = fun_iota_parent
	age_grid  = list( range( len(age_list) ) )
	time_grid = list( range( len(time_list) ) )
	smooth_table.append( {
		'name':name, 'age_id':age_grid, 'time_id':time_grid, 'fun':fun
	} )
	name = 'smooth_rho_parent'
	fun  = fun_rho_parent
	smooth_table.append( {
		'name':name, 'age_id':age_grid, 'time_id':time_grid, 'fun':fun
	} )
	# no standard deviation multipliers
	for dictionary in smooth_table :
		for name in [ 'value' , 'dage', 'dtime' ] :
			key   = 'mulstd_' + name + '_prior_name'
			value = None
			dictionary[key] = value
	# ----------------------------------------------------------------------
	# rate table:
	rate_table = [
		{	'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
			'child_smooth':  'smooth_rate_child',
		},{	'name':          'rho',
			'parent_smooth': 'smooth_rho_parent',
			'child_smooth':  'smooth_rate_child',
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'rate_case',              'value':'iota_pos_rho_pos' },
		{ 'name':'parent_node_name',       'value':'world'            },
		{ 'name':'ode_step_size',          'value':ode_step_size      },
		{ 'name':'random_seed',            'value':random_seed_arg    },

		{ 'name':'quasi_fixed',            'value':quasi_fixed        },
		{ 'name':'derivative_test_fixed',  'value':'none'             },
		{ 'name':'max_num_iter_fixed',     'value':'100'              },
		{ 'name':'print_level_fixed',      'value':'5'                },
		{ 'name':'tolerance_fixed',        'value':'1e-7'             },
		{ 'name':'accept_after_max_steps_fixed',     'value':'10'     },
		{ 'name':'limited_memory_max_history_fixed', 'value':'30'     },

		{ 'name':'derivative_test_random', 'value':'none'             },
		{ 'name':'max_num_iter_random',    'value':'100'              },
		{ 'name':'print_level_random',     'value':'0'                },
		{ 'name':'tolerance_random',       'value':'1e-8'             }
	]
	# ----------------------------------------------------------------------
	# avgint table: empty
	avgint_table = list()
	# ----------------------------------------------------------------------
	# nslist_table:
	nslist_table = dict()
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
# Run the init command to create the var table
file_name  = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
# -----------------------------------------------------------------------
# read database
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table        = dismod_at.get_table_dict(connection, 'var')
rate_table     = dismod_at.get_table_dict(connection, 'rate')
covariate_table= dismod_at.get_table_dict(connection, 'covariate')
# -----------------------------------------------------------------------
# truth table:
tbl_name     = 'truth_var'
col_name     = [ 'truth_var_value' ]
col_type     = [ 'real' ]
row_list     = list()
var_id2true  = list()
for var_id in range( len(var_table) ) :
	var_info        = var_table[var_id]
	truth_var_value = None
	var_type = var_info['var_type']
	if var_type == 'mulcov_rate_value' :
		rate_id   = var_info['rate_id']
		rate_name = rate_table[rate_id]['rate_name']
		if rate_name == 'iota' :
			covariate_id   = var_info['covariate_id']
			covariate_name = covariate_table[covariate_id]['covariate_name' ]
			assert( covariate_name == 'income' )
			truth_var_value = mulcov_income_iota_true
		else :
			assert( rate_name == 'rho' )
			covariate_id   = var_info['covariate_id']
			covariate_name = covariate_table[covariate_id]['covariate_name' ]
			assert( covariate_name == 'sex' )
			truth_var_value = mulcov_sex_rho_true
	else :
		assert( var_type == 'rate' )
		rate_id   = var_info['rate_id']
		rate_name = rate_table[rate_id]['rate_name']
		node_id   = var_info['node_id']
		# node zero is the world
		if node_id == 0 and rate_name == 'iota' :
			truth_var_value = iota_parent_true
		elif node_id == 0 and rate_name == 'rho' :
			truth_var_value = rho_parent_true
		else :
			truth_var_value = 0.0
	var_id2true.append( truth_var_value )
	row_list.append( [ truth_var_value ] )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
connection.close()
# -----------------------------------------------------------------------
# simulate_command
dismod_at.system_command_prc(
	[ program, file_name, 'simulate', '1' ]
)
# fit_command
dismod_at.system_command_prc(
	[ program, file_name, 'fit', 'both', '0' ]
)
# sample_command
dismod_at.system_command_prc(
	[ program, file_name, 'sample', 'asymptotic', 'both', '100', '0' ]
)
# -----------------------------------------------------------------------
# result tables
new           = False
connection    = dismod_at.create_connection(file_name, new)
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
log_dict      = dismod_at.get_table_dict(connection, 'log')
sample_table  = dismod_at.get_table_dict(connection, 'sample')
# -----------------------------------------------------------------------
# determine random seed the was used
random_seed = int(random_seed_arg)
if random_seed == 0 :
	for row in log_dict :
		if row['message_type'] == 'command' :
			message = row['message'].split()
			if message[0] == 'begin' and message[1] == 'simulate' :
				random_seed = int(row['unix_time'])
	assert random_seed != 0
# -----------------------------------------------------------------------
# sample_mean, sample_std
n_var    = len(var_table)
n_sample = int( len(sample_table) / n_var )
assert len(sample_table) == n_sample * n_var
sample_array    = numpy.zeros( (n_sample, n_var) , dtype=float )
for sample_id in range( len(sample_table) ) :
	sample_index     = int( sample_id / n_var )
	var_id           = sample_id % n_var
	assert sample_id == sample_index * n_var + var_id
	var_value        = sample_table[sample_id]['var_value']
	sample_array[sample_index, var_id] = var_value
sample_mean = numpy.mean(sample_array, axis=0)
sample_std  = numpy.std(sample_array, axis=0, ddof=1)
#
# -----------------------------------------------------------------------
# check fit, sample_mean, and sample_std
assert( len(fit_var_table) == n_var )
max_error       = 0.0
for var_id in range( n_var ) :
	row          = fit_var_table[var_id]
	fit_value    = row['fit_var_value']
	true_value   = var_id2true[var_id]
	if true_value == 0.0 :
		max_error = max(abs(fit_value), max_error)
	else :
		max_error = max( abs(fit_value / true_value - 1.0), max_error)
	mean_value   = sample_mean[var_id]
	std_value    = sample_std[var_id]
	max_error = max(abs(mean_value - fit_value), max_error)
	max_error = max(std_value, max_error)
print('random_seed      = ', random_seed)
print('n_children       = ', n_children)
print('quasi_fixed      = ', quasi_fixed)
print('ode_step_size    = ', ode_step_size)
print('elapsed seconds  =', time.time() - start_time)
print('max_error        = ', max_error)
if max_error > 5e-2 :
	print('simulated.py: Error')
	assert(False)
# -----------------------------------------------------------------------------
print('speed.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
