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
# BEGIN PYTHON
iota_true  = 1e-4
omega_true = 1e-3
# ---------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
test_program = 'test/user/scale_zero.py'
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
# ---------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def constant_weight_fun(a, t) :
	return 1.0
def fun_iota(a, t) :
	return ('prior_iota', None, None)
def fun_omega(a, t) :
	return ('prior_omega', None, None)
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
		 { 'name':'mtother', 'minimum_meas_cv':0.0 }
	]
	#
	# node table: north_america -> (united_states, canada)
	node_table = [
		{ 'name':'north_america', 'parent':'' },
		{ 'name':'united_states', 'parent':'north_america' },
		{ 'name':'canada',        'parent':'north_america' }
	]
	#
	# weight table: The constant function 1.0 (one age and one time point)
	fun = constant_weight_fun
	weight_table = [
		{ 'name':'constant',  'age_id':[0], 'time_id':[0], 'fun':fun }
	]
	#
	# covariate table: no covriates
	covariate_table = list()
	#
	# mulcov table
	mulcov_table = list()
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
	row = {
		'node':        'north_america',
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'integrand':   'Sincidence',
		'age_lower':    50.0,
		'age_upper':    50.0,
		'meas_value':   iota_true,
		'meas_std':     iota_true / 10.0
	}
	data_table.append( copy.copy(row) )
	row['integrand']  = 'mtother'
	row['meas_value'] = omega_true
	row['meas_std']   = omega_true / 10.0
	data_table.append( copy.copy(row) )
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_iota
			'name':     'prior_iota',
			'density':  'uniform',
			'lower':    1e-8,
			'mean':     0.01,
			'upper':    1.00
		},{	# prior_omega
			'name':     'prior_omega',
			'density':  'uniform',
			'lower':    0.00,
			'upper':    0.00,
			'mean':     0.00,
			'eta':      1e-8
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
		},{	# smooth_omega
			'name':                     'smooth_omega',
			'age_id':                   [0],
			'time_id':                  [0],
			'fun':                      fun_omega
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{	'name':          'iota',
			'parent_smooth': 'smooth_iota',
		},
		{	'name':          'omega',
			'parent_smooth': 'smooth_omega',
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name', 'value':'north_america' }
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
	[ program, file_name, 'set', 'start_var', 'fit_var'],
	[ program, file_name, 'fit', 'fixed' ]
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
omega_rate_id  = 4
for var_id in range( n_var ) :
	row    = var_table[var_id]
	value  = fit_var_table[var_id]['fit_var_value']
	assert row['var_type'] == 'rate'
	assert row['node_id'] == parent_node_id
	if row['rate_id'] == iota_rate_id :
		assert abs( value / iota_true - 1.0 ) < 1e-2
	elif row['rate_id'] == omega_rate_id :
		assert value == 0.0
	else :
		assert False
# -----------------------------------------------------------------------------
print('scale_zero.py: OK')
# END PYTHON
