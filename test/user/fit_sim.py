#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Simulate a data set and fit it. In addition, using sample table
# as the source in a set command.
# Test a case where sim_std is much different from meas_std.
#
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
test_program = 'test/user/fit_sim.py'
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
# change into the build/test/user directory
distutils.dir_util.mkpath('build/test/user')
os.chdir('build/test/user')
# ----------------------------------------------------------------------------
# run a system command
def system_command(command) :
	print( ' '.join(command) )
	flag = subprocess.call( command )
	if flag != 0 :
		sys.exit('command failed: flag = ' + str(flag))
	return
# ------------------------------------------------------------------------
def constant_weight_fun(a, t) :
	return 1.0
# note that the a, t values are not used for this case
def fun_rate_child(a, t) :
	return ('prior_gauss_zero', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_iota_parent(a, t) :
	return ('prior_iota_parent', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_mulcov(a, t) :
	return ('prior_mulcov', 'prior_gauss_zero', 'prior_gauss_zero')
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ 0.0, 5.0, 15.0, 35.0, 50.0, 75.0, 90.0, 100.0 ]
	#
	# time table:
	time_list   = [ 1990.0, 2000.0, 2010.0, 2200.0 ]
	#
	# integrand table:
	integrand_table = [
		 { 'name':'prevalence', 'minimum_meas_cv':0.0 }
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
		{'name':'income', 'reference':0.5, 'max_difference':None}
	]
	#
	# mulcov table:
	mulcov_table = [
		{
			'covariate': 'income',
			'type':      'rate_value',
			'effected':  'iota',
			'group':     'world',
			'smooth':    'smooth_mulcov'
		}
	]
	# ----------------------------------------------------------------------
	# data table:
	data_table = list()
	#
	density_list = [ "gaussian", "students", "log_gaussian", "log_students" ]
	# values that are the same for all data rows
	row = {
		'meas_value':  1.,   # measurement is way off truth and not used
		'eta':         1e-6,
		'nu':          10,
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   2000.,
		'time_upper':   2000.,
		'subgroup':     'world',
	}
	# values that change between rows:
	for data_id in range( n_data ) :
		density          = density_list[ data_id % 4 ]
		row['density']   = density
		#
		# one percent coefficient of variation
		if density == 'gaussian' or density == 'students' :
			row['meas_std'] = iota_parent_true / 100.
		else :
			row['meas_std'] = row['meas_value'] / 100.
		fraction         = data_id / float(n_data-1)
		age              = age_list[0] + (age_list[-1] - age_list[0])*fraction
		row['age_lower'] = age
		row['age_upper'] = age
		row['node']      = 'child_' + str( (data_id % n_children) + 1 )
		row['income']    = fraction
		row['integrand'] = integrand_table[0]['name']
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{   # prior_zero
			'name':     'prior_zero',
			'density':  'uniform',
			'lower':    0.0,
			'upper':    0.0,
			'mean':     0.0,
			'std':      None,
			'eta':      None
		}, { # prior_none
			'name':     'prior_none',
			'density':  'uniform',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      None,
			'eta':      None
		},{ # prior_gauss_zero
			'name':     'prior_gauss_zero',
			'density':  'gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      0.01,
			'eta':      None
		},{ # prior_loggauss_zero
			'name':     'prior_loggauss_zero',
			'density':  'log_gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      0.1,
			'eta':      1e-6
		},{ # prior_iota_parent
			'name':     'prior_iota_parent',
			'density':  'uniform',
			'lower':    iota_parent_true / 100.,
			'upper':    iota_parent_true * 10.0,
			'mean':     iota_parent_true / 2.0,
			'std':      None,
			'eta':      None
		},{ # prior_mulcov
			'name':     'prior_mulcov',
			'density':  'uniform',
			'lower':    -2.0,
			'upper':    +2.0,
			'mean':     0.0,
			'std':      None,
			'eta':      None
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
		{	'name':          'pini',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		},{	'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
			'child_smooth':  'smooth_rate_child',
			'child_nslist':  None
		},{	'name':          'rho',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		},{	'name':          'chi',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		},{	'name':          'omega',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },
		{ 'name':'parent_node_name',       'value':'world'        },
		{ 'name':'ode_step_size',          'value':'10.0'         },
		{ 'name':'random_seed',            'value':'0'            },
		{ 'name':'zero_sum_child_rate',    'value':'iota'         },

		{ 'name':'quasi_fixed',            'value':'true'         },
		{ 'name':'derivative_test_fixed',  'value':'none'         },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-6'         },

		{ 'name':'derivative_test_random', 'value':'none'         },
		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-8'         },
		{ 'name':'method_random',          'value':'ipopt_random'  }
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
file_name      = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
system_command([ program, file_name, 'init' ])
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
system_command([ program, file_name, 'set', 'scale_var', 'truth_var' ])
system_command([ program, file_name, 'set', 'start_var', 'truth_var' ])
system_command([ program, file_name, 'simulate', '2' ])
system_command([ program, file_name, 'fit', 'both', '0' ])
# -----------------------------------------------------------------------
# check fit results
new          = False
connection   = dismod_at.create_connection(file_name, new)
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
#
max_error    = 0.0
for var_id in range( len(var_table) ) :
	row      = fit_var_table[var_id]
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
# set start_var so it corresponds to second set of model variables
cmd = [ program, file_name, 'sample', 'simulate', 'both', '2' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at sample command failed')
cmd = [ program, file_name, 'set', 'start_var', 'sample', '1' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at set command failed')
start_var_table = dismod_at.get_table_dict(connection, 'start_var')
sample_table    = dismod_at.get_table_dict(connection, 'sample')
sample_index    = 1
n_var           = len(var_table)
for var_id in range( n_var ) :
	start_value  = start_var_table[var_id]['start_var_value']
	sample_value = sample_table[ sample_index * n_var + var_id ]['var_value']
	start_value  = float( start_value )
	sample_value = float( sample_value )
	if start_value == 0.0 :
		assert sample_value == 0.0
	else :
		assert abs( start_value / sample_value - 1.0 )  < 1e-10
# -----------------------------------------------------------------------------
connection.close()
print('fit_sim.py: OK')
