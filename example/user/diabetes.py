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
# The following smoothing grids are defined:
# $table
# Name          $cnext Description $cnext
# smooth_mulcov $cnext N(0, 1) and constant in (age, time)
# $tend
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
# MS_2012 $cnext market scan data for 2012 $cnext 0     $cnext null
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
# $code MS_2000$$, $code MS_2010$$ and $code MS_2012$$.
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
#
# weight table has constant value 1.0
def constant_weight_fun(a, t) :
	return 1.0
#
# covariate multiplies are gaussian with mean zero, and variance one.
# Note that there are no forward differences for covariate multipliers.
def fun_mulcov(a, t) :
	return ('prior_N(0,1)', None, None)
#
# note that the a, t values are not used for this case
def fun_rate_child(a, t) :
	return ('prior_gauss_zero', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_iota_parent(a, t) :
	return ('prior_iota_parent', 'prior_gauss_zero', 'prior_gauss_zero')
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ----------------------------------------------------------------------
	# avgint table: empty
	avgint_table = list()
	#
	# ----------------------------------------------------------------------
	# nslist_table:
	nslist_table = dict()
	# ----------------------------------------------------------------------
	# age table:
	age_list    = [ j * 5.0 for j in range(21) ]
	#
	# ----------------------------------------------------------------------
	# time table:
	time_list   = [ 1990.0 + i * 5.0 for i in range(7) ]
	#
	# ----------------------------------------------------------------------
	# integrand table:
	integrand_list = [ 'prevalence' ]
	#
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
	name    = 'constant'
	fun     = constant_weight_fun
	weight_table = [
		{ 'name':name,  'age_id':[0], 'time_id':[0], 'fun':fun }
	]
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
	]
	# smooth table
	smooth_table   = list()
	#
	# smooth_mulcov
	name           = 'smooth_mulcov'
	fun            = fun_rate_child
	smooth_table = [
		{'name':name, 'age_id':[0], 'time_id':[0], 'fun':fun }
	]
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
		{'name':'one',     'reference':0.0,  'max_difference':None } ,
		{'name':'sex',     'reference':0.0,  'max_difference':0.6  } ,
		{'name':'BMI',     'reference':27.0, 'max_difference':None } ,
		{'name':'LDI',     'reference':10.0, 'max_difference':None } ,
		{'name':'MS_2000', 'reference':0.0,  'max_difference':None } ,
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
			# beta for prevalence and MS_2012
			'covariate': 'MS_2012',
			'type':      'meas_value',
			'effected':  'prevalence',
			'smooth':    'smooth_mulcov'
		} , {
			# gamma for prevalence and one
			'covariate': 'MS_2012',
			'type':      'meas_std',
			'effected':  'prevalence',
			'smooth':    'smooth_mulcov'
		}
	]
	# ----------------------------------------------------------------------
	# rate table:
	rate_table = [
		{	'name':          'iota',
			'parent_smooth': 'smooth_rate_parent',
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
	# data table:
	data_table = list()
	# values that are the same for all data rows
	row = {
		'meas_value':  0.0,             # not used (will be simulated)
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   2000.,
		'time_upper':   2000.
	}
	# values that change between rows:
	for data_id in range( n_data ) :
		fraction         = data_id / float(n_data-1)
		age              = age_list[0] + (age_list[-1] - age_list[0])*fraction
		row['age_lower'] = age
		row['age_upper'] = age
		row['node']      = 'child_' + str( (data_id % n_children) + 1 )
		row['income']    = fraction
		row['integrand'] = integrand_list[0]
		row['meas_std']  = 1e-3
		data_table.append( copy.copy(row) )
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },
		{ 'name':'parent_node_name',       'value':'world'        },
		{ 'name':'ode_step_size',          'value':'10.0'         },
		{ 'name':'random_seed',            'value':'0'            },

		{ 'name':'quasi_fixed',            'value':'true'         },
		{ 'name':'derivative_test_fixed',  'value':'first-order'  },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-8'         },

		{ 'name':'derivative_test_random', 'value':'second-order' },
		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-8'         }
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
# -----------------------------------------------------------------------
# read database
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table        = dismod_at.get_table_dict(connection, 'var')
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
