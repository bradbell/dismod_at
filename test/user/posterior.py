# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
omega_world_mean  = 1e-2
number_sample     = 2
# ------------------------------------------------------------------------
import sys
import os
import subprocess
import copy
import csv
from math import exp
test_program = 'test/user/posterior.py'
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
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def fun_rate_world(a, t) :
	return ('prior_rate_world', None, 'prior_world_diff')
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ----------------------------------------------------------------------
	# age table
	age_list    = [    0.0, 100.0 ]
	#
	# time table
	time_list   = [ 1995.0, 2015.0 ]
	#
	# integrand table
	integrand_table = list()
	#
	# node table:
	node_table = [ { 'name':'world',  'parent':'' } ]
	#
	# weight table
	weight_table = list()
	#
	# covariate table: no covriates
	covariate_table = list()
	#
	# mulcov table
	mulcov_table = list()
	#
	# avgint table: same order as list of integrands
	avgint_table = list()
	#
	# nslist_table:
	nslist_table = dict()
	#
	# data table:
	data_table = list()
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_rate_world
			'name':     'prior_rate_world',
			'density':  'gaussian',
			'mean':     omega_world_mean,
			'std':      omega_world_mean
		},{ # prior_world_diff
			'name':     'prior_world_diff',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      omega_world_mean
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	last_time_id = len(time_list) - 1
	smooth_table = [
		{ # smooth_rate_world
			'name':                     'smooth_rate_world',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0, last_time_id ],
			'fun':                       fun_rate_world
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'omega',
			'parent_smooth': 'smooth_rate_world',
			'child_smooth':  None,
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'world'             },
		{ 'name':'rate_case',              'value':'iota_zero_rho_zero'},

		{ 'name':'quasi_fixed',            'value':'true'              },
		{ 'name':'derivative_test_fixed',  'value':'first-order'       },
		{ 'name':'max_num_iter_fixed',     'value':'100'               },
		{ 'name':'print_level_fixed',      'value':'0'                 },
		{ 'name':'tolerance_fixed',        'value':'1e-11'             },

		{ 'name':'derivative_test_random', 'value':'second-order'      },
		{ 'name':'max_num_iter_random',    'value':'100'               },
		{ 'name':'print_level_random',     'value':'0'                 },
		{ 'name':'tolerance_random',       'value':'1e-11'             }
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
# Create database and run init, start, fit with just fixed effects
file_name = 'example.db'
example_db(file_name)
program   = '../../devel/dismod_at'
#
# init
cmd       = [ program, file_name, 'init' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at init command failed')
#
# set truth_var prior_mean
cmd       = [ program, file_name, 'set', 'truth_var', 'prior_mean' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at set command failed')
#
# simulate number_sample
cmd       = [ program, file_name, 'simulate', str(number_sample) ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at simulate command failed')
#
# sample simulate number_sample
cmd = [ program, file_name, 'sample', 'simulate', 'both', str(number_sample) ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at sample command failed')
#
# fit both simulate_index
cmd       = [ program, file_name, 'fit', 'both', '1' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at fit command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# check the zero random effects solution
#
# read database tables
var_table      = dismod_at.get_table_dict(connection, 'var')
fit_var_table  = dismod_at.get_table_dict(connection, 'fit_var')
sample_table   = dismod_at.get_table_dict(connection, 'sample')
#
# check omega_0 and omega_1 are only variables
number_var = 2
assert len(var_table) == number_var
assert len(fit_var_table) == number_var
assert len(sample_table) == number_sample * number_var
#
# check fit_var is same as sample_table with sample_index == 1
sample_index = 1
for var_id in range( number_var ) :
	sample_id = sample_index * number_var + var_id
	sample_var_value = sample_table[sample_id]['var_value']
	fit_var_value    = fit_var_table[var_id]['fit_var_value']
	assert abs( fit_var_value / sample_var_value - 1.0 ) < 1e-10
# -----------------------------------------------------------------------
# run sandbox version of dismodat.py example.db db2csv
os.chdir('../../..')
program        = 'bin/dismodat.py'
file_name      = 'build/test/user/example.db'
command        = 'db2csv'
cmd            = [ program, file_name, command ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at db2csv command failed')
os.chdir('build/test/user')
#
# variable_table
variable_table = list()
file_ptr       = open('variable.csv', 'r')
reader         = csv.DictReader(file_ptr)
for row in reader :
	variable_table.append(row)
file_ptr.close
#
def compare(in_db, in_csv) :
	if in_db == None :
		assert in_csv == ''
	else :
		check = float( in_csv )
		assert abs( check / in_db - 1.0 ) < 1e-4
#
# prior_sim_table
prior_sim_table = dismod_at.get_table_dict(connection, 'prior_sim')
simulate_index  = 1
assert len(variable_table) == number_var
for var_id in range( number_var ) :
	simulate_id      = simulate_index * number_var + var_id
	#
	prior_sim_value  = prior_sim_table[simulate_id]['prior_sim_value']
	sim_v            = variable_table[var_id]['sim_v']
	compare(prior_sim_value, sim_v)
	#
	prior_sim_dage   = prior_sim_table[simulate_id]['prior_sim_dage']
	sim_a            = variable_table[var_id]['sim_a']
	compare(prior_sim_value, sim_v)
	#
	prior_sim_dtime  = prior_sim_table[simulate_id]['prior_sim_dtime']
	sim_t            = variable_table[var_id]['sim_t']
	compare(prior_sim_value, sim_v)
# -----------------------------------------------------------------------
print('posterior.py: OK')
# END PYTHON
