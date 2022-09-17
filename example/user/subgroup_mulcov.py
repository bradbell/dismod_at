# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin user_subgroup_mulcov.py$$ $newlinech #$$
# $spell
#	covariate
#	covariates
#	subgraph
#	Mulcov
#	var
# $$
#
# $section Example Fitting With Subgroup Covariate Multipliers $$
#
# $head Node Table$$
# The following is a diagram of the node tree for this example:
# $pre
#       p1
#      /  \
#     c1  c2
# $$
#
# $head Subgroup Table$$
# The following is a diagram of the subgroup tree for this example:
# $pre
#         p1
#        /  \
#      g1   none
#     /  \    \
#    s1   s2  none
# $$
#
# $head Model Variables$$
#
# $subhead Parent Rates$$
# There is an iota
# $cref/parent rate/model_variables/Fixed Effects, theta/Parent Rates/$$
# corresponding to node p1.
# This variable has a uniform prior.
#
# $subhead Group Covariate Multiplier$$
# There is a
# $cref/
#	group covariate multiplier/
#	model_variables/
#	Fixed Effects, theta/
#	Group Covariate Multipliers
# /$$
# that affects all the data in group g1.
# This variable has a uniform prior.
#
# $subhead Child Rate Effects$$
# There are two
# $cref/child rate effects
#	/model_variables
#	/Random Effects, u
#	/Child Rate Effects
# /$$
# one for the data corresponding to child c1,
# the other for child c2.
# These variables have gaussian priors with mean zero and standard deviation
# 100 times the absolute true child rate effect.
#
# $subhead Subgroup Covariate Multiplier$$
# There are two
# $cref/
#	subgroup covariate multipliers/
#	model_variables/
#	Random Effects, u/
#	Subgroup Covariate Multipliers
# /$$
# one for the data corresponding to subgroup s1,
# the other for subgroup s2.
# These variables have gaussian priors with mean zero and standard deviation
# 100 times the absolute true subgroup effect.
#
# $head Rate Table$$
# There is one entry in the rate table that
# specifies the iota smoothing for the parent node p1.
# There is another entry that specifies the iota smoothing
# for the child nodes c1, c2.
#
# $head Mulcov Table$$
# There is one entry in the mulcov table for group g1 and
# the two subgroups s1, s1 (that make up g1).
# The group smoothing is used for the fixed effect corresponding to g1
# and the subgroup smoothing is used for the s1, s2 random effects.
#
# $head Data Table$$
# Each child (c1, c2) and each subgroup (s1, s2, none)
# have a corresponding data point that is simulated without any noise.
#
# $head Problem Parameters$$
# $srccode%py%
iota_parent_true      = 0.01 # value corresponding to parent with no effects
group_effect_true     = 0.3  # value corresponding to group g1
child_effect_true     = 0.2  # absolute value of child random effects
subgroup_effect_true  = 0.1  # absoulte value of subgroup random effects
meas_cv               = 0.1  # coefficient of variation for each data point
# %$$
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
#
# $end
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
# run in build/example/user using local (not installed) version of dismod_at
test_program = 'example/user/subgroup_mulcov.py'
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
def avg_integrand(node, subgroup) :
	effect = 0.0
	if subgroup == 's1' :
		effect += subgroup_effect_true + group_effect_true
	elif subgroup == 's2' :
		effect += - subgroup_effect_true + group_effect_true
	if node == 'c1' :
		effect += child_effect_true
	elif node == 'c2' :
		effect += - child_effect_true
	#
	return iota_parent_true * math.exp(effect)
