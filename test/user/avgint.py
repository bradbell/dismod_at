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
# This tests for a bug in the average integrand when the first (last) entry in
# the age (time) table was not the minimum (maximum) entry.
# ---------------------------------------------------------------------------
import sys
import os
import copy
import distutils.dir_util
import subprocess
test_program = 'test/user/avgint.py'
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
# note that the a, t values are not used for this example
def fun_one(a, t) :
	return 1.0
def fun_rate(a, t) :
	return ('prior_rate', 'prior_difference', 'prior_difference')
# ---------------------------------------------------------------------------
def data_list2dict(value_list) :
	key_list = [
		'integrand',  'density',    'node',      'weight',    'hold_out',
		'meas_value', 'meas_std',   'eta',       'age_lower', 'age_upper',
		'time_lower', 'time_upper', 'sex',       'one'
	]
	assert len(value_list) == len(key_list)
	dict_out = dict()
	for i in range( len(value_list) ) :
		dict_out[key_list[i]] = value_list[i]
	return copy.copy( dict_out )
# ------------------------------------------------------------------------
# age table: first list fails, second works.
#             ages are not in order
age_list    = [ 10.0, 40.0, 100.0,  0.0 ]
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ----------------------------------------------------------------------
	# time table
	time_list   = [ 1990.0, 2015.0 ]
	#
	# integrand table
	integrand_table = [
		{ 'name':'Sincidence', 'minimum_meas_cv':0.0 }
	]
	#
	# node table: world
	node_table = [
		{ 'name':'world',         'parent':'' }
	]
	#
	# weight table: The constant function 1.0 (one age and one time point)
	weight_table = [
		{ 'name':'weight_one',  'age_id':[1], 'time_id':[1], 'fun':fun_one }
	]
	#
	# covariate table:
	covariate_table = list()
	#
	# mulcov table
	mulcov_table = list()
	#
	# ----------------------------------------------------------------------
	# data table: order for value_list
	#	'integrand',  'density',    'node',      'weight', '     hold_out',
	#	'meas_value', 'meas_std',   'age_lower', 'age_upper',
	#   'time_lower', 'time_upper', 'sex',       'one'
	data_table = list()
	#
	# Sincidence
	value_list = [
		'Sincidence', 'log_gaussian', 'world',    'weight_one',  0,
		4.0e-3,       2.0e-3,         1e-6,        40,          40,
		1995,         1995,           0.0,         1
	]
	data_table.append( data_list2dict( value_list ) );
	#
	value_list = [
		'Sincidence', 'log_gaussian', 'world',    'weight_one',  0,
		4.0e-2,       2.0e-2,         1e-6,        100,          100,
		1995,         1995,           0.0,         1
	]
	data_table.append( data_list2dict( value_list ) );
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
		},{ # prior_rate
			'name':     'prior_rate',
			'density':  'log_gaussian',
			'lower':    1e-5,
			'upper':    1e+0,
			'mean':     1e-2,
			'std':      1e-0,
			'eta':      1e-6
		},{ # prior_difference
			'name':     'prior_difference',
			'density':  'log_gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      1.0,
			'eta':      1e-6
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	middle_age_id  = 1
	last_time_id   = 2
	smooth_table = [
		{
		# smooth_rate
			'name':                     'smooth_rate',
			'age_id':                   range(len(age_list)),
			'time_id':                  [0],
			'mulstd_value_prior_name':  None,
			'mulstd_dage_prior_name':   None,
			'mulstd_dtime_prior_name':  None,
			'fun':                      fun_rate
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'pini',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		},{
			'name':          'iota',
			'parent_smooth': 'smooth_rate',
			'child_smooth':  None,
			'child_nslist':  None
		},{
			'name':          'rho',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		},{
			'name':          'chi',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		},{
			'name':          'omega',
			'parent_smooth': None,
			'child_smooth':  None,
			'child_nslist':  None
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'world'             },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },
		#
		{ 'name':'quasi_fixed',            'value':'false'             },
		{ 'name':'derivative_test_fixed',  'value':'first-order'       },
		#
		{ 'name':'derivative_test_random', 'value':'second-order'      }
	]
	# ----------------------------------------------------------------------
	# avgint table: same order as list of integrands
	avgint_table = list()
	# ----------------------------------------------------------------------
	# nslist_table:
	nslist_table = dict()
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
	return
# ===========================================================================
file_name  = 'example.db'
example_db(file_name)
new             = False
connection      = dismod_at.create_connection(file_name, new)
#
# Test that density table does not need entries that are not used
# (last entry is log_students so can delete without changing density ids)
sqlcmd  = 'DELETE FROM density WHERE density_name IN '
sqlcmd += '("log_students", "cen_gaussian", "cen_laplace"'
sqlcmd += ',"cen_log_gaussian", "cen_log_laplace")'
dismod_at.sql_command(connection, sqlcmd)
connection.close()
#
# run init and fit
program    = '../../devel/dismod_at'
for command in [ 'init', 'fit' ] :
	cmd = [ program, file_name, command ]
	if command == 'fit' :
		variables = 'both'
		cmd.append(variables)
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
#
# get variable and fit_var tables
connection            = dismod_at.create_connection(file_name, new)
var_table             = dismod_at.get_table_dict(connection, 'var')
fit_var_table         = dismod_at.get_table_dict(connection, 'fit_var')
fit_data_subset_dict = dismod_at.get_table_dict(connection, 'fit_data_subset')
# -----------------------------------------------------------------------------
count = 0
n_var = len(var_table)
for var_id in range(n_var) :
	var    = var_table[var_id]
	age_id = var['age_id']
	age    = age_list[age_id]
	if age == 40 :
		fit           = fit_var_table[var_id]
		iota_at_40    = fit['fit_var_value']
		data          = fit_data_subset_dict[0]
		avgint_at_40  = data['avg_integrand']
		assert abs( avgint_at_40 / iota_at_40 - 1.0 ) < 1e-10
		count += 1


print('avgint.py: OK')
