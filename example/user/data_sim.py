#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-19 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_data_sim.py$$ $newlinech #$$
# $spell
#	init
#	covariate
#	Sincidence
#	std
#	sim
# $$
#
# $section Explanation of Simulated Data Table, data_sim$$
#
# $head Random Effects$$
# There are no random effects in this example.
#
# $head Priors$$
# The priors do not matter for this example except for the fact that
# the $cref truth_var_table$$ values for the $cref model_variables$$
# must satisfy the lower and upper limits in the corresponding priors.
#
# $head Iota$$
# The value $icode iota_true$$
# is the simulated true rate for iota.
# There is only one grid point (one $cref/model_variable/model_variables/$$)
# corresponding to $icode iota$$, hence it is constant in age and time.
#
# $head Other Rates$$
# For this example the other rates are all zero.
# This is specified by setting the
# $cref/parent_smooth_id/rate_table/parent_smooth_id/$$ and
# $cref/child_smooth_id/rate_table/child_smooth_id/$$ to null
# for the other rates.
#
# $head Covariate Multiplier$$
# There is one covariate multiplier on the covariate column $code one$$
# and the rate $code iota$$.
# This is a measurement noise covariate multiplier
# $cref/gamma/data_like/Measurement Noise Covariates/gamma_j/$$.
# The true value for this multiplier, used to simulate data, is
# called $icode gamma_true$$.
# There is only one grid point in the covariate multiplier,
# hence it is constant in age and time.
#
# $head Data$$
# There are $icode n_data$$ measurements of Sincidence and each has a standard
# deviation $icode meas_std$$ (before adding the covariate effect).
# The $cref/meas_value/data_table/meas_value/$$ do not affect (do affect)
# the values in $cref data_sim_table$$ when the
# $cref/density/data_table/density_id/$$ is
# $cref/linear/density_table/Notation/Linear/$$
# ($cref/log scaled/density_table/Notation/Log Scaled/$$).
#
# $head Data Subset$$
# Data is only simulated for
# $cref/data_id/data_subset_table/data_id/$$
# values that appear in the data_subset table.
# For this case, this includes all the
# $cref/data_id/data_table/data_id/$$ values in the data table.
#
# $head meas_noise_effect$$
# see $cref/meas_noise_effect/option_table/meas_noise_effect/$$.
#
# $head Scaling Gamma$$
# The function $code gamma_true()$$ shows on the scaling of $icode gamma$$
# depends on the value of $icode meas_noise_effect$$.
#
# $head Source Code$$
# $srcfile%
#	example/user/data_sim.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# You can changed the values below and rerun this program
iota_true          = 0.01
meas_std           = 0.001
scale_gamma_true   = 2.0
n_data             = 10
meas_noise_effect  = 'add_var_scale_log'
data_density       = 'log_gaussian'
# You can changed the values above and rerun this program
# ---------------------------------------------------------------------------
def gamma_true() :
	if meas_noise_effect == 'add_std_scale_all' :
		result = scale_gamma_true
	elif meas_noise_effect == 'add_std_scale_log' :
		result = scale_gamma_true * meas_std
	elif meas_noise_effect == 'add_var_scale_all' :
		result = scale_gamma_true
	else :
		assert meas_noise_effect == 'add_var_scale_log'
		result = scale_gamma_true * meas_std * meas_std
	return result
# ----------------------------------------------------------------------------
import math
import sys
import os
import distutils.dir_util
import copy
test_program = 'example/user/data_sim.py'
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
distutils.dir_util.mkpath('build/example/user')
os.chdir('build/example/user')
# ---------------------------------------------------------------------------
def log_density(density) :
	return density.startswith('log_') or density.startswith('cen_log_')
