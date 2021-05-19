#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
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
iota_true       = 0.01
eta_global      = iota_true * 1e-3
meas_std_global = iota_true * 0.5
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
import time
import numpy
test_program = 'example/user/sim_log.py'
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
#
# random_seed_str
random_seed_str = str( int( time.time() ) )
# ----------------------------------------------------------------------------
# run a system command
def system_command(command) :
	print( ' '.join(command) )
	flag = subprocess.call( command )
	if flag != 0 :
		msg  = 'command failed: flag = ' + str(flag)
		msg += ', random_seed = ' + random_seed_str
		sys.exit(msg)
	return
# ------------------------------------------------------------------------
def fun_iota_parent(a, t) :
	return ('prior_iota_parent', None, None)
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ 0.0, 100.0 ]
	#
	# time table:
	time_list   = [ 1990.0, 2020.0 ]
	#
	# integrand table:
	integrand_table = [
		 { 'name':'Sincidence', 'minimum_meas_cv':0.0 }
	]
	#
	# node table:
	node_table = [ { 'name':'world', 'parent':'' } ]
	#
	# empty tables
	weight_table    = list()
	covariate_table = list()
	mulcov_table    = list()
	avgint_table    = list()
	nslist_table = dict()
	# ----------------------------------------------------------------------
	# data table:
	# values that are the same for all data rows
	row = {
		'meas_value':  iota_true  ,
		'eta':         eta_global ,
		'weight':      '',
		'hold_out':     False,
		'time_lower':   2000.,
		'time_upper':   2000.,
		'age_lower':    40.0,
		'age_upper':    40.0,
		'subgroup':     'world',
		'density':      'log_gaussian',
		'meas_std':     meas_std_global,
		'node':         'world',
		'integrand':    'Sincidence',
	}
	data_table = [ row ]
	# ----------------------------------------------------------------------
	# prior_table
	# Note that the prior mean is equal to the true value for iota
	prior_table = [
		{ # prior_iota_parent
			'name':     'prior_iota_parent',
			'density':  'uniform',
			'lower':    iota_true / 100.,
			'upper':    iota_true * 10.0,
			'mean':     iota_true ,
			'std':      None,
			'eta':      None
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	age_id   = 0
	time_id  = 0
	name     = 'smooth_iota_parent'
	fun      = fun_iota_parent
	smooth_table = [
		{'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	]
	# ----------------------------------------------------------------------
	# rate table:
	rate_table = [
		{	'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
			'child_smooth':  None,
			'child_nslist':  None
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },
		{ 'name':'parent_node_name',       'value':'world'             },
		{ 'name':'random_seed',            'value':random_seed_str     },

		{ 'name':'quasi_fixed',            'value':'false'             },
		{ 'name':'max_num_iter_fixed',     'value':'50'                },
		{ 'name':'print_level_fixed',      'value':'0'                 },
		{ 'name':'tolerance_fixed',        'value':'1e-6'              },

		{ 'name':'max_num_iter_random',    'value':'100'               },
		{ 'name':'print_level_random',     'value':'0'                 },
		{ 'name':'tolerance_random',       'value':'1e-8'              }
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
# file_name
file_name      = 'example.db'
#
# create example.db
example_db(file_name)
#
# program
program        = '../../devel/dismod_at'
#
# init database
system_command([ program, file_name, 'init' ])
#
# Note that the prior mean is equal to the true value for iota
system_command([ program, file_name, 'set', 'truth_var', 'prior_mean' ])
#
# create data_sim table
n_simulate = 200
system_command([ program, file_name, 'simulate', str(n_simulate) ])
# -----------------------------------------------------------------------
# check simulated data
from math import log
new                = False
connection         = dismod_at.create_connection(file_name, new)
data_sim_table     = dismod_at.get_table_dict(connection, 'data_sim')
data_table         = dismod_at.get_table_dict(connection, 'data')
data_subset_table  = dismod_at.get_table_dict(connection, 'data_subset')
residual_list      = list()
for row in data_sim_table :
	data_sim_value = row['data_sim_value']
	data_subset_id = row['data_subset_id']
	data_id        = data_subset_table[data_subset_id]['data_id']
	meas_value     = data_table[data_id]['meas_value']
	meas_std       = data_table[data_id]['meas_std']
	eta            = data_table[data_id]['eta']
	assert meas_value == iota_true
	assert meas_std   == meas_std_global
	assert eta        == eta_global
	sigma          = log(meas_value + meas_std + eta) - log(meas_value + eta)
	residual       = (log(data_sim_value + eta) - log(iota_true + eta) )/sigma
	residual_list.append( residual )
residual_array  = numpy.array( residual_list )
residual_mean   = residual_array.mean()
residual_std    = residual_array.std(ddof=1)
# check that the mean of the residuals is within 2.5 standard deviations
assert abs(residual_mean) <=  2.5 / numpy.sqrt(n_simulate)
# check that the standard deviation of the residuals is near one
assert abs(residual_std - 1.0) <= 2.5 / numpy.sqrt(n_simulate)
# -----------------------------------------------------------------------------
print('fit_sim.py: OK')
