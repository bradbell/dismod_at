#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_diabetes.py$$ $newlinech #$$
# $spell
#	mtall
#	mtspecific
#	covariate
#	covariates
#	jk
#	mulcov
#	smoothings
# $$
#
# $section An Example Fitting Simulated Diabetes Data$$
#
# $head Under Construction$$
# This example is under construction and not yet tested.
#
# $head Node Table$$
# The $cref node_table$$ only contains the
# $cref/parent/node_table/parent/$$ node US,
# and the child nodes
# Alabama, California, Massachusetts, and Wisconsin.
#
# $head Age Table$$
# The $cref age_table$$ is 0, 5, 10, ... , 100.
#
# $head Time Table$$
# The $cref time_table$$ is 1990, 1995, ... , 2020.
#
# $head Integrand Table$$
# The $cref integrand_table$$ is
# mtall, mtspecific, prevalence.
# We think of
# $cref/mtall/avg_integrand/I_i(a,t)/mtall/$$
# as informing $cref/omega/rate_table/rate_name/omega/$$,
# $cref/mtspecific/avg_integrand/I_i(a,t)/mtspecific/$$
# as informing $cref/chi/rate_table/rate_name/chi/$$,
# $cref/prevalence/avg_integrand/I_i(a,t)/prevalence/$$
# as informing $cref/iota/rate_table/rate_name/iota/$$, and
# $cref/prevalence/avg_integrand/I_i(a,t)/prevalence/$$
# as informing $cref/iota/rate_table/rate_name/iota/$$.
# We assume that $cref/rho/rate_table/rate_name/rho/$$ is zero.
#
# $head Weight Table and Grid$$
# There is one weighting, with the constant value one,
# in the $cref weight_table$$ and $cref weight_grid_table$$.
#
# $head Smooth Table and Grid$$
# The smoothings for omega are the same as for chi.
#
# $subhead smooth_mulcov$$
# This smoothing is used for the covariate multipliers.
# It has one grid point (functions are constant in age and time).
# The value has an N(0, 1) prior distribution.
#
# $subhead smooth_chi_parent$$
# This smoothing's age and time grid include every age and time point.
# The value at each grid point has a U(0, 1) distribution.
#
# $subhead smooth_chi_child$$
# This smoothing's age (time) grid has two points,
# one at the minimum age (time) and one at the maximum age (time).
# The value at each grid point has an N(0, 1) distribution.
#
# $subhead smooth_iota_parent$$
# This smoothing's time grid includes every time point.
# The age grid for this smoothing has one point age the minimum age
# and all the age points that are greater than or equal 20.
# The value at each grid point has a U(0, 0) distribution if the age
# is less than or equal 20 and U(0, 1) otherwise.
#
# $subhead smooth_iota_child$$
# This smoothing's time grid has two points,
# one at the minimum time and one at the maximum time.
# The age grid for this smoothing has three points,
# one at the minimum age, one at age 20, and one at the maximum age.
# The value at each grid point has a U(0, 0) distribution if the
# age is less than or equal 20 and N(0, 1) otherwise.
#
# $head Covariate Table$$
# The following information is placed in the $cref covariate_table$$
# $table
# Name $cnext Description $cnext reference  $cnext max_difference $rnext
# one     $cnext constant value = 1        $cnext 0     $cnext null $rnext
# sex     $cnext .5=male -.5=female        $cnext 0     $cnext 0.6  $rnext
# BMI     $cnext body mass index           $cnext 27    $cnext null $rnext
# LDI     $cnext lag distributed income    $cnext 10    $cnext null $rnext
# MS_2000 $cnext market scan data for 2000 $cnext 0     $cnext null $rnext
# MS_2010 $cnext market scan data for 2010 $cnext 0     $cnext null $rnext
# MS_2015 $cnext market scan data for 2015 $cnext 0     $cnext null
# $tend
#
# $head Covariate Multiplier Table$$
# All of the covariate multipliers use that same smoothing which corresponds
# to a constant in age and time. The value is a Gaussian with mean zero
# and variable one.
#
# $subhead Rate Value$$
# There are two rate value covariate multipliers,
# $cref/alpha_jk/avg_integrand/Rate Functions/alpha_jk/$$
# that are used to adjust
# $cref/iota/rate_table/rate_name/iota/$$.
# A separate multiplier is applied to the covariates
# $code sex$$ and $code BMI$$.
#
# $subhead Measurement Value$$
# There are three measurement value covariate multipliers
# $cref/beta_j/avg_integrand/Measurement Value Covariates/beta_j/$$
# that is used to adjust the prevalence measurements.
# A separate multiplier is applied to the covariates
# $code MS_2000$$, $code MS_2010$$ and $code MS_2015$$.
#
# $subhead Measurement Standard Deviations$$
# There is one measurement standard deviation covariate multiplier
# $cref/gamma_j/data_like/Measurement Standard Deviation Covariates/gamma_j/$$
# that is for prevalence measurements and multiplies the $code one$$ covariate.
#
# $code
# $srcfile%
#	example/user/diabetes.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import subprocess
import copy
test_program = 'example/user/diabetes.py'
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
# functions used for priors in smoothing
#
# weight table has constant value 1.0
def fun_constant_one(a, t) :
	return 1.0
