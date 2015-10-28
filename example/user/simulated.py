#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_simulated.py$$ $newlinech #$$
# $spell
# $$
#
# $section A Simulate Data Example$$
#
# $code
# $verbatim%
#	example/user/simulated.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# values used to simulate data
iota_parent               = 0.05
rho_parent                = 0.2
mulcov_income_iota_true   = 1.0
mulcov_sex_rho_true       = -1.0
n_children                = 5
n_data                    = 100
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
test_program = 'example/user/simulated.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
	usage  = 'python3 ' + test_program + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
#
# import dismod_at
sys.path.append( os.getcwd() + '/python' )
import dismod_at
#
# change into the build/example/user directory
distutils.dir_util.mkpath('build/example/user')
os.chdir('build/example/user')
# ------------------------------------------------------------------------
def constant_weight_fun(a, t) :
	return 1.0
# note that the a, t values are not used for this case
def fun_zero(a, t) :
	return ('prior_zero', 'prior_none', 'prior_none')
def fun_rate_child(a, t) :
	return ('prior_gauss_zero', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_iota_parent(a, t) :
	return ('prior_iota_parent', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_rho_parent(a, t) :
	return ('prior_rho_parent', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_mulcov(a, t) :
	return ('prior_mulcov', 'prior_gauss_zero', 'prior_gauss_zero')
# ------------------------------------------------------------------------
def example_db (file_name) :
	import copy
	import dismod_at
	import math
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ 0.0, 5.0, 15.0, 35.0, 50.0, 75.0, 90.0, 100.0 ]
	#
	# time table:
	time_list   = [ 1990.0, 2000.0, 2010.0, 2200.0 ]
	#
	# integrand table:
	integrand_dict = [
		{ 'name':'Sincidence',  'eta': 1e-2 * iota_parent  },
		{ 'name':'prevalence',  'eta': 1e-3 * (iota_parent / rho_parent)  }
	]
	#
	# node table:
	node_dict = [ { 'name':'world', 'parent':'' } ]
	for i in range(n_children) :
		name = 'child_' + str(i + 1)
		node_dict.append( { 'name':name, 'parent':'world' } )
	#
	# weight table:
	# The constant function 1.0, note any valid age and time id would work
	name    = 'constant'
	fun     = constant_weight_fun
	age_id  = int( len(age_list) / 2 )
	time_id = int( len(time_list) / 2 )
	weight_dict = [
		{ 'name':name,  'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	]
	#
	# covariate table:
	covariate_dict = [
		{'name':'income', 'reference':0.5, 'max_difference':None},
		{'name':'sex',    'reference':0.0, 'max_difference':0.6}
	]
	#
	# mulcov table:
	# income has been scaled the same as sex so man use same smoothing
	mulcov_dict = [
		{
			'covariate': 'income',
			'type':      'rate_value',
			'effected':  'iota',
			'smooth':    'smooth_mulcov'
		},{
			'covariate': 'sex',
			'type':      'rate_value',
			'effected':  'rho',
			'smooth':    'smooth_mulcov'
		}
	]
	# --------------------------------------------------------------------------
	# data table:
	data_dict = list()
	# values that are the same for all data rows
	row = {
		'meas_value':  0.0,             # not used (will be simulated)
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'age_lower':    50.,
		'age_upper':    50.,
		'time_lower':   2000.,
		'time_upper':   2000.
	}
	# values that change between rows:
	for data_id in range( n_data ) :
		row['node']      = 'child_' + str( (data_id % n_children) + 1 )
		row['income']    = data_id / float(n_data-1)
		row['sex']       = ( data_id % 3 - 1.0 ) / 2.0
		row['integrand'] = integrand_dict[ data_id % 2 ]['name']
		if row['integrand'] == 'Sincidence' :
			row['meas_std']  = 0.1 * iota_parent
		elif row['integrand'] == 'prevalence' :
			row['meas_std']  = 0.1 * (iota_parent / rho_parent)
		else :
			assert(False)
		data_dict.append( copy.copy(row) )
	# --------------------------------------------------------------------------
	# prior_table
	prior_dict = [
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
		},{ # prior_iota_parent
			'name':     'prior_iota_parent',
			'density':  'uniform',
			'lower':    0.0,
			'upper':    1.0,
			'mean':     0.1,
			'std':      None,
			'eta':      None
		},{ # prior_iota_parent
			'name':     'prior_rho_parent',
			'density':  'uniform',
			'lower':    0.0,
			'upper':    1.0,
			'mean':     0.1,
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
	# --------------------------------------------------------------------------
	# smooth table
	name           = 'smooth_rate_child'
	fun            = fun_rate_child
	age_id         = int( len( age_list ) / 2 )
	time_id        = int( len( time_list ) / 2 )
	smooth_dict = [
		{'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	]
	name = 'smooth_iota_parent'
	fun  = fun_iota_parent
	smooth_dict.append(
		{'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	)
	name = 'smooth_rho_parent'
	fun  = fun_rho_parent
	smooth_dict.append(
		{'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	)
	name = 'smooth_zero'
	fun  = fun_zero
	smooth_dict.append(
		{'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	)
	name = 'smooth_mulcov'
	fun  = fun_mulcov
	smooth_dict.append(
		{'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	)
	# no standard deviation multipliers
	for dictionary in smooth_dict :
		for name in [ 'value' , 'dage', 'dtime' ] :
			key   = 'mulstd_' + name + '_prior_name'
			value = ''
			dictionary[key] = value
	# --------------------------------------------------------------------------
	# rate table:
	rate_dict = [
		{	'name':          'pini',
			'parent_smooth': 'smooth_zero',
			'child_smooth':  'smooth_rate_child'
		},{	'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
			'child_smooth':  'smooth_rate_child'
		},{	'name':          'rho',
			'parent_smooth': 'smooth_rho_parent',
			'child_smooth':  'smooth_rate_child'
		},{	'name':          'chi',
			'parent_smooth': 'smooth_zero',
			'child_smooth':  'smooth_rate_child'
		},{	'name':          'omega',
			'parent_smooth': 'smooth_zero',
			'child_smooth':  'smooth_rate_child'
		}
	]
	# ------------------------------------------------------------------------
	# option_dict
	option_dict = [
		{ 'name':'parent_node_name',       'value':'world'        },
		{ 'name':'number_sample',          'value':'1'            },
		{ 'name':'ode_step_size',          'value':'10.0'         },
		{ 'name':'random_seed',            'value':'0'            },

		{ 'name':'quasi_fixed',            'value':'true'         },
		{ 'name':'derivative_test_fixed',  'value':'first-order'  },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'5'            },
		{ 'name':'tolerance_fixed',        'value':'1e-8'         },

		{ 'name':'derivative_test_random', 'value':'second-order' },
		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-8'         }
	]
	# --------------------------------------------------------------------------
	# avgint table: empty
	avgint_dict = list()
	# --------------------------------------------------------------------------
	# create database
	dismod_at.create_database(
		file_name,
		age_list,
		time_list,
		integrand_dict,
		node_dict,
		weight_dict,
		covariate_dict,
		data_dict,
		prior_dict,
		smooth_dict,
		rate_dict,
		mulcov_dict,
		option_dict,
		avgint_dict
	)
	# -----------------------------------------------------------------------
	return
# ===========================================================================
# Run the init command to create the var table
file_name      = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
cmd            = [ program, 'init', file_name ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at init command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# read databas
var_dict        = dismod_at.get_table_dict(connection, 'var')
rate_dict       = dismod_at.get_table_dict(connection, 'rate')
covariate_dict  = dismod_at.get_table_dict(connection, 'covariate')
#
# truth table:
tbl_name     = 'truth'
col_name     = [ 'truth_var_value' ]
col_type     = [ 'real' ]
row_list     = list()
for variable in var_dict :
	truth_var_value = None
	var_type = variable['var_type']
	if var_type == 'mulcov_rate_value' :
		rate_id   = variable['rate_id']
		rate_name = rate_dict[rate_id]['rate_name']
		if rate_name == 'iota' :
			covariate_id   = variable['covariate_id']
			covariate_name = covariate_dict[covariate_id]['covariate_name' ]
			assert( covariate_name == 'income' )
			truth_var_value = mulcov_income_iota_true
		else :
			assert( rate_name == 'rho' )
			covariate_id   = variable['covariate_id']
			covariate_name = covariate_dict[covariate_id]['covariate_name' ]
			assert( covariate_name == 'sex' )
			truth_var_value = mulcov_sex_rho_true
	else :
		assert( var_type == 'rate' )
		rate_id   = variable['rate_id']
		rate_name = rate_dict[rate_id]['rate_name']
		node_id   = variable['node_id']
		# node zero is the world
		if node_id == 0 and rate_name == 'iota' :
			truth_var_value = iota_parent
		elif node_id == 0 and rate_name == 'rho' :
			truth_var_value = rho_parent
		else :
			truth_var_value = 0.0
	row_list.append( [ truth_var_value ] )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
