# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin user_plot_rate_fit.py$$ $newlinech #$$
# $spell
#	Integrands
#	Sincidence
#	mtexcess
#	cv
# $$
#
# $section Example Plotting The Rates for a Fit$$
#
# $head Nodes$$
# There are four nodes in this example.
# The world node has one child, north_america.
# The north_america node has two children, united_states and canada.
# The $cref/parent_node/option_table/Parent Node/$$ is canada which
# does not have any children.
#
# $head Rates$$
# There is a parent smoothing the $icode iota$$, $icode rho$$
# and $icode chi$$ rates.
# There is no child node smoothing so there are no random effects
# for these rates.
# In addition, there is no parent smoothing for the other rates
# so they are zero.
# The value priors for the rate smoothing is uniform with lower limit 1e-4
# and upper limit 1.0. The mean 0.1, is only used as a starting point
# for the optimization.
# The age and time difference prior for this smoothing is
# uniform with mean zero and no upper or lower bound.
#
# $head Integrands$$
# The integrands for this example are
# $cref/Sincidence/avg_integrand/Integrand, I_i(a,t)/Sincidence/$$,
# $cref/remission/avg_integrand/Integrand, I_i(a,t)/remission/$$, and
# $cref/mtexcess/avg_integrand/Integrand, I_i(a,t)/mtexcess/$$.
# Note that these integrands are direct measurements of the following rates:
# $srccode%py%
integrand2rate = {
	'Sincidence':  'iota'   ,
	'remission':   'rho'    ,
	'mtexcess':    'chi'    ,
}
# %$$
#
# $head Data$$
# All of the data corresponds to canada.
# There is one data point for each of the integrands listed above.
# It is simulated using true value for the corresponding rate:
# $srccode%py%
def rate_true(rate_name, age, time) :
	age_fraction  = age / 100.0
	time_fraction = (time - 1980) / 40.0
	assert 0 <= age_fraction and age_fraction <= 1.0
	assert 0 <= time_fraction and time_fraction <= 1.0
	value         = age_fraction + time_fraction + 1.0
	factor        = { 'iota':1e-2 , 'rho':5e-2 , 'chi':1e-3  }
	return factor[rate_name] * value
