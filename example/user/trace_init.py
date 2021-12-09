#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_trace_init.py$$ $newlinech #$$
# $spell
#	def
#	init
#	exp
#	Integrands
#	Sincidence
# $$
#
# $section Using Initialization Trace Option$$
#
# $head Purpose$$
# This example shows how to use the
# $cref/trace_init_fit_model/option_table/trace_init_fit_model/$$ option.
#
# $head Integrands$$
# For this example there are one integrand, $code Sincidence$$.
#
# $head Nodes$$
# There three nodes.
# The first is called $code world$$ and is the parent node for this example.
# The second (third) is called $code child_1$$ ($code child_2$$)
# and is a child of the parent node.
#
# $head True Iota$$
# For this example, the true model incidence rate $icode iota$$ is
def iota_true(age, node) :
	import math
	iota_parent  = 0.01 * (1 + age / 100.0)
	child_effect = 0.2
	if node == 'world' :
		return iota_parent
	if node == 'child_1' :
		return math.exp(+ child_effect) * iota_parent
	if node == 'child_2' :
		return math.exp(- child_effect) * iota_parent
	assert False
#
# $head Model$$
#
# $subhead Parent Node$$
# There is only one rate $icode iota$$ and it linear in age
# with knots at the age 0 and 100.
#
# $subhead Child Nodes$$
# There is only one rate $icode iota$$ and it constant.
#
# $head Data$$
# There are six data points measuring Sincidence
# with the true value of $icode iota$$.
# These correspond to ages 0 and 100 at each of the three nodes.
#
# $head trace_init_fit_model$$
# The option $cref/trace_init_fit_model/option_table/trace_init_fit_model/$$
# is set to true and the corresponding output is checked.
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
import sys
import os
import csv
import copy
import math
test_program = 'example/user/trace_init.py'
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
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def example_db (file_name) :
	def fun_iota_parent(a, t) :
		return ('prior_iota_parent', None, None)
	def fun_iota_child(a, t) :
		return ('prior_iota_child', None, None)
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ 0.0, 100.0 ]
	#
	# time table:
	time_list   = [ 1990.0, 2020.0 ]
	#
	# integrand table:
	integrand_table = [
		 { 'name':'Sincidence' }
	]
	#
	# node table:
	node_table = [
		{ 'name':'world',   'parent':''      } ,
		{ 'name':'child_1', 'parent':'world' } ,
		{ 'name':'child_2', 'parent':'world' } ,
	]
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
	#
	# values that are the same for all data points
	row = {
		'integrand':   'Sincidence',
		'hold_out':    False,
		'density':     'gaussian',
		'weight':      '',
		'time_lower':   2000.,
		'time_upper':   2000.,
		'subgroup':     'world',
	}
	for age in [ 0.0 , 100.0 ] :
		for node in [ 'world', 'child_1', 'child_2' ] :
			# Sincidence
			row['meas_value'] = iota_true(age, node)
			row['meas_std']   = row['meas_value'] / 10.0
			row['age_lower']  = age
			row['age_upper']  = age
			row['node']       = node
			data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_iota_parent
			'name':     'prior_iota_parent',
			'density':  'uniform',
			'lower':    iota_true(0, 'world') / 10.0,
			'upper':    iota_true(0, 'world') * 10.0,
			'mean':     iota_true(0, 'world') / 2.0 ,
		}, {
			# prior_iota_child
			'name':     'prior_iota_child',
			'density':  'uniform',
			'mean':     0.0 ,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	smooth_table = [
		{	'name':    'smooth_iota_parent',
			'age_id':  [0, 1], # ages 0, 100
			'time_id': [0],
			'fun':     fun_iota_parent
		},{
			'name':    'smooth_iota_child',
			'age_id':  [0],
			'time_id': [0],
			'fun':     fun_iota_child
		}
	]
	# ----------------------------------------------------------------------
	# rate table:
	rate_table = [
		{	'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
			'child_smooth':  'smooth_iota_child',
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero'   },
		{ 'name':'parent_node_name',       'value':'world'               },

		{ 'name':'quasi_fixed',            'value':'false'               },
		{ 'name':'max_num_iter_fixed',     'value':'50'                  },
		{ 'name':'print_level_fixed',      'value':'0'                   },
		{ 'name':'tolerance_fixed',        'value':'1e-9'                },

		{ 'name':'trace_init_fit_model',   'value':'true'                },
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
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
stdout = dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
# -----------------------------------------------------------------------
# Check trace_init_fit_model results
check  = 'Begin dismod_at: fit_model constructor\n'
check += 'Begin cppad_mixed::initialize\n'
check += 'init_ran_like_done_\n'
check += 'init_ran_jac_done_\n'
check += 'init_ran_hes_done_\n'
check += 'init_ldlt_ran_hes_done_\n'
check += 'init_hes_cross_done_\n'
check += 'init_fix_like_done_\n'
check += 'init_fix_con_done_\n'
check += 'End cppad_mixed::initialize\n'
check += 'End dismod_at: fit_model constructor\n'
check += 'Begin cppad_mixed::init_laplace_obj\n'
check += 'init_laplace_obj_fun_done_\n'
check += 'init_laplace_obj_hes_done_\n'
check += 'End cppad_mixed::init_laplace_obj\n'
assert stdout == check
# -----------------------------------------------------------------------
# read database
new                   = False
connection            = dismod_at.create_connection(file_name, new)
node_table            = dismod_at.get_table_dict(connection, 'node')
age_table             = dismod_at.get_table_dict(connection, 'age')
var_table             = dismod_at.get_table_dict(connection, 'var')
fit_var_table         = dismod_at.get_table_dict(connection, 'fit_var')
connection.close()
#
# There are four values for iota in this model
assert len(var_table) == 4
assert len(fit_var_table) == 4
#
# check that the fit is accurate
for var_id in range( len(var_table) ) :
	age_id      = var_table[var_id]['age_id']
	age         = age_table[age_id]['age']
	node_id     = var_table[var_id]['node_id']
	node_name   = node_table[node_id]['node_name']
	true_value  = iota_true(age, node_name)
	if node_name.startswith('child_') :
		parent_value = iota_true(age, 'world')
		true_value   = math.log( true_value / parent_value )
	fit_value   = fit_var_table[var_id]['fit_var_value']
	rel_error   = 1.0 - fit_value/true_value
	assert abs(rel_error) < 1e-6
# -----------------------------------------------------------------------------
print('trace_init.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
