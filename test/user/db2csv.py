# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# 1. Check meas_std, Delta, and delta in data.csv
# ---------------------------------------------------------------------------
#
iota_true       = 1e-1
meas_value      = 1.1 * iota_true
meas_std        = 1e-5
minimum_meas_cv = 1e-1
Delta           = max(minimum_meas_cv * meas_value, meas_std)
gamma_true      = Delta * Delta
# ---------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
import math
import csv
test_program = 'test/user/db2csv.py'
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
		 { 'name':'Sincidence', 'minimum_meas_cv':minimum_meas_cv },
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
		'meas_value':   meas_value,
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
			'lower':    iota_true / 10.0,
			'mean':     iota_true,
			'upper':    10.0 *iota_true
		},{	# prior_gamma
			'name':     'prior_gamma',
			'density':  'uniform',
			'lower':    0.00,
			'mean':     gamma_true,
			'upper':    10.0 * gamma_true
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
		{ 'name':'max_num_iter_fixed', 'value':'-1' },
		{ 'name':'parent_node_name',   'value':'north_america' },
		{ 'name':'meas_noise_effect',  'value':'add_var_scale_log' },
		{ 'name':'print_level_fixed',  'value':'0' },
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
	return
# ===========================================================================
# create the database
file_name  = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
command_list = [
	[ program  , file_name, 'init' ],
	[ program  , file_name, 'fit', 'fixed' ],
]
for command in command_list :
	print( ' '.join(command) )
	flag = subprocess.call( command )
	if flag != 0 :
		sys.exit('db2csv.py: command failed')
# must go back to distribution directory to run bin/dismodat.py in sandbox
os.chdir('../../..')
command = [ 'bin/dismodat.py', 'build/test/user/' + file_name, 'db2csv' ]
print( ' '.join(command) )
flag = subprocess.call( command )
if flag != 0 :
	sys.exit('db2csv.py: command failed')
# -----------------------------------------------------------------------
# data.csv
data_file = open('build/test/user/data.csv', 'r')
reader    = csv.DictReader(data_file)
for row in reader :
	# only one data row
	assert int(row['data_id']) == 0
	#
	# meas_std
	result = float( row['meas_std'] )
	assert abs( 1.0 - result / meas_std) < 1e-4
	#
	# Delta
	result = float( row['Delta'] )
	assert abs( 1.0 - result / Delta) < 1e-4
	#
	# delta
	result = float( row['delta'] )
	delta  = math.sqrt( Delta * Delta + gamma_true)
	assert abs( 1.0 - result / delta) < 1e-4
# -----------------------------------------------------------------------------
print('db2csv.py: OK')
sys.exit(0)
