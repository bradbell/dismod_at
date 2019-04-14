#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_lasso_covariate.py$$ $newlinech #$$
# $spell
#	init
#	avgint
#	Covariates
#	covariate
# $$
#
# $section Using Lasso on Covariate Multiplier$$
#
# $head See Also$$
# $cref user_meas_covariate.py$$
#
# $srcfile%
#	example/user/lasso_covariate.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# problem parameters that can be changed
import math
mulcov_income    = 1.0
iota_reference   = 0.05
n_data           = 301
laplace_std      = 0.1 / math.sqrt(n_data)
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
import random
test_program = 'example/user/lasso_covariate.py'
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
def constant_weight_fun(a, t) :
	return 1.0
def fun_iota_parent(a, t) :
	return ('prior_value_parent', None, None)
def fun_income(a, t) :
	return ('prior_income', None, None)
def fun_sex(a, t) :
	return ('prior_sex', None, None)
def fun_uniform(a, t) :
	return ('prior_uniform', None, None)
# ------------------------------------------------------------------------
def example_db (file_name) :
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
	]
	#
	# weight table: The constant function 1.0 (one age and one time point)
	fun = constant_weight_fun
	weight_table = [
		{ 'name':'constant',  'age_id':[1], 'time_id':[1], 'fun':fun }
	]
	#
	# covariate table:
	covariate_table = [
		{'name':'income', 'reference':0.5},
		{'name':'sex',    'reference':0.0, 'max_difference':0.6}
	]
	#
	# mulcov table
	# income has been scaled the same as sex so man use same smoothing
	mulcov_table = [
		{
			'covariate': 'income',
			'type':      'rate_value',
			'effected':  'iota',
			'smooth':    'smooth_income'
		},{
			'covariate': 'sex',
			'type':      'rate_value',
			'effected':  'iota',
			'smooth':    'smooth_sex'
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
		'node':        'world',
		'integrand':   'Sincidence',
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   1995.0,
		'time_upper':   1995.0,
		'age_lower':    0.0,
		'age_upper':    0.0
	}
	# values that change between rows:
	income_reference = 0.5
	for data_id in range( n_data ) :
		income         = data_id / float(n_data-1)
		sex            = ( data_id % 3 - 1.0 ) / 2.0
		avg_integrand  = iota_reference
		avg_integrand *= math.exp( (income - income_reference) * mulcov_income )
		meas_value     = random.gauss(avg_integrand, 1e-1 * avg_integrand)
		meas_std       = 0.1 * meas_value
		row['meas_value'] = meas_value
		row['meas_std']   = meas_std
		row['income']     = income
		row['sex']        = sex
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_uniform
			'name':     'prior_uniform',
			'density':  'uniform',
			'mean':     0.0,
		},{ # prior_value_parent
			'name':     'prior_value_parent',
			'density':  'uniform',
			'lower':    0.01,
			'upper':    1.00,
			'mean':     0.1,
		},{ # prior_income
			'name':     'prior_income',
			'density':  'laplace',
			'mean':     0.0,
			'std':      laplace_std,
		},{ # prior_sex
			'name':     'prior_sex',
			'density':  'laplace',
			'mean':     0.0,
			'std':      laplace_std,
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
		},{ # smooth_income
			'name':                     'smooth_income',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'fun':                      fun_income
		},{ # smooth_sex
			'name':                     'smooth_sex',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'fun':                      fun_sex
		},{ # smooth_uniform
			'name':                     'smooth_uniform',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'fun':                      fun_uniform
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'world'        },
		{ 'name':'random_seed',            'value':'0'            },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },

		{ 'name':'quasi_fixed',            'value':'false'        },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-13'        },

		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-13'        }
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
	n_smooth  = len( smooth_table )
	rate_true = []
	for rate_id in range( len( data_table ) ) :
		# for this particular example
		data_id    = rate_id
		meas_value = data_table[data_id]['meas_value']
		rate_true.append(meas_value)
	#
	return
# ===========================================================================
# Fit to determine nonzero covariate multipliers
file_name      = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
for command in [ 'init', 'fit' ] :
	cmd = [ program, file_name, command ]
	if command == 'fit' :
		variables = 'fixed'
		cmd.append(variables)
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
#
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
# check covariate multiplier values
nonzero_mulcov  = list()
for var_id in range( len(var_table) ) :
	row   = var_table[var_id]
	match = row['var_type'] == 'mulcov_rate_value'
	if match :
		fit_var_value  = fit_var_table[var_id]['fit_var_value']
		covariate_id   = row['covariate_id']
		#
		nonzero = abs(fit_var_value) > laplace_std
		if covariate_id == 0 :
			assert nonzero
		else :
			assert not nonzero
		nonzero_mulcov.append( nonzero )
assert len(nonzero_mulcov) == 2
# -----------------------------------------------------------------------------
# Remove laplace prior on nonzero multipliers and re-fit
prior_name = [ 'prior_income', 'prior_sex' ]
for covariate_id in range(2):
	if nonzero_mulcov[covariate_id] :
		command = 'UPDATE prior SET density_id = 0 WHERE prior_name == '
		command += '"' + prior_name[covariate_id] + '"'
		dismod_at.sql_command(connection, command)
	else :
		command = 'UPDATE prior SET lower=0.0, upper=0.0 WHERE prior_name == '
		command += '"' + prior_name[covariate_id] + '"'
		dismod_at.sql_command(connection, command)
cmd = [ program, file_name, 'fit', 'fixed' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at ' + command + ' command failed')
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
# check covariate multiplier values
for var_id in range( len(var_table) ) :
	row   = var_table[var_id]
	match = row['var_type'] == 'mulcov_rate_value'
	if match :
		fit_var_value  = fit_var_table[var_id]['fit_var_value']
		covariate_id   = row['covariate_id']
		#
		if covariate_id == 0 :
			abs( 1.0 - fit_var_value / mulcov_income ) < 0.1
		else :
			assert fit_var_value == 0.0
print('lasso_covariate.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