# ---------------------------------------------------------------------------
def delta_effect(Delta, effect) :
	if meas_noise_effect == 'add_std_scale_all' :
		delta = Delta * (1.0 + effect)
	elif meas_noise_effect == 'add_std_scale_log' :
		if log_density(data_density) :
			delta = Delta * (1.0 + effect)
		else :
			delta = Delta + effect
	elif meas_noise_effect == 'add_var_scale_all' :
		delta = Delta * math.sqrt(1.0 + effect)
	else :
		assert meas_noise_effect == 'add_var_scale_log'
		if log_density(data_density) :
			delta = Delta * math.sqrt(1.0 + effect)
		else :
			delta = math.sqrt( Delta * Delta + effect )
	return delta
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def example_db (file_name) :
	def weight_fun(a, t) :
		return 1.0
	# note that the a, t values are not used for this case
	def fun_iota(a, t) :
		return ('prior_iota', None, None)
	def fun_gamma(a, t) :
		return ('prior_gamma', None, None)
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ 0.0, 100.0 ]
	#
	# time table:
	time_list   = [ 1990.0, 2010.0 ]
	#
	# integrand table:
	integrand_table = [
		 { 'name': 'Sincidence' }
	]
	#
	# node table:
	node_table = [ { 'name':'world', 'parent':'' } ]
	#
	# weight table:
	weight_table = list()
	#
	# covariate table:
	covariate_table = [
		{'name':'one', 'reference':0.0}
	]
	#
	# mulcov table:
	mulcov_table = [
		{
			'covariate': 'one',
			'type':      'meas_noise',
			'effected':  'Sincidence',
			'smooth':    'smooth_gamma'
		}
	]
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
	row = {
		'weight':      '',
		'hold_out':     False,
		'node':        'world',
		'one':          1.0   ,
		'age_lower':    50.0,
		'age_upper':    50.0,
		'time_lower':   2000.,
		'time_upper':   2000.,
		'integrand':   'Sincidence',
		'density':      data_density,
		'meas_std':     meas_std,
		'eta':          iota_true / 100.0,
		'nu':           10
	}
	# values that change between rows:
	for data_id in range( n_data ) :
		if data_id % 2 == 0 :
			row['meas_value'] = 0.9 * iota_true
		else :
			row['meas_value'] = 1.1 * iota_true
		data_table.append( copy.copy(row) )
	#
	# ----------------------------------------------------------------------
	# prior_table
	prior_table = [
		{ # prior_iota
			'name':     'prior_iota',
			'density':  'uniform',
			'lower':    iota_true / 100.0,
			'upper':    1.0,
			'mean':     iota_true / 10.0
		},{ # prior_gamma
			'name':     'prior_gamma',
			'density':  'uniform',
			'lower':    0.0,
			'upper':    10.0 * gamma_true(),
			'mean':     gamma_true() / 10.0
		}
	]
	# ----------------------------------------------------------------------
	# smooth table
	name           = 'smooth_iota'
	fun            = fun_iota
	age_id         = 0
	time_id        = 0
	smooth_table = [
		{'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	]
	name = 'smooth_iota'
	#
	name = 'smooth_gamma'
	fun  = fun_gamma
	smooth_table.append(
		{'name':name, 'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	)
	# ----------------------------------------------------------------------
	# rate table:
	rate_table = [
		{	'name':          'iota',
			'parent_smooth': 'smooth_iota',
			'child_smooth':  None
		}
	]
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'meas_noise_effect',      'value':meas_noise_effect   },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },
		{ 'name':'parent_node_name',       'value':'world'             },
		{ 'name':'random_seed',            'value':'0'                 },

	]
	# ----------------------------------------------------------------------
	# create database
	dismod_at.create_database(
		file_name,
		age_list,
		time_list,
		integrand_table,
		node_table,
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
# Run the init command to create the var table
file_name = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
# -----------------------------------------------------------------------
# read database
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table       = dismod_at.get_table_dict(connection, 'var')
rate_table      = dismod_at.get_table_dict(connection, 'rate')
integrand_table = dismod_at.get_table_dict(connection, 'integrand')
covariate_table = dismod_at.get_table_dict(connection, 'covariate')
node_table      = dismod_at.get_table_dict(connection, 'node')
# -----------------------------------------------------------------------
# truth table:
tbl_name     = 'truth_var'
col_name     = [ 'truth_var_value' ]
col_type     = [ 'real' ]
row_list     = list()
var_id2true  = list()
for var_id in range( len(var_table) ) :
	var_info        = var_table[var_id]
	truth_var_value = None
	var_type        = var_info['var_type']
	if var_type == 'mulcov_meas_noise' :
		integrand_id  = var_info['integrand_id']
		integrand_name = integrand_table[integrand_id]['integrand_name']
		assert integrand_name == 'Sincidence'
		#
		covariate_id   = var_info['covariate_id']
		covariate_name = covariate_table[covariate_id]['covariate_name' ]
		assert( covariate_name == 'one' )
		#
		truth_var_value = gamma_true()
	else :
		assert( var_type == 'rate' )
		rate_id   = var_info['rate_id']
		rate_name = rate_table[rate_id]['rate_name']
		node_id   = var_info['node_id']
		node_name = node_table[node_id]['node_name']
		assert node_name == 'world'
		#
		truth_var_value = iota_true
	#
	var_id2true.append( truth_var_value )
	row_list.append( [ truth_var_value ] )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
connection.close()
# -----------------------------------------------------------------------
# create and check the data_sim table
dismod_at.system_command_prc([ program, file_name, 'simulate', '1' ])
#
# check results in data_sim table
new               = False
connection        = dismod_at.create_connection(file_name, new)
data_table        = dismod_at.get_table_dict(connection, 'data')
data_subset_table = dismod_at.get_table_dict(connection, 'data_subset')
data_sim_table    = dismod_at.get_table_dict(connection, 'data_sim')
#
# check that all the data_id appear in the data_subset table
for data_subset_id in range(len(data_subset_table)) :
	data_id = data_subset_table[data_subset_id]['data_id']
	assert data_id == data_subset_id
#
# check the values in the data_sim table
eps99 = 99.0 * sys.float_info.epsilon
for data_sim_id in range( len(data_sim_table) ) :
	row = data_sim_table[data_sim_id]
	simulate_index = row['simulate_index']
	data_subset_id = row['data_subset_id']
	sim_value      = row['data_sim_value']
	sim_stdcv      = row['data_sim_stdcv']
	sim_delta      = row['data_sim_delta']
	#
	# only one set of data is simulated
	assert simulate_index == 0
	assert data_subset_id == data_sim_id
	#
	data_id        = data_subset_table[data_subset_id]['data_id']
	meas_value     = data_table[data_id]['meas_value']
	eta            = data_table[data_id]['eta']
	effect         = gamma_true()
	data_delta     = delta_effect(meas_std, effect)
	#
	if not log_density(data_density) :
		assert sim_stdcv == meas_std
		assert abs( 1.0 - sim_delta / data_delta ) < eps99
	else :
		# check calculation of sim_delta from sim_stdcv
		check = delta_effect(sim_stdcv, effect)
		assert abs( 1.0 - sim_delta / check ) < eps99
		# check calculation of sigma
		sigma  = math.log(meas_value + eta + data_delta)
		sigma -= math.log(meas_value + eta)
		check  = math.log(sim_value + eta + sim_delta)
		check -= math.log(sim_value + eta)
		assert abs( 1.0 - sigma / check ) < eps99
#
# -----------------------------------------------------------------------------
print('data_sim.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
