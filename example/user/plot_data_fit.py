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
# $begin user_plot_data_fit.py$$ $newlinech #$$
# $spell
#	Integrands
#	Sincidence
#	mtexcess
#	cv
# $$
#
# $section Case with no Children; i.e., no Random Effects$$
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
# This smoothing has one grid point; i.e., the rates are constant
# in age and time.
# There is no child node smoothing so there are no random effects
# for these rates.
# In addition, there is no parent smoothing for the other rates
# so they are zero.
# The value priors for the rate smoothing is uniform with lower limit 1e-4
# and upper limit 1.0. The mean 0.1, is only used as a starting point
# for the optimization.
# There are no difference priors because the smoothing
# has only one grid point.
#
# $head Integrands$$
# The integrands for this example are
# $cref/Sincidence/avg_integrand/Integrand, I_i(a,t)/Sincidence/$$,
# $cref/remission/avg_integrand/Integrand, I_i(a,t)/remission/$$,
# $cref/mtexcess/avg_integrand/Integrand, I_i(a,t)/mtexcess/$$, and
# $cref/prevalence/avg_integrand/Integrand, I_i(a,t)/prevalence/$$.
# The first three integrands are direct measurements of the following rates:
# $srccode%py%
integrand2rate = {
	'Sincidence':  'iota'   ,
	'remission':   'rho'    ,
	'mtexcess':    'chi'    ,
}
# %$$
# Prevalence is in the integrand table, but it has no simulated data.
# This shows what happens when there is no data for one of the
# integrands in the $cref/integrand_list/plot_data_fit/integrand_list/$$.
#
# $head Data$$
# All of the data corresponds to canada.
#
# $subhead n_data$$
# There are $icode n_data$$ data points for each of the integrands where
# $srccode%py%
n_data  = 100
# %$$
#
# $subhead max_plot$$
# This is the maximum number of data points to plot per integrand.
# The points are chosen randomly, but there order is preserved.
# $srccode%py%
max_plot = int( n_data / 2 )
# %$$
#
# $subhead Measurement Noise$$
# The data is simulated a Gaussian with mean equal to the
# corresponding $icode rate_true$$ :
# $srccode%py%
rate_true = {
	'iota'  : 1e-2 ,
	'rho'   : 1e-3 ,
	'chi'   : 1e-4 ,
}
# %$$
# and with coefficient of variation $icode meas_cv$$ :
# $srccode%py%
meas_cv = 0.2
# %$$
#
# $subhead hold_out$$
# There is also one outlier for each integrand
# (that is not simulated with the Gaussian noise described above).
# The outlier is placed in the middle of the data set for each integrand.
# The $cref/hold_out/data_table/hold_out/$$ for the outliers is set to one
# (it is zero for all the other data).
#
# $head Call to plot_data_fit$$
# $srcthisfile%0%# BEGIN call plot_data_fit%# END call plot_data_fit%1%$$
#
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
test_program = 'example/user/plot_data_fit.py'
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
		return ('prior_rate_parent', None, None)
	# ----------------------------------------------------------------------
	# age table
	age_list    = [ 0.0 , 50.0 , 100.0 ]
	#
	# time table
	time_list   = [ 1990.0, 2000.0, 2010.0, 2020 ]
	#
	# integrand table
	integrand_table = [
		{ 'name':'Sincidence' },
		{ 'name':'remission' },
		{ 'name':'mtexcess' },
		{ 'name':'prevalence'},
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
		'age_lower':    50.0,
		'age_upper':    50.0,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
	}
	# values that change between rows: (one data point for each integrand)
	for integrand_name in integrand2rate :
		true_value        = rate_true[ integrand2rate[integrand_name] ]
		meas_std          = meas_cv * true_value
		row['integrand']  = integrand_name
		row['meas_std']   = meas_std
		#
		# i
		for i in range(n_data) :
			meas_value        = random.gauss(true_value, meas_std)
			row['meas_value'] = meas_value
			#
			row['hold_out'] = int( i == int(n_data / 2) )
			if row['hold_out'] == 1 :
				# outlier
				row['meas_value'] = 10. * true_value
			#
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
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	last_time_id   = len(time_list) - 1
	smooth_table = [
		{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   [ 0 ],
			'time_id':                  [ 0 ],
			'fun':                       fun_rate_parent
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
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-10'        },

		{ 'name':'derivative_test_random', 'value':'second-order' },
		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-10'        }
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
# --------------------------------------------------------------------------
# BEGIN call plot_data_fit
database          = file_name
integrand_list    = [ 'Sincidence', 'mtexcess', 'prevalence' ]
pdf_file          = 'example.pdf'
plot_title        = 'Example Data Plot'
n_fit_dict     = dismod_at.plot_data_fit(
	database          = database,
	pdf_file          = pdf_file,
	integrand_list    = integrand_list,
	plot_title        = plot_title,
	max_plot          = max_plot,
)
assert n_fit_dict['Sincidence'] == n_data - 1
assert n_fit_dict['mtexcess'] == n_data - 1
assert n_fit_dict['prevalence'] == 0
# END call plot_data_fit
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# get variable and fit_var tables
var_table       = dismod_at.get_table_dict(connection, 'var')
node_table      = dismod_at.get_table_dict(connection, 'node')
rate_table      = dismod_at.get_table_dict(connection, 'rate')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
# 3 rates and one variable for each rate
assert len(var_table) == 3
for (var_id, row) in enumerate(var_table) :
	assert row['var_type'] == 'rate'
	#
	# node_name
	node_id   = row['node_id']
	node_name = node_table[node_id]['node_name']
	assert node_name == 'canada'
	#
	# rate_name
	rate_id   = row['rate_id']
	rate_name = rate_table[rate_id]['rate_name']
	#
	# fit_var_value
	fit_var_value = fit_var_table[var_id]['fit_var_value']
	#
	# check
	check  = rate_true[rate_name]
	err    = fit_var_value  / check - 1.0
	tol    = 3.0 * meas_cv / math.sqrt( n_data )
	assert abs(err) < tol
# -----------------------------------------------------------------------------
print(f'Plot file: build/example/user/{pdf_file}')
print('plot_data_fit.py: OK')
# END PYTHON
