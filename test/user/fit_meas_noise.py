#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# values used to simulate data
iota_true   = 1.0
gamma_true  = iota_true * 2.0
n_data      = 2000
data_std    = iota_true / 10.0
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
import random
import math
test_program = 'test/user/fit_meas_noise.py'
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
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this test
def weight_fun(a, t) :
	return 1.0
# note that the a, t values are not used for this case
def fun_iota(a, t) :
	return ('prior_iota', None, None)
def fun_gamma(a, t) :
	return ('prior_gamma', None, None)
# ------------------------------------------------------------------------
def example_db (file_name, meas_noise_effect) :
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ 0.0, 100.0 ]
	#
	# time table:
	time_list   = [ 1990.0, 2200.0 ]
	#
	# integrand table:
	integrand_table = [
		 { 'name': 'Sincidence' }
	]
	#
	# node table:
	node_table = [ { 'name':'world', 'parent':'' } ]
	#
	# weight table:
	# The constant function 1.0, note any valid age and time id would work
	name    = 'constant'
	fun     = weight_fun
	age_id  = 0
	time_id = 0
	weight_table = [
		{ 'name':name,  'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	]
	#
	# covariate table:
	covariate_table = [
		{'name':'one', 'reference':0.0}
	]
	#
	# mulcov table:
	mulcov_table = [
		{
			'covariate': 'one',
			'type':      'meas_noise',
			'effected':  'Sincidence',
			'group':     'world',
			'smooth':    'smooth_gamma'
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
		'time_upper':   2000.,
		'integrand':   'Sincidence',
		'meas_std':     data_std,
		'node':        'world',
		'subgroup':    'world',
		'one':          1.0
	}
	delta_dict = {
		'add_std_scale_none' : data_std + gamma_true ,
		'add_std_scale_log'  : data_std + gamma_true ,
		'add_std_scale_all'  : data_std * ( 1.0 +  gamma_true ) ,
		'add_var_scale_none' : math.sqrt( data_std * data_std + gamma_true ),
		'add_var_scale_log'  : math.sqrt( data_std * data_std + gamma_true ),
		'add_var_scale_all'  : data_std * math.sqrt( 1.0 + gamma_true ),
	}
	delta = delta_dict[meas_noise_effect]
	# values that change between rows:
	for data_id in range( n_data ) :
		row['meas_value'] = random.gauss(iota_true , delta)
		fraction          = data_id / float(n_data-1)
		age               = age_list[0] + (age_list[-1] - age_list[0])*fraction
		row['age_lower']  = age
		row['age_upper']  = age
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_iota
			'name':     'prior_iota',
			'density':  'uniform',
			'lower':    iota_true / 100.0,
			'upper':    1.0,
			'mean':     iota_true / 10.0
		},{ # prior_gamma
			'name':     'prior_gamma',
			'density':  'uniform',
			'lower':    0.0,
			'upper':    10.0 * gamma_true,
			'mean':     gamma_true / 10.0
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	name           = 'smooth_iota'
	fun            = fun_iota
	age_id         = 0
	time_id        = 0
	smooth_table = [
		{'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	]
	name = 'smooth_iota'
	#
	name = 'smooth_gamma'
	fun  = fun_gamma
	smooth_table.append(
		{'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	)
	# ----------------------------------------------------------------------
	# rate table:
	rate_table = [
		{	'name':          'iota',
			'parent_smooth': 'smooth_iota',
			'child_smooth':  None
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'meas_noise_effect',        'value':meas_noise_effect     },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },
		{ 'name':'parent_node_name',       'value':'world'        },
		{ 'name':'random_seed',            'value':'0'            },
		{ 'name':'zero_sum_child_rate',    'value':'iota'         },

		{ 'name':'quasi_fixed',            'value':'false'        },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-12'        }
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
for meas_noise_effect in [
	'add_std_scale_none' ,
	'add_std_scale_log' ,
	'add_std_scale_all' ,
	'add_var_scale_none' ,
	'add_var_scale_log' ,
	'add_var_scale_all' ,
] :
	print(meas_noise_effect)
	#
	# create database
	file_name      = 'example.db'
	example_db(file_name, meas_noise_effect)
	#
	# initialize
	program        = '../../devel/dismod_at'
	cmd            = [ program, file_name, 'init' ]
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at init command failed')
	#
	# fit
	cmd = [ program, file_name, 'fit', 'fixed' ]
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
	#
	# check fit results
	new           = False
	connection    = dismod_at.create_connection(file_name, new)
	var_table     = dismod_at.get_table_dict(connection, 'var')
	fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
	connection.close()
	#
	max_error    = 0.0
	for var_id in range( len(var_table) ) :
		row        = fit_var_table[var_id]
		var_type   = var_table[var_id]['var_type']
		fit_value  = row['fit_var_value']
		true_value = None
		if var_type == 'mulcov_meas_noise' :
			true_value = gamma_true
		if var_type == 'rate' :
			true_value = iota_true
		assert( true_value != None )
		# remove # at start of next line to see relative error values
		# print (true_value, fit_value, fit_value / true_value - 1.0)
		max_error = max( abs(fit_value / true_value - 1.0), max_error)
	if max_error > 0.20 :
		print('max_error = ', max_error)
		assert(False)
# -----------------------------------------------------------------------------
print('fit_meas_noise.py: OK')