# %$$
# Even though there is not noise in the simulated data, it is modeled as
# have the following coefficient of variation:
# $srccode%py%
meas_cv = 0.2
# %$$
#
# $head Call to plot_rate_fit$$
# $srcthisfile%0%# BEGIN call plot_rate_fit%# END call plot_rate_fit%1%$$
#
# $head Source Code$$
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import time
import sys
import os
import copy
import random
import math
test_program = 'example/user/plot_rate_fit.py'
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
#
random_seed = int( time.time() )
# ---------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def example_db (file_name) :
	def fun_rate_parent(a, t) :
		return ('prior_rate_parent', 'prior_none', 'prior_none')
	# ----------------------------------------------------------------------
	# age table
	age_list    = list( range(0, 101, 10) )
	age_index   = list( range(1, len(age_list) - 1, 1) )
	#
	# time table
	time_list   = list( range(1980, 2021, 10) )
	time_index  = list( range(1, len(time_list) - 1, 1) )
	#
	# integrand table
	integrand_table = [
		{ 'name':'Sincidence' },
		{ 'name':'remission' },
		{ 'name':'mtexcess' },
	]
	#
	# node table: world -> north_america
	#             north_america -> (united_states, canada)
	node_table = [
		{ 'name':'world',         'parent':'' },
		{ 'name':'north_america', 'parent':'world' },
		{ 'name':'united_states', 'parent':'north_america' },
		{ 'name':'canada',        'parent':'north_america' }
	]
	#
	# weight table:
	weight_table = list()
	#
	# covariate table: no covriates
	covariate_table = list()
	#
	# mulcov table
	mulcov_table = list()
	#
	# nslist_table:
	nslist_table = dict()
	#
	# avgint table:
	avgint_table = list()
	# ----------------------------------------------------------------------
	# data table: same order as list of integrands
	data_table = list()
	# values that are the same for all data rows
	row = {
		'node':        'canada',
		'subgroup':    'world',
		'density':     'gaussian',
		'weight':      '',
		'hold_out':     False,
	}
	# values that change between rows: (one data point for each integrand)
	for integrand_id in range( len(integrand_table) ) :
		integrand_name    = integrand_table[integrand_id]['name']
		rate_name         = integrand2rate[integrand_name]
		#
		for age_id in age_index :
			for time_id in time_index :
				age        = age_list[age_id]
				time       = time_list[time_id]
				true_value = rate_true(rate_name, age, time)
				meas_std          = meas_cv * true_value
				row['age_lower']  = age
				row['age_upper']  = age
				row['time_lower'] = time
				row['time_upper'] = time
				row['integrand']  = integrand_name
				row['meas_value'] = true_value
				row['meas_std']   = true_value * 0.2
				data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{	# prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    1e-4,
			'upper':    1.0,
			'mean':     0.1,
		},{ # prior_none
			'name':     'prior_none',
			'density':  'uniform',
			'mean':     0.0,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	last_time_id   = len(time_list) - 1
	smooth_table = [
		{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   age_index,
			'time_id':                  time_index,
			'fun':                      fun_rate_parent
		}
	]
	# ----------------------------------------------------------------------
	# rate table
	rate_table = [
		{
			'name':          'iota',
			'parent_smooth': 'smooth_rate_parent',
		},{
			'name':          'rho',
			'parent_smooth': 'smooth_rate_parent',
		},{
			'name':          'chi',
			'parent_smooth': 'smooth_rate_parent',
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'parent_node_name',       'value':'canada'       },
		{ 'name':'random_seed',            'value':random_seed    },
		{ 'name':'rate_case',              'value':'iota_pos_rho_pos' },

		{ 'name':'quasi_fixed',            'value':'false'        },
		{ 'name':'derivative_test_fixed',  'value':'first-order'  },
		{ 'name':'max_num_iter_fixed',     'value':'30'           },
		{ 'name':'print_level_fixed',      'value':'5'            },
		{ 'name':'tolerance_fixed',        'value':'1e-12'        },

		{ 'name':'derivative_test_random', 'value':'second-order' },
		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-12'        }
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
	n_smooth  = len( smooth_table )
# ===========================================================================
file_name  = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
dismod_at.system_command_prc(
	[ program, file_name, 'sample', 'asymptotic', 'both', '20' ]
)
# --------------------------------------------------------------------------
# BEGIN call plot_rate_fit
database   = file_name
rate_set   = { 'iota', 'chi' }
pdf_file   = 'example.pdf'
plot_title = 'Example Rate Plot'
plot_set   = dismod_at.plot_rate_fit(database, pdf_file, plot_title, rate_set)
assert plot_set == rate_set
# END call plot_rate_fit
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# get variable and fit_var tables
age_table       = dismod_at.get_table_dict(connection, 'age')
time_table      = dismod_at.get_table_dict(connection, 'time')
var_table       = dismod_at.get_table_dict(connection, 'var')
node_table      = dismod_at.get_table_dict(connection, 'node')
rate_table      = dismod_at.get_table_dict(connection, 'rate')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
# 3 rates and n_grid values per rate
n_grid = ( len(age_table) - 2) * ( len(time_table) - 2 )
assert len(var_table) == 3 * n_grid
for (var_id, row) in enumerate(var_table) :
	assert row['var_type'] == 'rate'
	#
	# age_id, time_id, node_id, rate_id
	age_id    = row['age_id']
	time_id   = row['time_id']
	node_id   = row['node_id']
	rate_id   = row['rate_id']
	#
	# age, time, node_name, rate_name
	age       = age_table[age_id]['age']
	time      = time_table[time_id]['time']
	node_name = node_table[node_id]['node_name']
	rate_name = rate_table[rate_id]['rate_name']
	assert node_name == 'canada'
	#
	# rate_name
	#
	# fit_var_value
	fit_var_value = fit_var_table[var_id]['fit_var_value']
	#
	# check
	check  = rate_true(rate_name, age, time)
	err    = fit_var_value  / check - 1.0
	assert abs(err) < 1e-6
# -----------------------------------------------------------------------------
# Run plot at unix command line
os.chdir('../../..')
program  = 'bin/dismodat.py'
database = f'build/example/user/{database}'
pdf_file = f'build/example/user/{pdf_file}'
rate_set = 'iota chi'
dismod_at.system_command_prc( [
	program, database, 'plot_rate_fit', pdf_file, plot_title, rate_set
] )
# -----------------------------------------------------------------------------
print(f'Plot file: {pdf_file}')
print('plot_rate_fit.py: OK')
# END PYTHON
