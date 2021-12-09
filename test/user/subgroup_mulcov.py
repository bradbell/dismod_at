#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# This case used to produce an error when checking the derivative
# ---------------------------------------------------------------------------
iota_parent_true      = 0.1  # value corresponding to parent with no effects
meas_cv               = 0.1  # coefficient of variation for each data point
# ----------------------------------------------------------------------------
# BEGIN PYTHON
#
import sys
import os
import copy
import random
import math
import numpy
import shutil
# ----------------------------------------------------------------------------
# run in build/test/user using local (not installed) version of dismod_at
test_program = 'test/user/subgroup_mulcov.py'
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
# ----------------------------------------------------------------------------
# example.db function
def example_db (file_name) :
	def fun_iota_parent(a, t) :
		return ('prior_iota_parent_value', None, None)
	def fun_std_normal(a, t) :
		return ('prior_std_normal', None, None)
	#
	# age_table
	age_list = [ 0.0, 100.0 ]
	#
	# time_table
	time_list = [ 1990.0, 2020.0 ]
	#
	# node_table
	node_table = [
		{ 'name':'p1',      'parent':''    },
		{ 'name':'c1',      'parent':'p1'  },
		{ 'name':'c2',      'parent':'p1'  },
	]
	#
	# subgroup_table
	subgroup_table = [
		{ 'subgroup':'s1',    'group':'g1'   },
		{ 'subgroup':'s2',    'group':'g1'   },
	]
	# rate_table
	rate_table = [ {
		'name':          'iota',
		'parent_smooth': 'smooth_iota_parent',
		'child_smooth':  'smooth_std_normal',
	} ]
	# covariate_table
	covariate_table = [
		{ 'name':'one',    'reference':0.0 }
	]
	# mulcov_table
	mulcov_table = [ {
		'covariate': 'one',
		'type':      'rate_value',
		'effected':  'iota',
		'group':     'g1',
		'smooth':    None,
		'subsmooth': 'smooth_std_normal',
	} ]
	# prior_table
	prior_table = [
		{   # prior_iota_parent_value
			'name':    'prior_iota_parent_value',
			'density': 'gaussian',
			'upper':   iota_parent_true * 10.0,
			'lower':   iota_parent_true / 10.0,
			'mean':    iota_parent_true,
			'std':     1.0,
		},{ # prior_std_normal
			'name':    'prior_std_normal',
			'density': 'gaussian',
			'mean':     0.0,
			'std':      1.0,
		}
	]
	# smooth_table
	smooth_table = [
		{   # smooth_iota_parent
			'name' :    'smooth_iota_parent',
			'age_id':   [0],
			'time_id':  [0],
			'fun':      fun_iota_parent
		},{ # smooth_std_normal
			'name' :    'smooth_std_normal',
			'age_id':   [0],
			'time_id':  [0],
			'fun':      fun_std_normal
		}
	]
	# weight table:
	weight_table = list()
	# nslist_table
	nslist_table = dict()
	# option_table
	option_table = [
		{ 'name':'parent_node_name',      'value':'p1'},
		{ 'name':'rate_case',             'value':'no_ode'},
		{ 'name':'quasi_fixed',           'value':'true'},
		{ 'name':'max_num_iter_fixed',    'value':'0'},
		{ 'name':'print_level_fixed',     'value':'0'},
		{ 'name':'derivative_test_fixed', 'value':'adaptive'},
	]
	# integrand_table
	integrand_table = [ {'name':'Sincidence'} ]
	# ------------------------------------------------------------------------
	# data_table
	data_table = list()
	# values that are the same for all data points
	row = {
		'integrand':   'Sincidence',
		'density':     'gaussian',
		'weight':      '',
		'hold_out':    False,
		'age_lower':   50.0,
		'age_upper':   50.0,
		'time_lower':  2000.0,
		'time_upper':  2000.0,
		'one':         1.0,
		'node':        'c1',
		'subgroup':    's2',
	}
	row['meas_value'] = iota_parent_true
	row['meas_std']   = row['meas_value'] * meas_cv
	data_table.append( copy.copy(row) )
	# ------------------------------------------------------------------------
	# avgint_table
	avgint_table = list()
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
# ---------------------------------------------------------------------------
# Fit Both
# ---------------------------------------------------------------------------
#
# create the database
file_name  = 'example.db'
example_db(file_name)
#
# initialize the database
program = '../../devel/dismod_at'
dismod_at.system_command_prc( [ program, file_name, 'init' ] )
#
# fit both the fixed and random effects
dismod_at.system_command_prc( [ program, file_name, 'fit', 'both' ] )
#
# db2csv
print('db2csv_command(' + file_name + ')')
dismod_at.db2csv_command( file_name )
# ----------------------------------------------------------------------------
print('subgroup_mulcov.py: OK')
# END PYTHON
