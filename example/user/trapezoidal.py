# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
"""
$begin user_trapezoidal.py$$
$spell
	Integrands
	mtexcess
	mtother
$$

$section Using the Trapezoidal ODE Solver$$

$head Rates$$
The non-zero rates in this example are iota, rho, chi, and omega.
They are all constant in age and time because they each have one smoothing
grid point.

$head Integrands$$
The integrands for this example are prevalence, remission, mtexcess,
and mtother.
Modeling the prevalence integrand requires solving the ODE.
The other integrands are direct measurements of the rates.
To be specific, remission measures rho,
mtexcess measures chi, and
mtother measures omega.

$head Data$$
For each of the integrands there is one data point are
age 50 and time 2000.
No noise is added to the data but it is modeled as if it
had a 10 percent coefficient of variation.

$head Random Effects$$
There are no random effects in this example.

$head rate_case$$
The rate case for this example is $code trapezoidal$$.
This case has does not require any of the rates to be positive,
so we use a lower limit of one for all the rates.

$end
---------------------------------------------------------------------------
"""
# BEGIN PYTHON
import math
import time
import sys
import os
import copy
import random
import statistics
# ---------------------------------------------------------------------------
test_program = 'example/user/trapezoidal.py'
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
if not os.path.exists('build/example/user') :
	os.makedirs('build/example/user')
os.chdir('build/example/user')
# ----------------------------------------------------------------------------
def iota_true(age, time) :
	return 0.01
def rho_true(age, time) :
	return 0.2
def chi_true(age, time) :
	return 0.3
def omega_true(age, time) :
	return 0.02
rate_true  = {
	'iota'  : iota_true,
	'rho'   : rho_true,
	'chi'   : chi_true,
	'omega' : omega_true
}
# ---------------------------------------------------------------------------
def example_db (file_name) :
	def fun_rate(a, t) :
		return ('prior_value', None, None)
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ 0.0, 100.0 ]
	#
	# time table:
	time_list   = [ 2000.0, 2020.0 ]
	#
	# integrand table:
	integrand_table = [
		{ 'name' : 'prevalence' },
		{ 'name' : 'remission'  },
		{ 'name' : 'mtexcess'   },
		{ 'name' : 'mtother'    },
	]
	#
	# node table:
	node_table = [ { 'name':'world', 'parent':'' } ]
	#
	# weight table:
	weight_table = list()
	#
	# covariate table:
	covariate_table = list()
	#
	# mulcov table:
	mulcov_table = list()
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
	data_row = {
		'weight':      '',
		'hold_out':     False,
		'node':        'world',
		'subgroup':    'world',
		'density':     'gaussian',
		'age_lower':   50.0,
		'age_upper':   50.0,
		'time_lower':  2000,
		'time_upper':  2000,
	}
	grid           = { 'age' : [50.0] , 'time' : [2000] }
	abs_tol        = 1e-7
	for integrand_row in integrand_table :
		integrand_name         = integrand_row['name']
		meas_value             = dismod_at.average_integrand(
			rate_true, integrand_name, grid, abs_tol
		)
		meas_std               = meas_value * 0.1
		data_row['integrand']  = integrand_name
		data_row['meas_value'] = meas_value
		data_row['meas_std']   = meas_std
		data_table.append( copy.copy(data_row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_value
			'name':     'prior_value',
			'density':  'uniform',
			'lower':    0.0,
			'upper':    1.0,
			'mean':     0.01,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	name           = 'smooth_rate'
	fun            = fun_rate
	age_id         = [0]
	time_id        = [0]
	smooth_table = [
		{'name':name, 'age_id':age_id, 'time_id':time_id, 'fun':fun }
	]
	# ----------------------------------------------------------------------
	# rate table:
	rate_table = list()
	for rate in [ 'iota', 'rho', 'chi', 'omega' ] :
		rate_table.append( {
			'name':          rate,
			'parent_smooth': 'smooth_rate',
			'child_smooth':  None
		} )
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'rate_case',              'value':'trapezoidal' },
		{ 'name':'parent_node_name',       'value':'world'       },
		{ 'name':'ode_step_size',          'value':'5.0'         },
		{ 'name':'print_level_fixed',      'value':'0'           },
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
# Create database
file_name = 'example.db'
example_db(file_name)
program = '../../devel/dismod_at'
#
# fit fixed
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
#
#
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table       = dismod_at.get_table_dict(connection, 'var')
rate_table      = dismod_at.get_table_dict(connection, 'rate')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
rate2integrand  = {
	'iota'  : 'Sincidence',
	'rho'   : 'remission' ,
	'chi'   : 'mtexcess' ,
	'omega' : 'mtother' ,
}
#
grid      = { 'age' : [50.0] , 'time' : [2000] }
abs_tol   = 1e-7
#
assert len(var_table) == 4
for (var_id, row) in enumerate( var_table ) :
	var_type = row['var_type']
	assert var_type == 'rate'
	#
	fit_var_value  = fit_var_table[var_id]['fit_var_value']
	rate_id        = row['rate_id']
	rate_name      = rate_table[rate_id]['rate_name']
	integrand_name = rate2integrand[rate_name]
	true_var_value = dismod_at.average_integrand(
		rate_true, integrand_name, grid, abs_tol
	)
	#
	rel_err        = 1.0 - fit_var_value / true_var_value
	if abs(rel_err) >= 1e-4 :
		print(fit_var_value, true_var_value, rel_err)
	assert abs(rel_err) < 1e-3
# ---------------------------------------------------------------------------
print('trapezoidal.py: OK')
# END PYTHON
