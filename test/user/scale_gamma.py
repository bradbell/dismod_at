# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# BEGIN PYTHON
iota_true  = 1e-1
meas_std   = 1e-2
gamma_true = meas_std * meas_std
# ---------------------------------------------------------------------------
import sys
import os
import subprocess
import copy
import math
test_program = 'test/user/scale_gamma.py'
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
if not os.path.exists('build/test/user') :
    os.makedirs('build/test/user')
os.chdir('build/test/user')
# ---------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def constant_weight_fun(a, t) :
	return 1.0
def fun_iota(a, t) :
	return ('prior_iota', None, None)
def fun_gamma(a, t) :
	return ('prior_gamma', None, None)
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ----------------------------------------------------------------------
	# age table
	age_list    = [    0.0,   100.0 ]
	#
	# time table
	time_list   = [ 1990.0,  2010.0 ]
	#
	# integrand table
	integrand_table = [
		 { 'name':'Sincidence', 'minimum_meas_cv':0.0 },
	]
	#
	# node table: north_america
	node_table = [
		{ 'name':'north_america', 'parent':'' },
	]
	#
	# weight table: The constant function 1.0 (one age and one time point)
	fun = constant_weight_fun
	weight_table = [
		{ 'name':'constant',  'age_id':[0], 'time_id':[0], 'fun':fun }
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
	# nslist_table:
	nslist_table = dict()
	#
	# avgint_table
	avgint_table = list()
	# ----------------------------------------------------------------------
	# data table: same order as age_list
	data_table = list()
	# values that are the same for all data rows
	data_std = math.sqrt( meas_std * meas_std + gamma_true )
	row = {
		'node':        'north_america',
		'subgroup':    'world',
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'integrand':   'Sincidence',
		'age_lower':    50.0,
		'age_upper':    50.0,
		'meas_value':   iota_true + data_std,
		'meas_std':     meas_std,
		'one':          1.0
	}
	data_table.append( copy.copy(row) )
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_iota
			'name':     'prior_iota',
			'density':  'uniform',
			'lower':    iota_true,
			'mean':     iota_true,
			'upper':    iota_true
		},{	# prior_gamma
			'name':     'prior_gamma',
			'density':  'uniform',
			'lower':    0.00,
			'upper':    10.0 * gamma_true,
			'mean':     0.00,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	smooth_table = [
		{	# smooth_iota
			'name':                     'smooth_iota',
			'age_id':                   [0],
			'time_id':                  [0],
			'fun':                      fun_iota
		},{	# smooth_gamma
			'name':                     'smooth_gamma',
			'age_id':                   [0],
			'time_id':                  [0],
			'fun':                      fun_gamma
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{	'name':          'iota',
			'parent_smooth': 'smooth_iota',
		},
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',  'value':'north_america' },
		{ 'name':'meas_noise_effect', 'value':'add_var_scale_log' },
		{ 'name':'print_level_fixed', 'value':'0' },
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
	return
# ===========================================================================
# create the database
file_name  = 'example.db'
example_db(file_name)
#
program      = '../../devel/dismod_at'
command_list = [
	[ program, file_name, 'init' ],
	[ program, file_name, 'fit', 'fixed' ],
]
for command in command_list :
	print( ' '.join(command) )
	flag = subprocess.call( command )
	if flag != 0 :
		sys.exit('The dismod_at command failed')
# -----------------------------------------------------------------------
# result tables
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
n_var          = len( var_table )
parent_node_id = 0
iota_rate_id   = 1
for var_id in range( n_var ) :
	row    = var_table[var_id]
	value  = fit_var_table[var_id]['fit_var_value']
	if row['var_type'] == 'rate' :
		assert row['node_id'] == parent_node_id
		assert row['rate_id'] == iota_rate_id
		assert abs( value / iota_true - 1.0 ) < 1e-10
	else :
		assert row['var_type'] == 'mulcov_meas_noise'
		assert abs( value / gamma_true - 1.0 ) < 1e-5
# -----------------------------------------------------------------------------
print('scale_gamma.py: OK')
# END PYTHON