# ----------------------------------------------------------------------------
# example.db function
def example_db (file_name) :
	def fun_iota_parent(a, t) :
		return ('prior_iota_parent_value', None, None)
	def fun_iota_child(a, t) :
		return ('prior_iota_child', None, None)
	def fun_iota_group(a, t) :
		return ('prior_iota_group', None, None)
	def fun_iota_subgroup(a, t) :
		return ('prior_iota_subgroup', None, None)
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
		{ 'subgroup':'none',  'group':'none' },
		{ 'subgroup':'s1',    'group':'g1'   },
		{ 'subgroup':'s2',    'group':'g1'   },
	]
	# rate_table
	rate_table = [ {
		'name':          'iota',
		'parent_smooth': 'smooth_iota_parent',
		'child_smooth':  'smooth_iota_child',
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
		'smooth':    'smooth_iota_group',
		'subsmooth': 'smooth_iota_subgroup',
	} ]
	# prior_table
	prior_table = [
		{   # prior_iota_parent_value
			'name':    'prior_iota_parent_value',
			'density': 'uniform',
			'lower':   iota_parent_true / 100.0,
			'upper':   iota_parent_true * 100.0,
			'mean':    iota_parent_true / 2.0,
		},{ # prior_iota_child
			'name':    'prior_iota_child',
			'density': 'gaussian',
			'mean':     0.0,
			'std':      100 * child_effect_true,
		},{ # prior_iota_group
			'name':    'prior_iota_group',
			'density': 'uniform',
			'mean':     0.0,
		},{ # prior_iota_subgroup
			'name':    'prior_iota_subgroup',
			'density': 'gaussian',
			'mean':     0.0,
			'std':      100 * subgroup_effect_true,
		}
	]
	# smooth_table
	smooth_table = [
		{   # smooth_iota_parent
			'name' :    'smooth_iota_parent',
			'age_id':   [0],
			'time_id':  [0],
			'fun':      fun_iota_parent
		},{ # smooth_iota_child
			'name' :    'smooth_iota_child',
			'age_id':   [0],
			'time_id':  [0],
			'fun':      fun_iota_child
		},{ # smooth_iota_group
			'name' :    'smooth_iota_group',
			'age_id':   [0],
			'time_id':  [0],
			'fun':      fun_iota_group
		},{ # smooth_iota_subgroup
			'name' :    'smooth_iota_subgroup',
			'age_id':   [0],
			'time_id':  [0],
			'fun':      fun_iota_subgroup
		}
	]
	# weight table:
	weight_table = list()
	# nslist_table
	nslist_table = dict()
	# option_table
	option_table = [
		{ 'name':'parent_node_name',      'value':'p1'},
		{ 'name':'rate_case',             'value':'iota_pos_rho_zero'},
		{ 'name':'zero_sum_child_rate',   'value':'iota'},
		{ 'name':'zero_sum_mulcov_group', 'value':'g1'},
		{ 'name':'quasi_fixed',           'value':'false'},
		{ 'name':'max_num_iter_fixed',    'value':'100'},
		{ 'name':'print_level_fixed',     'value':'0'},
		{ 'name':'tolerance_fixed',       'value':'1e-12'},
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
	}
	for subgroup in [ 'none', 's1', 's2' ] :
		for node in [ 'c1', 'c2', 'p1' ] :
			# true value of iota for this node, subgroup
			iota       = avg_integrand(node, subgroup)
			# measurement standard deviation is used during fittting
			meas_std   = iota * meas_cv
			# for this example, measurements are simulated without any noise
			meas_value = iota
			#
			row['node']       = node
			row['subgroup']   = subgroup
			row['meas_value'] = meas_value
			row['meas_std']   = meas_std
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
# ---------------------------------------------------------------------------
# Compare Fit and Truth
# ---------------------------------------------------------------------------
#
# read var table and supporting information including the fit
new              = False
connection       = dismod_at.create_connection(file_name, new)
subgroup_table   = dismod_at.get_table_dict(connection, 'subgroup')
node_table       = dismod_at.get_table_dict(connection, 'node')
var_table        = dismod_at.get_table_dict(connection, 'var')
fit_var_table    = dismod_at.get_table_dict(connection, 'fit_var')
n_var            = len(var_table)
#
# check fit results
max_abs_rel_err = 0.0
for var_id in range(n_var) :
	fit_var_value  = fit_var_table[var_id]['fit_var_value']
	var_type       = var_table[var_id]['var_type']
	name           = None
	if var_type == 'rate' :
		node_id  = var_table[var_id]['node_id']
		name     = node_table[node_id]['node_name']
	else :
		assert var_type == 'mulcov_rate_value'
		group_id        = var_table[var_id]['group_id']
		subgroup_id     = var_table[var_id]['subgroup_id']
		if subgroup_id is not None :
			name  = subgroup_table[subgroup_id]['subgroup_name']
		if group_id is not None :
			for row in subgroup_table :
				if row['group_id'] == group_id :
					name = row['group_name']
	#
	truth_var_value = {
		'p1': iota_parent_true        ,
		'c1': child_effect_true       ,
		'c2': - child_effect_true     ,
		'g1': group_effect_true       ,
		's1': subgroup_effect_true    ,
		's2': - subgroup_effect_true  ,
	}
	truth   = truth_var_value[name]
	rel_err = 1.0 - fit_var_value / truth
	# print(name, truth, fit_var_value, rel_err)
	max_abs_rel_err = max(max_abs_rel_err, abs(rel_err) )
assert max_abs_rel_err < 1e-4
# ----------------------------------------------------------------------------
print('subgroup_mulcov.py: OK')
# END PYTHON
