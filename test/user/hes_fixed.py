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
# Test sub-setting fixed Hessian to variables that do not have
# lower and upper bounds equal
# ---------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import copy
import numpy
test_program = 'test/user/hes_fixed.py'
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
prior_omega_std = 0.5e-2
prior_chi_std   = 5.0e-2
# ------------------------------------------------------------------------
def example_db (file_name) :
	#
	def fun_omega(a, t) :
		return ('prior_omega', None, None)
	def fun_child(a, t) :
		return ('prior_child', None, None)
	def fun_chi(a, t) :
		return ('prior_chi', None, None)
	#
	# ----------------------------------------------------------------------
	# age table
	age_list    = [ 0.0, 100.0 ]
	#
	# time table
	time_list   = [ 1980.0, 2020.0 ]
	#
	# integrand table
	integrand_table = [ { 'name':'mtother' }, { 'name':'mtexcess'} ]
	#
	# node table: world
	node_table = [
		{ 'name':'n0',     'parent':'' },
		{ 'name':'n1',     'parent':'n0' },
	]
	#
	# weight table:
	weight_table = list()
	#
	# covariate table:
	covariate_table = list()
	#
	# mulcov table
	mulcov_table = list()
	#
	# avgint table:
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
		{ # prior_omega
			'name':     'prior_omega',
			'density':  'gaussian',
			'mean':     1e-2,
			'std':      prior_omega_std,
		},{ # prior_chi
			'name':     'prior_chi',
			'density':  'gaussian',
			'mean':     1e-2,
			'std':      prior_chi_std,
			'lower':    1e-2,
			'upper':    1e-2,
		},{	# prior_child
			'name':     'prior_child',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      1.0,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	#
	smooth_table = [
		{	# smooth_omega
			'name':                     'smooth_omega',
			'age_id':                   [0],
			'time_id':                  [0],
			'fun':                      fun_omega
		},{	# smooth_chi
			'name':                     'smooth_chi',
			'age_id':                   [0],
			'time_id':                  [0],
			'fun':                      fun_chi
		},{	# smooth_child
			'name':                     'smooth_child',
			'age_id':                   [0],
			'time_id':                  [0],
			'fun':                      fun_child
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{	'name':          'omega',
			'parent_smooth': 'smooth_omega',
			'child_smooth':  'smooth_child',
		},{
		'name':          'chi',
			'parent_smooth': 'smooth_chi',
		},
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'n0'                 },
		{ 'name':'rate_case',              'value':'iota_zero_rho_zero'  }
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
file_name = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init'] )
dismod_at.system_command_prc([ program, file_name, 'fit', 'both'] )
dismod_at.system_command_prc(
	[ program, file_name, 'sample', 'asymptotic', 'both', '20' ]
)
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
#
# some tables
var_table       = dismod_at.get_table_dict(connection, 'var')
rate_table      = dismod_at.get_table_dict(connection, 'rate')
node_table      = dismod_at.get_table_dict(connection, 'node')
hes_fixed_table = dismod_at.get_table_dict(connection, 'hes_fixed')
#
assert len(var_table) == 3
assert len(hes_fixed_table) == 1
omega_var_id = None
for (var_id, row) in enumerate(var_table) :
	rate_id   = row['rate_id']
	node_id   = row['node_id']
	rate_name = rate_table[rate_id]['rate_name']
	node_name = node_table[node_id]['node_name']
	if rate_name == 'omega' and node_name == 'n0' :
		omega_var_id = var_id
assert hes_fixed_table[0]['row_var_id'] ==  omega_var_id
assert hes_fixed_table[0]['col_var_id'] ==  omega_var_id
relative_err = 1.0 - hes_fixed_table[0]['hes_fixed_value'] * prior_omega_std**2
eps99 = 99.0 * numpy.finfo(float).eps
assert abs(relative_err) < eps99
# -----------------------------------------------------------------------------
print('hes_fixed: OK')
