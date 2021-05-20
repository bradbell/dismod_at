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
omega_true      = 1e-2
omega_mean      = 2e-2;
minimum_meas_cv = 0.2;
# ---------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import math
import subprocess
import csv
#
test_program = 'test/user/minimum_cv.py'
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
def constant_one_fun(a, t):
	return 1.0
def fun_omega_parent(a, t):
	return ('prior_omega_parent', None, None)
# ------------------------------------------------------------------------
def example_db ():
	# ----------------------------------------------------------------------
	# age list
	age_list    = [ 0.0, 100.0 ]
	#
	# time list
	time_list   = [ 1995.0, 2015.0 ]
	#
	# only one integrand in this example
	integrand_table = [
		 { 'name':'susceptible', 'minimum_meas_cv':minimum_meas_cv }
	]
	#
	# just the world (which has no parent)
	node_table = [ { 'name':'world', 'parent':'' } ]
	#
	# weight table is represented by one (age, time) point with value 1.0
	fun = constant_one_fun
	weight_table = [
		{ 'name':'constant_one', 'age_id':[0], 'time_id':[0], 'fun':fun }
	]
	#
	# ---------------------------------------------------------------------
	covariate_table = list()
	avgint_table    = list()
	mulcov_table    = list()
	nslist_table    = dict()
	# ---------------------------------------------------------------------
	# data table
	meas_value     = math.exp( - omega_true * 50.0 )
	meas_std       = meas_value / 1e10 # very small so use minimum_meas_cv
	data_table = [
		{
			'data_info':   'd1',
			'integrand':   'susceptible',
			'node':        'world',
			'subgroup':    'world',
			'weight':      'constant_one',
			'age_lower':   50.0,
			'age_upper':   50.0,
			'time_lower':  2000.0,
			'time_upper':  2000.0,
			'income':      1000.0,
			'density':     'gaussian',
			'hold_out':    False,
			'meas_value':  meas_value,
			'meas_std':    meas_std,
		}
	]
	# ---------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_omega_parent
			'name':     'prior_omega_parent',
			'density':  'uniform',
			'lower':    1e-4,
			'upper':    1.0,
			'mean':     omega_mean,
		}
	]
	# ---------------------------------------------------------------------
	# smooth table:
	smooth_table = [
		{	# smooth_omega_parent
			'name':                     'smooth_omega_parent',
			'age_id':                   [0],
			'time_id':                  [0],
			'fun':                      fun_omega_parent
		}
	]
	# ---------------------------------------------------------------------
	# rate table
	rate_table = [
		{	'name':          'omega',
			'parent_smooth': 'smooth_omega_parent',
		}
	]
	# -------------------------------------------------------------------
	# option_table
	# maximum number of iterations is -1 so compute residuals at start value
	option_table = [
		{'name':'parent_node_name',     'value':'world'             },
		{'name':'ode_step_size',        'value':'10.0'              },
		{'name':'rate_case',            'value':'iota_zero_rho_zero'},
		{'name':'avgint_extra_columns', 'value':'avgint_info'       },
		{'name':'data_extra_columns',   'value':'data_info'         },
		{'name':'max_num_iter_fixed',   'value':'-1'                }
	]
	# -------------------------------------------------------------------
	# subgroup_table
	subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
	# -------------------------------------------------------------------
	# create database
	file_name = 'example.db'
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
# ---------------------------------------------------------------------------
example_db()
file_name = 'example.db'
program   = '../../devel/dismod_at'
#
cmd       = [ program, file_name, 'init' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at init command failed')
#
cmd       = [ program, file_name, 'fit', 'fixed' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at init command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
#
# get variable and fit_var tables
var_table      = dismod_at.get_table_dict(connection, 'var')
data_table     = dismod_at.get_table_dict(connection, 'data')
fit_var_table  = dismod_at.get_table_dict(connection, 'fit_var')
fit_data_table = dismod_at.get_table_dict(connection, 'fit_data_subset')
#
omega_id = 4
n_var    = len(var_table)
assert( n_var == 1)
assert( var_table[0]['var_type'] == 'rate' )
assert( var_table[0]['rate_id'] == omega_id )
assert( fit_var_table[0]['fit_var_value'] == omega_mean )
#
# eps
eps     = 1e-10
#
# n_data
n_data  = 1
assert( n_data == len(fit_data_table) )
assert( n_data == len(data_table) )
#
# avg_integrand
avg_integrand = fit_data_table[0]['avg_integrand']
check         = math.exp( - omega_mean * 50.0 )
assert( abs(avg_integrand / check - 1.0) < eps )
#
# check minimum_cv is applied
meas_value    = data_table[0]['meas_value']
Delta         = minimum_meas_cv * meas_value
residual      = fit_data_table[0]['weighted_residual']
check         = (meas_value - avg_integrand) / Delta
assert( abs(residual / check - 1.0) < eps )
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
# data_csv
file_ptr   = open('data.csv', 'r')
reader     = csv.DictReader(file_ptr)
data_csv = list()
for row in reader :
	data_csv.append(row)
file_ptr.close()
assert( n_data == len(data_csv) )
#
# only 4 digits of accuracy in data.csv
eps    = 1e-4
#
meas_stdcv  = float( data_csv[0]['meas_stdcv'] )
check       = minimum_meas_cv * meas_value
assert( abs(meas_stdcv / check - 1.0) < 1e-4 )
# -----------------------------------------------------------------------
print('minimum_cv.py: OK')
