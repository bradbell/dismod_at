#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-19 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_fit_sim.py$$ $newlinech #$$
# $spell
#	init
#	covariate
#	mulcov
# $$
#
# $section Fitting Simulated Data Example$$
#
# $head Parent Iota$$
# The value $icode iota_parent_true$$
# is the simulated true rate for $icode iota$$
# for the parent.
# A uniform prior is used for the parent rate with
# $icode%iota_parent_true%/100%$$ as a lower limit,
# and $code 1$$ as the upper limit.
#
# $head Child Iota$$
# The $icode iota$$ $cref/child rate effects
#	/model_variables/Random Effects, u/Child Rate Effects/$$
# have a Gaussian prior with a mean zero and standard deviation 0.1.
# Note that there is only one grid point in the parent and child smoothing
# for iota, hence it is constant in age and time.
# In addition, the sum of the child rate random effects is constrained to
# be zero.
#
# $head Other Rates$$
# For this example the other rates are all zero.
# This is specified by setting the
# $cref/parent_smooth_id/rate_table/parent_smooth_id/$$ and
# $cref/child_smooth_id/rate_table/child_smooth_id/$$ to null
# for the other rates.
#
# $head Covariate Multiplier$$
# There is one covariate multiplier on income and it affects the
# value of the rate $icode iota$$ for a particular data point.
# The income covariate has been normalized so it ranges between
# zero and one.
# The prior for this multiplier is an uniform on the interval
# [-2, +2].
# The true value for this multiplier, used to simulate data, is
# called $icode mulcov_income_iota_true$$.
# Note that there is only one grid point in the covariate multiplier,
# hence it is constant in age and time.
#
# $head Data$$
# All of the data is for the prevalence integrand and has a standard
# deviation of 1e-3.
#
# $head Source Code$$
# $srcfile%
#	example/user/fit_sim.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# values used to simulate data
iota_parent_true          = 0.01
mulcov_income_iota_true   = 1.0
n_children                = 2
n_data                    = 20
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
test_program = 'example/user/fit_sim.py'
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
	def constant_weight_fun(a, t) :
		return 1.0
	# note that the a, t values are not used for this case
	def fun_rate_child(a, t) :
		return ('prior_iota_child', None, None)
	def fun_iota_parent(a, t) :
		return ('prior_iota_parent', None, None)
	def fun_mulcov(a, t) :
		return ('prior_mulcov', None, None)
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ 0.0, 5.0, 15.0, 35.0, 50.0, 75.0, 90.0, 100.0 ]
	#
	# time table:
	time_list   = [ 1990.0, 2000.0, 2010.0, 2200.0 ]
	#
	# integrand table:
	integrand_table = [
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
	# The constant function 1.0, note any valid age and time id would work
	name    = 'constant'
	fun     = constant_weight_fun
	age_id  = int( len(age_list) / 2 )
	time_id = int( len(time_list) / 2 )
	weight_table = [
		{ 'name':name,  'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	]
	#
	# covariate table:
	covariate_table = [
		{'name':'income', 'reference':0.5}
	]
	#
	# mulcov table:
	mulcov_table = [
		{
			'covariate': 'income',
			'type':      'rate_value',
			'effected':  'iota',
			'smooth':    'smooth_mulcov'
		}
	]
	#
	# avgint table: empty
	avgint_table = list()
	#
	# nslist_table:
	nslist_table = dict()
	# ----------------------------------------------------------------------
	# data table:
	data_table = list()
	# values that are the same for all data rows
	row = {
		'meas_value':  0.0,             # not used (will be simulated)
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   2000.,
		'time_upper':   2000.
	}
	# values that change between rows:
	for data_id in range( n_data ) :
		fraction         = data_id / float(n_data-1)
		age              = age_list[0] + (age_list[-1] - age_list[0])*fraction
		row['age_lower'] = age
		row['age_upper'] = age
		row['node']      = 'child_' + str( (data_id % n_children) + 1 )
		row['income']    = fraction
		row['integrand'] = integrand_table[0]['name']
		row['meas_std']  = 1e-3
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_iota_child
			'name':     'prior_iota_child',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      0.1,
		},{ # prior_iota_parent
			'name':     'prior_iota_parent',
			'density':  'uniform',
			'lower':    iota_parent_true / 100.,
			'upper':    1.0,
			'mean':     0.1,
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
	name           = 'smooth_rate_child'
	fun            = fun_rate_child
	age_id         = int( len( age_list ) / 2 )
	time_id        = int( len( time_list ) / 2 )
	smooth_table = [
		{'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	]
	name = 'smooth_iota_parent'
	fun  = fun_iota_parent
	smooth_table.append(
		{'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	)
	name = 'smooth_mulcov'
	fun  = fun_mulcov
	smooth_table.append(
		{'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	)
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
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },
		{ 'name':'parent_node_name',       'value':'world'        },
		{ 'name':'ode_step_size',          'value':'10.0'         },
		{ 'name':'random_seed',            'value':'0'            },
		{ 'name':'zero_sum_random',        'value':'iota'         },

		{ 'name':'quasi_fixed',            'value':'true'         },
		{ 'name':'derivative_test_fixed',  'value':'first-order'  },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-8'         },

		{ 'name':'derivative_test_random', 'value':'second-order' },
		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-8'         }
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
# Run the init command to create the var table
file_name      = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
cmd            = [ program, file_name, 'init' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at init command failed')
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
			assert( False )
	else :
		assert( var_type == 'rate' )
		rate_id   = var_info['rate_id']
		rate_name = rate_table[rate_id]['rate_name']
		node_id   = var_info['node_id']
		# node zero is the world
		if node_id == 0 and rate_name == 'iota' :
			truth_var_value = iota_parent_true
		else :
			truth_var_value = 0.0
	var_id2true.append( truth_var_value )
	row_list.append( [ truth_var_value ] )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
connection.close()
# -----------------------------------------------------------------------
# Simulate and then fit the data
for command in [ 'simulate', 'fit' ] :
	cmd = [ program, file_name, command ]
	if command == 'simulate' :
		number_simulate = '1'
		cmd.append(number_simulate)
	if command == 'fit' :
		variables = 'both'
		cmd.append(variables)
	if command == 'fit' :
		simulate_index = '0';
		cmd.append(simulate_index)
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# check fit results
new          = False
connection   = dismod_at.create_connection(file_name, new)
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
connection.close()
#
max_error    = 0.0
for var_id in range( len(var_table) ) :
	row        = fit_var_table[var_id]
	fit_value  = row['fit_var_value']
	true_value = var_id2true[var_id]
	if true_value == 0.0 :
		max_error = max(abs(fit_value) , max_error)
	else :
		max_error = max( abs(fit_value / true_value - 1.0), max_error)
if max_error > 5e-2 :
	print('max_error = ', max_error)
	assert(False)
# -----------------------------------------------------------------------------
print('fit_sim.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
