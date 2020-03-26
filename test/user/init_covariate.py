# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Test for a bug that where init command does not set age_id and time_id
# correctly for covariate multipliers. Fixed 2019-01-23
#
# true values used to simulate data
iota_true        = 0.05
mulcov_incidence = 1.0
n_data           = 51
# ------------------------------------------------------------------------
import csv
import sys
import os
import distutils.dir_util
import subprocess
import copy
import math
test_program = 'test/user/init_covariate.py'
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
# Note that the a, t values are not used for this example
def constant_weight_fun(a, t) :
	return 1.0
# note that the a, t values are not used for this case
def fun_iota(a, t) :
	return ('prior_iota', None, None)
def fun_mulcov(a, t) :
	return ('prior_mulcov', None, None)
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
		{ 'name':'Sincidence' },
	]
	#
	# node table: world
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
		{'name':'sex',    'reference':0.0, 'max_difference':0.6}
	]
	#
	# mulcov table
	mulcov_table = [
		{
			'covariate': 'sex',
			'type':      'meas_value',
			'effected':  'Sincidence',
			'group':     'world',
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
		'node':        'world',
		'subgroup':    'world',
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   1995.0,
		'time_upper':   1995.0,
		'integrand':    'Sincidence',
	}
	# values that change between rows:
	for data_id in range( n_data ) :
		age         =  data_id * 100.0 / float(n_data - 1)
		sex         = ( data_id % 3 - 1.0 ) / 2.0
		meas_value  = iota_true
		effect      = sex * mulcov_incidence
		meas_value *= math.exp(effect) * iota_true
		row['meas_value'] = meas_value
		row['meas_std']   = meas_value / 10.0
		row['sex']        = sex
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
			'lower':    0.01,
			'mean':     0.1,
			'upper':    1.0,
		},{ # prior_mulcov
			'name':     'prior_mulcov',
			'density':  'uniform',
			'lower':    -5.0,
			'upper':     5.0,
			'mean':     0.0,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	smooth_table = [
		{ # smooth_iota
			'name':                     'smooth_iota',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'fun':                      fun_iota
		},{ # smooth_mulcov
			'name':                     'smooth_mulcov',
			'age_id':                   [2],
			'time_id':                  [0],
			'fun':                      fun_mulcov
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'iota',
			'parent_smooth': 'smooth_iota',
			'child_smooth':  None
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'world'        },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },
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
# -----------------------------------------------------------------------------
file_name      = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
#
# init command
command = [ program, file_name, 'init' ]
print( ' '.join(command) )
flag = subprocess.call( command )
if flag != 0 :
	sys.exit('The dismod_at init command failed')
#
# db2csv command
dismod_at.db2csv_command( file_name )
#
# read the variable.csv file
csvfile = open('variable.csv', 'r')
reader  = csv.DictReader(csvfile)
for row in reader :
	if row['var_type'] == 'rate' :
		check = {
			'rate': 'iota',
			'node': 'world',
			'fixed': 'true',
			'start': 0.1,
			'scale': 0.1,
			'mean_v': 0.1,
			'lower_v': 0.01,
			'upper_v': 1.0,
			'density_v': 'uniform'
		}
	else :
		check = {
			'integrand': 'Sincidence',
			'covariate': 'sex',
			'fixed': 'true',
			'start': 0.0,
			'scale': 0.0,
			'mean_v': 0.0,
			'lower_v': -5.0,
			'upper_v': 5.0,
			'density_v': 'uniform'
		}

	for key in check :
		if type( check[key] ) == float :
			assert float( row[key] ) == check[key]
		else :
			assert row[key] == check[key]
csvfile.close()
print('init_covariate.py: OK')