#
# Note that there are no forward differences for covariate multiplier grids.
def fun_mulcov(a, t) :
	return ('prior_N(0,1)', None, None)
fun['mulcov'] = fun_mulcov
#
# priors used in smoothing for iota
def fun_iota_parent(a, t) :
	if a <= 20.0 :
		return ('prior_U(0,0)', 'prior_diff_rate', 'prior_diff_rate')
	else :
		return ('prior_U(0,1)', 'prior_diff_rate', 'prior_diff_rate')
def run_iota_child(a, t) :
	if a <= 20.0
		return ('prior_U(0,0)', 'prior_diff_rate', 'prior_diff_rate')
	else :
		return ('prior_N(0,1)', 'prior_diff_rate', 'prior_diff_rate')
#
# priors used in smoothing for chi and omega
def fun_chi_parent(a, t) :
	return ('prior_U(0,1)', 'prior_diff_rate', 'prior_diff_rate')
def fun_chi_child(a, t) :
	return ('prior_N(0,1)', 'prior_diff_rate', 'prior_diff_rate')
#
# priors used in smoothing for pini
def fun_pini_parent(a, t)
	return ('prior_U(0,1)', None, 'prior_diff_rate')
def fun_pini_child(a, t)
	return ('prior_N(0,1)', None, 'prior_diff_rate')
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ----------------------------------------------------------------------
	fun                    = dict()
	fun['constant_one']    = fun_constant_one
	fun['iota_parent']     = fun_iota_parent
	fun['iota_child']      = fun_iota_child
	fun['chi_parent']      = fun_chi_parent
	fun['chi_child']       = fun_chi_child
	fun['omega_parent']    = fun_chi_parent
	fun['omega_child']     = fun_chi_child
	fun['pini_parent']     = fun_pini_parent
	fun['pini_child']      = fun_pini_child
	#
	# ----------------------------------------------------------------------
	# nslist_table:
	nslist_table = dict()
	# ----------------------------------------------------------------------
	# age lists
	age_index                 = dict()
	age_list                  = [ j * 5.0 for j in range(21) ]
	age_index_all             = range(21)
	age_index['iota_parent']  = [ 0 , 4] + range(5, 21)
	age_index['iota_child']   = [ 0, 4, 20]
	age_index['chi_parent']   = age_index_all
	age_index['chi_child']    = [ 0, 20 ]
	age_index['omega_parent'] = age_index['chi_parent']
	age_index['omega_child']  = age_index['chi_child']
	#
	# ----------------------------------------------------------------------
	# time lists
	time_list              = [ 1990.0 + i * 5.0 for i in range(7) ]
	time_index_all         = range(7)
	time_index_rate_parent = time_index_all
	time_index_rate_child  = [0, 6]
	# ----------------------------------------------------------------------
	# integrand table:
	integrand_list = [ 'mtall', 'mtspecific', 'prevalence' ]
	# ----------------------------------------------------------------------
	# node table:
	node_table = [
		{ 'name':'US',             'parent':''   } ,
		{ 'name':'Alabama',        'parent':'US' } ,
		{ 'name':'California',     'parent':'US' } ,
		{ 'name':'Massachuesetts', 'parent':'US' } ,
	]
	#
	# ----------------------------------------------------------------------
	# weight table and weight_grid table:
	name    = 'weight_constant'
	weight_table = [ {
		'name':      'weight_constant',
		'age_id':    [0],
		'time_id':   [0],
		'fun':       fun['constant_one']
	} ]
	# ----------------------------------------------------------------------
	# prior_table, smooth_table, and smooth_grid_table
	prior_table = [
		{
			# prior_N(0,1)
			'name':     'prior_N(0,1)',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      1.0,
		} , {
			# prior_U(0,1)
			'name':     'prior_U(0,1)',
			'density':  'uniform',
			'lower':    0.0,
			'upper':    1.0,
		} , {
			# prior_diff_rate
			'name':     'prior_diff_rate',
			'density':  'log_gaussian',
			'mean':     0.0,
			'std':      0.1,
			'eta':      1e-4,
		}
	]
	# smooth table
	smooth_table   = list()
	#
	# smooth_mulcov
	name           = 'smooth_mulcov'
	fun            = fun_rate_child
	smooth_table.append( {
		'name':     'smooth_mulcov',
		'age_id':   [0],
		'time_id':  [0],
		'fun':      fun['mulcov']
	} )
	for rate in [ 'pini', 'iota', 'chi', 'omega' ] :
		#
		# smooth_rate_parent
		name = rate + '_parent'
		smooth_table.append( {
			'name':        'smooth_' + name  ,
			'age_id':       age_index[name]  ,
			'time_id':      time_rate_parent ,
			'fun':          fun[name]
		} )
		#
		# smooth_rate_child
		name = rate + '_child'
		smooth_table.append( {
			'name':        'smooth_' + name  ,
			'age_id':       age_index[name]  ,
			'time_id':      time_rate_parent ,
			'fun':          fun[name]
		} )
	#
	# no standard deviation multipliers
	for dictionary in smooth_table :
		for name in [ 'value' , 'dage', 'dtime' ] :
			key   = 'mulstd_' + name + '_prior_name'
			value = None
			dictionary[key] = value
	# -----------------------------------------------------------------------
	# covariate table:
	covariate_table = [
		{'name':'sex',     'reference':0.0,  'max_difference':0.6  } ,
		{'name':'BMI',     'reference':27.0, 'max_difference':None } ,
		{'name':'LDI',     'reference':10.0, 'max_difference':None } ,
		{'name':'MS_2000', 'reference':0.0,  'max_difference':None } ,
		{'name':'MS_2010', 'reference':0.0,  'max_difference':None } ,
		{'name':'MS_2015', 'reference':0.0,  'max_difference':None } ,
		{'name':'one',     'reference':0.0,  'max_difference':None } ,
	]
	#
	# mulcov table:
	mulcov_table = [
		{
			# alpha for iota and sex
			'covariate': 'sex',
			'type':      'rate_value',
			'effected':  'iota',
			'smooth':    'smooth_mulcov'
		} , {
			# alpha for iota and BMI
			'covariate': 'BMI',
			'type':      'rate_value',
			'effected':  'iota',
			'smooth':    'smooth_mulcov'
		} , {
			# beta for prevalence and MS_2000
			'covariate': 'MS_2000',
			'type':      'meas_value',
			'effected':  'prevalence',
			'smooth':    'smooth_mulcov'
		} , {
			# beta for prevalence and MS_2010
			'covariate': 'MS_2010',
			'type':      'meas_value',
			'effected':  'prevalence',
			'smooth':    'smooth_mulcov'
		} , {
			# beta for prevalence and MS_2015
			'covariate': 'MS_2015',
			'type':      'meas_value',
			'effected':  'prevalence',
			'smooth':    'smooth_mulcov'
		} , {
			# gamma for prevalence and one
			'covariate': 'MS_2015',
			'type':      'meas_std',
			'effected':  'prevalence',
			'smooth':    'smooth_mulcov'
		}
	]
	# ----------------------------------------------------------------------
	# rate table:
	rate_table = [
		{	'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
			'child_smooth':  'smooth_iota_child',
		} , {
			'name':          'rho',
			'parent_smooth': None,
			'child_smooth':  None,
		} , {
			'name':          'chi',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child',
		} , {
			'name':          'omega',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child',
		}
	]
	# ----------------------------------------------------------------------
	# data table, avgint_table:
	data_table   = list()
	avgint_table = list()
	#
	# for each integrand, age, time
	n_integrad = len(interand_list)
	n_age      = len(age_list)
	n_time     = len(time_list)
	for k1 in range(n_integrand * n_age * n_time) :
		i_integrand = int( iat / (n_age * n_time) )
		k2          = k1 % (n_age * n_time)
		i_age       = int( k2 / n_time)
		i_time      = k2 % n_time
		#
		age         = age_list[i_age]
		time        = time_list[i_time]
		integrand   = integrand_list[i_integrand]
		#
		# sex
		if k1 % 2 == 0 :
			sex = -0.5
		else
			sex = +0.5
		#
		# market scan
		if k1 % 3 == 0 :
			ms = (1, 0, 0)
		elif k1 % 3 == 1 :
			ms = (0, 1, 0)
		else :
			ms = (0, 0, 1)
		#
		# body mass index
		bmi = 20 + k1 % 16
		#
		#
		row = {
			'density':     'log_gaussian',
			'weight':      'weight_constant',
			'age_lower':    age,
			'age_upper':    age,
			'time_lower':   time,
			'time_upper':   time,
			'x_sex':        sex,
			'MS_2000':      ms[0],
			'x_MS_2010':    ms[1],
			'x_MS_2015':    ms[2],
			'x_BMI':        bmi,
		}
		avgint_table.append( copy.copy(row) )
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },
		{ 'name':'parent_node_name',       'value':'US'                },
		{ 'name':'ode_step_size',          'value':'10.0'              },
		{ 'name':'random_seed',            'value':'0'                 },

		{ 'name':'quasi_fixed',            'value':'true'              },
		{ 'name':'derivative_test_fixed',  'value':'first-order'       },
		{ 'name':'max_num_iter_fixed',     'value':'20'                },
		{ 'name':'print_level_fixed',      'value':'5'                 },
		{ 'name':'tolerance_fixed',        'value':'1e-8'              },

		{ 'name':'derivative_test_random', 'value':'second-order'      },
		{ 'name':'max_num_iter_random',    'value':'50'                },
		{ 'name':'print_level_random',     'value':'0'                 },
		{ 'name':'tolerance_random',       'value':'1e-8'              }
	]
	# ----------------------------------------------------------------------
	# create database
	dismod_at.create_database(
		file_name,
		age_list,
		time_list,
		integrand_list,
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
sys.exit(0)
# -----------------------------------------------------------------------
# read database
new            = False
connection     = dismod_at.create_connection(file_name, new)
var_table      = dismod_at.get_table_dict(connection, 'var')
rate_table     = dismod_at.get_table_dict(connection, 'rate')
covariate_table= dismod_at.get_table_dict(connection, 'covariate')
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
	var_type = var_info['var_type']
	if var_type == 'mulcov_rate_value' :
		rate_id   = var_info['rate_id']
		rate_name = rate_table[rate_id]['rate_name']
		if rate_name == 'iota' :
			covariate_id   = var_info['covariate_id']
			covariate_name = covariate_table[covariate_id]['covariate_name' ]
			assert( covariate_name == 'income' )
			truth_var_value = mulcov_income_iota_true
		else :
			assert( False )
	else :
		assert( var_type == 'rate' )
		rate_id   = var_info['rate_id']
		rate_name = rate_table[rate_id]['rate_name']
		node_id   = var_info['node_id']
		# node zero is the world
		if node_id == 0 and rate_name == 'iota' :
			truth_var_value = iota_parent
		else :
			truth_var_value = 0.0
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
		variables = 'both'
		cmd.append(variables)
	if command == 'fit' :
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
	row      = fit_var_table[var_id]
	fit_value  = row['fit_var_value']
	true_value = var_id2true[var_id]
	if true_value == 0.0 :
		max_error = max(abs(fit_value) , max_error)
	else :
		max_error = max( abs(fit_value / true_value - 1.0), max_error)
if max_error > 5e-2 :
	print('max_error = ', max_error)
	assert(False)
# -----------------------------------------------------------------------------
print('diabetes.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
