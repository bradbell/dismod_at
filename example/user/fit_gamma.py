#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_fit_gamma.py$$ $newlinech #$$
# $spell
#	init
#	covariate
#	Sincidence
#	std
# $$
#
# $section Fitting Data Standard Deviations, Gamma$$
#
# $head Random Effects$$
# There are no random effects in this example.
#
# $head Iota$$
# The value $icode iota_true$$
# is the simulated true rate for iota.
# The prior for iota is uniform prior with lower limit
# $icode iota_true / 100$$ and upper limit one.
# The mean for the prior is $icode iota_true / 10$$
# (this is only used as a starting point for the optimization).
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
# (which has the constant value one) and the rate $code iota$$.
# This is a measurement standard deviation covariate multiplier
# $cref/gamma/data_like/Measurement Standard Deviation Covariates/gamma_j/$$.
# The prior for this multiplier is a uniform on the interval from zero
# to $codei%10 * %gamma_true%$$.
# The true value for this multiplier, used to simulate data, is
# called $icode gamma_true$$.
# The mean for the prior is $codei%gamma_true% / 10%$$
# (this is only used as a starting point for the optimization).
# Note that there is only one grid point in the covariate multiplier,
# hence it is constant in age and time.
#
# $head Data$$
# There are $icode n_data$$ measurements of Sincidence and each has a standard
# deviation $icode data_std$$ (before adding $icode gamma$$).
#
# $head meas_std_effect$$
# see $cref/meas_std_effect/option_table/meas_std_effect/$$.
#
# $srcfile%
#	example/user/fit_gamma.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# values used to simulate data
iota_true          = 0.01
gamma_true         = 2.0
n_data             = 1000
data_std           = 0.001
meas_std_effect    = 'add_std'
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
test_program = 'example/user/fit_gamma.py'
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
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def weight_fun(a, t) :
	return 1.0
# note that the a, t values are not used for this case
def fun_iota(a, t) :
	return ('prior_iota', None, None)
def fun_gamma(a, t) :
	return ('prior_gamma', None, None)
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ 0.0, 100.0 ]
	#
	# time table:
	time_list   = [ 1990.0, 2200.0 ]
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
	# The constant function 1.0, note any valid age and time id would work
	name    = 'constant'
	fun     = weight_fun
	age_id  = 0
	time_id = 0
	weight_table = [
		{ 'name':name,  'age_id':[age_id], 'time_id':[time_id], 'fun':fun }
	]
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
			'type':      'meas_std',
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
		'meas_value':  0.0,             # not used (will be simulated)
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   2000.,
		'time_upper':   2000.,
		'integrand':   'Sincidence',
		'meas_std':     data_std,
		'node':        'world',
		'one':          1.0
	}
	# values that change between rows:
	for data_id in range( n_data ) :
		fraction         = data_id / float(n_data-1)
		age              = age_list[0] + (age_list[-1] - age_list[0])*fraction
		row['age_lower'] = age
		row['age_upper'] = age
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
			'upper':    10.0 * gamma_true,
			'mean':     gamma_true / 10.0
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
		{ 'name':'meas_std_effect',        'value':meas_std_effect     },
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },
		{ 'name':'parent_node_name',       'value':'world'             },
		{ 'name':'random_seed',            'value':'0'                 },
		{ 'name':'zero_sum_random',        'value':'iota'              },

		{ 'name':'quasi_fixed',            'value':'false'             },
		{ 'name':'derivative_test_fixed',  'value':'second-order'      },
		{ 'name':'max_num_iter_fixed',     'value':'100'               },
		{ 'name':'print_level_fixed',      'value':'0'                 },
		{ 'name':'tolerance_fixed',        'value':'1e-10'             }
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
file_name      = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
cmd            = [ program, file_name, 'init' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at init command failed')
# -----------------------------------------------------------------------
# read database
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table       = dismod_at.get_table_dict(connection, 'var')
rate_table      = dismod_at.get_table_dict(connection, 'rate')
integrand_table = dismod_at.get_table_dict(connection, 'integrand')
covariate_table = dismod_at.get_table_dict(connection, 'covariate')
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
	if var_type == 'mulcov_meas_std' :
		integrand_id  = var_info['integrand_id']
		integrand_name = integrand_table[integrand_id]['integrand_name']
		if integrand_name == 'Sincidence' :
			covariate_id   = var_info['covariate_id']
			covariate_name = covariate_table[covariate_id]['covariate_name' ]
			assert( covariate_name == 'one' )
			truth_var_value = gamma_true
		else :
			assert( False )
	else :
		assert( var_type == 'rate' )
		rate_id   = var_info['rate_id']
		rate_name = rate_table[rate_id]['rate_name']
		node_id   = var_info['node_id']
		# node zero is the world
		if node_id == 0 and rate_name == 'iota' :
			truth_var_value = iota_true
		else :
			assert( False )
	var_id2true.append( truth_var_value )
	row_list.append( [ truth_var_value ] )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
connection.close()
# -----------------------------------------------------------------------
# Simulate and then fit the data
for command in [ 'simulate', 'fit' ] :
	cmd = [ program, file_name, command ]
	if command == 'simulate' :
		number_simulate = '1'
		cmd.append(number_simulate)
	if command == 'fit' :
		variables = 'fixed'
		cmd.append(variables)
		simulate_index = '0';
		cmd.append(simulate_index)
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# check fit results
new          = False
connection   = dismod_at.create_connection(file_name, new)
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
connection.close()
#
max_error    = 0.0
for var_id in range( len(var_table) ) :
	row        = fit_var_table[var_id]
	fit_value  = row['fit_var_value']
	true_value = var_id2true[var_id]
	assert( true_value != 0.0 )
	# remove # at start of next line to see relative error values
	# print( fit_value / true_value - 1.0 )
	max_error = max( abs(fit_value / true_value - 1.0), max_error)
if max_error > 1e-1 :
	print('max_error = ', max_error)
	assert(False)
# -----------------------------------------------------------------------------
print('fit_gamma.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
