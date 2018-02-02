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
#	mtother
#	mtspecific
#	covariate
#	covariates
#	jk
#	mulcov
#	smoothings
#	pini
# $$
#
# $section An Example Fitting Simulated Diabetes Data$$
#
# $head Under Construction$$
# This example is under construction.
#
# $head Node Table$$
# The $cref node_table$$ only contains the
# $cref/parent/node_table/parent/$$ node US,
# and the child nodes
# Alabama, California, Massachusetts, and Wisconsin.
#
# $head Age Table$$
# The $cref age_table$$ is uniformly spaced starting at age zero,
# ending at age 100.
#
# $head Time Table$$
# The $cref time_table$$ is uniformly spaced starting at time 1990
# and ending at time 2020.
#
# $head omega$$
# We are constraint $cref/omega/rate_table/rate_name/omega/$$ to have
# the value used during simulation of the data.
#
# $head Integrand Table$$
# The $cref integrand_table$$ is
# mtspecific, prevalence.
# We think of
# $cref/mtspecific/avg_integrand/I_i(a,t)/mtspecific/$$
# as informing $cref/chi/rate_table/rate_name/chi/$$,
# $cref/prevalence/avg_integrand/I_i(a,t)/prevalence/$$
# as informing $cref/iota/rate_table/rate_name/iota/$$, and
# $cref/prevalence/avg_integrand/I_i(a,t)/prevalence/$$ at age zero
# as informing $cref/pini/rate_table/rate_name/pini/$$.
# We assume that $cref/rho/rate_table/rate_name/rho/$$ is zero.
#
# $head Weight Table and Grid$$
# There is one weighting, with the constant value one,
# in the $cref weight_table$$ and $cref weight_grid_table$$.
#
# $head Covariate Table$$
# The following information is placed in the $cref covariate_table$$
# $table
# Name $cnext Description $cnext reference  $cnext max_difference $rnext
# one     $cnext constant value = 1        $cnext 0     $cnext null $rnext
# sex     $cnext .5=male -.5=female        $cnext 0     $cnext 0.6  $rnext
# BMI     $cnext body mass index           $cnext 27    $cnext null $rnext
# MS_2000 $cnext market scan data for 2000 $cnext 0     $cnext null $rnext
# MS_2010 $cnext market scan data for 2010 $cnext 0     $cnext null $rnext
# MS_2015 $cnext market scan data for 2015 $cnext 0     $cnext null
# $tend
#
# $head Covariate Multiplier Table$$
# All of the covariate multipliers use that same smoothing which corresponds
# to a constant in age and time. The value is a Gaussian with mean zero
# and variable one.
# (The covariates multipliers are currently set to zero.)
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
# $head Truth$$
# The start_age is 0, The end_age is 100,
# the start_time is 1990, and the end_time is 2020.
# The values in the $cref truth_var_table$$ are generated using bilinear
# interpolation of the log of the specified values at
# (start_age, start_time),
# (start_age, end_time),
# (end_age, start_time)
# and (end_age, end_time).
#
# $head Predict$$
# The $cref predict_command$$ is used to compute the
# $cref/avg_integrand/predict_table/avg_integrand/$$ corresponding to the
# true values for the variables.
# This is then used to create a version of the $cref data_table$$
# with no noise, but modeled with a standard deviation corresponding
# to a coefficient of variation.
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
import math
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
# model parameters that can be changed
time_grid = { 'start':1990.0, 'end': 2020, 'number': 2  }
age_grid  = { 'start':0.0,    'end': 100,  'number': 8  }
#
fit_with_noise_in_data = True
noise_cv               = 0.1
# ------------------------------------------------------------------------
#
def log_bilinear(grid_value, a, t) :
	age_start = age_grid['start']
	#
	# denominator
	da  = age_grid['end'] - age_start
	dt  = time_grid['end'] - time_grid['start']
	den = da * dt;
	num = 0.0
	#
	# value at start age and start time
	da   = age_grid['end'] - a
	dt   = time_grid['end'] - t
	num += math.log( grid_value['start_age, start_time'] ) * da * dt
	#
	# value at start age and end time
	da   = age_grid['end'] - a
	dt   = t - time_grid['start']
	num += math.log( grid_value['start_age, end_time'] ) * da * dt
	#
	# value at end age and start time
	da   = a - age_start
	dt   = time_grid['end'] - t
	num += math.log( grid_value['end_age, start_time'] ) * da * dt
	#
	# value at end age and end time
	da   = a - age_start
	dt   = t - time_grid['start']
	num += math.log( grid_value['end_age, end_time'] ) * da * dt
	#
	return math.exp( num / den )
#
def true_rate(node, rate, a, t) :
	# default
	grid_value = dict()
	# -------------------------------------------------------------------------
	if rate == 'pini' :
		if node == 'US' :
			grid_value['start_age, start_time'] = 1e-2
			grid_value['start_age, end_time']   = 1e-2
		elif node in ['Alabama', 'Wisconsin'] :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
		elif node in [ 'California', 'Massachusetts' ] :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
		else :
			assert False
		#
		# pini is constant in age
		grid_value['end_age, start_time'] = grid_value['start_age, start_time']
		grid_value['end_age, end_time']   = grid_value['start_age, end_time']
		#
		ret = log_bilinear(grid_value, a, t)
		if node != 'US' :
			ret = math.log(ret)
	# -------------------------------------------------------------------------
	elif rate == 'iota' :
		if node == 'US' :
			grid_value['start_age, start_time'] = 2e-3
			grid_value['start_age, end_time']   = 4e-3
			grid_value['end_age, start_time']   = 2e-2
			grid_value['end_age, end_time']     = 4e-2
		elif node in ['Alabama', 'Wisconsin'] :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
			grid_value['end_age, start_time']   = 1.0
			grid_value['end_age, end_time']     = 1.0
		elif node in [ 'California', 'Massachusetts' ] :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
			grid_value['end_age, start_time']   = 1.0
			grid_value['end_age, end_time']     = 1.0
		else :
			assert False
		#
		ret = log_bilinear(grid_value, a, t)
		if node != 'US' :
			ret = math.log(ret)
	# -------------------------------------------------------------------------
	elif rate == 'omega' :
		if node == 'US' :
			grid_value['start_age, start_time'] = 3e-3
			grid_value['start_age, end_time']   = 2e-3
			grid_value['end_age, start_time']   = 3e-1
			grid_value['end_age, end_time']     = 2e-1
		elif node in ['Alabama', 'Wisconsin'] :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
			grid_value['end_age, start_time']   = 1.0
			grid_value['end_age, end_time']     = 1.0
		elif node in [ 'California', 'Massachusetts' ] :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
			grid_value['end_age, start_time']   = 1.0
			grid_value['end_age, end_time']     = 1.0
		else :
			assert False
		#
		ret = log_bilinear(grid_value, a, t)
		if node != 'US' :
			ret = math.log(ret)
	# -------------------------------------------------------------------------
	elif rate == 'chi' :
		if node == 'US' :
			grid_value['start_age, start_time'] = 4e-3
			grid_value['start_age, end_time']   = 2e-3
			grid_value['end_age, start_time']   = 1e-1
			grid_value['end_age, end_time']     = 2e-1
		elif node in ['Alabama', 'Wisconsin'] :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
			grid_value['end_age, start_time']   = 1.0
			grid_value['end_age, end_time']     = 1.0
		elif node in [ 'California', 'Massachusetts' ] :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
			grid_value['end_age, start_time']   = 1.0
			grid_value['end_age, end_time']     = 1.0
		else :
			assert False
		#
		ret = log_bilinear(grid_value, a, t)
		if node != 'US' :
			ret = math.log(ret)
	# -------------------------------------------------------------------------
	else :
		assert False
	#
	return ret
# ------------------------------------------------------------------------
def example_db (file_name) :
	# ------------------------------------------------------------------------
	# functions used for priors in smoothing
	#
	# weight table has constant value 1.0
	def fun_constant_one(a, t) :
		return 1.0
	#
	# Note that there are no forward differences for covariate multiplier grids.
	def fun_mulcov_meas_std(a, t) :
		return('prior_U(0,0)', None, None)
	def fun_mulcov_meas_value(a, t) :
		return('prior_U(0,0)', None, None)
	def fun_mulcov_rate_value(a, t) :
		return('prior_U(0,0)', None, None)
	#
	# priors used in smoothing for iota
	def fun_iota_parent(a, t) :
		return ('prior_U(1e-8,1)', 'prior_diff_iota_age', 'prior_diff_time')
	def fun_iota_child(a, t) :
		return ('prior_N(0,1)', 'prior_diff_iota_age', 'prior_diff_time')
	#
	# priors used in smoothing for chi
	def fun_chi_parent(a, t) :
		return ('prior_U(1e-8,1)', 'prior_diff_chi_age', 'prior_diff_time')
	def fun_chi_child(a, t) :
		return ('prior_N(0,1)', 'prior_diff_chi_age', 'prior_diff_time')
	#
	# use const_value to constrain omega to true value
	def fun_omega(node) :
		def fun_omega_node(a, t) :
			true_value = true_rate(node, 'omega', a, t)
			return (true_value, 'prior_U(-inf,inf)', 'prior_U(-inf,inf)')
		return fun_omega_node
	#
	# priors used in smoothing for pini
	def fun_pini_parent(a, t) :
		return ('prior_U(0,1)', None, 'prior_diff_time')
	def fun_pini_child(a, t) :
		return ('prior_N(0,1)', None, 'prior_diff_time')
	# ----------------------------------------------------------------------
	fun                       = dict()
	fun['mulcov_rate_value']  = fun_mulcov_rate_value
	fun['mulcov_meas_value']  = fun_mulcov_meas_value
	fun['mulcov_meas_std']    = fun_mulcov_meas_std
	fun['constant_one']       = fun_constant_one
	fun['iota_parent']        = fun_iota_parent
	fun['iota_child']         = fun_iota_child
	fun['chi_parent']         = fun_chi_parent
	fun['chi_child']          = fun_chi_child
	fun['pini_parent']        = fun_pini_parent
	fun['pini_child']         = fun_pini_child
	# ----------------------------------------------------------------------
	d_age =  ( age_grid['end'] -  age_grid['start'])/( age_grid['number'] - 1)
	d_time = (time_grid['end'] - time_grid['start'])/(time_grid['number'] - 1)
	# ----------------------------------------------------------------------
	# nslist_table:
	nslist_table = dict()
	# ----------------------------------------------------------------------
	# age lists
	start                     = age_grid['start']
	number                    = age_grid['number']
	age_index                 = dict()
	age_list                  = [ start + j * d_age for j in range(number) ]
	age_list                  = sorted( age_list )
	age_index_all             = range(number)
	age_index['iota_parent']  = age_index_all
	age_index['chi_parent']   = age_index_all
	age_index['omega_parent'] = age_index_all
	age_index['pini_parent']  = [ 0 ]
	#
	age_index['iota_child']   = [ 0, number-1 ]
	age_index['chi_child']    = [ 0, number-1 ]
	age_index['omega_child']  = [ 0, number-1 ]
	age_index['pini_child']   = [ 0 ]
	#
	# ----------------------------------------------------------------------
	# time lists
	start                  = time_grid['start']
	number                 = time_grid['number']
	time_list              = [ start + i * d_time for i in range(number) ]
	time_index_all         = range(number)
	time_index_rate_parent = time_index_all
	time_index_rate_child  = [0, number-1]
	# ----------------------------------------------------------------------
	# integrand table:
	integrand_list = [ 'mtspecific', 'prevalence' ]
	# ----------------------------------------------------------------------
	# node table:
	node_table = [
		{ 'name':'US',             'parent':''   } ,
		{ 'name':'Alabama',        'parent':'US' } ,
		{ 'name':'California',     'parent':'US' } ,
		{ 'name':'Massachusetts',  'parent':'US' } ,
		{ 'name':'Wisconsin',      'parent':'US' } ,
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
			# prior_N_+(0,1)
			'name':     'prior_N_+(0,1)',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      1.0,
			'lower':    0.0,
		} , {
			# prior_U(0,1)
			'name':     'prior_U(0,1)',
			'density':  'uniform',
			'mean':     0.1,     # only affects initial optimizer value
			'lower':    0.0,
			'upper':    1.0,
		} , {
			# prior_U(1e-8,1)
			'name':     'prior_U(1e-8,1)',
			'density':  'uniform',
			'mean':     0.1,     # only affects initial optimizer value
			'lower':    1e-8,
			'upper':    1.0,
		} , {
			# prior_U(0,0)
			'name':     'prior_U(0,0)',
			'density':  'uniform',
			'mean':     0.0,
			'lower':    0.0,
			'upper':    0.0,
		} , {
			# prior_U(-inf,inf)
			'name':     'prior_U(-inf,inf)',
			'density':  'uniform',
			'mean':     0.0,
		} , {
			# prior_U(1e-8,1e-8)
			'name':     'prior_U(1e-8,1e-8)',
			'density':  'uniform',
			'mean':     1e-8,
			'lower':    1e-8,
			'upper':    1e-8,
		} , {
			# prior_diff_iota_age
			'name':     'prior_diff_iota_age',
			'density':  'log_gaussian',
			'mean':     0.0,
			'std':      0.3,
			'eta':      1e-4,
		} , {
			# prior_diff_chi_age
			'name':     'prior_diff_chi_age',
			'density':  'log_gaussian',
			'mean':     0.0,
			'std':      0.3,
			'eta':      1e-4,
		} , {
			# prior_diff_time
			'name':     'prior_diff_time',
			'density':  'log_gaussian',
			'mean':     0.0,
			'std':      0.5,
			'eta':      1e-4,
		}
	]
	# smooth table
	smooth_table   = list()
	#
	# smooth_mulcov
	for cov_type in [ 'rate_value', 'meas_value', 'meas_std' ] :
		var_type = 'mulcov_' + cov_type
		smooth_table.append( {
			'name':     'smooth_' + var_type,
			'age_id':   [0],
			'time_id':  [0],
			'fun':      fun[var_type]
		} )
	for rate in [ 'pini', 'iota', 'chi' ] :
		#
		# smooth_rate_parent
		name = rate + '_parent'
		smooth_table.append( {
			'name':        'smooth_' + name  ,
			'age_id':       age_index[name]  ,
			'time_id':      time_index_rate_parent ,
			'fun':          fun[name]
		} )
		#
		# smooth_rate_child
		name = rate + '_child'
		smooth_table.append( {
			'name':        'smooth_' + name  ,
			'age_id':       age_index[name]  ,
			'time_id':      time_index_rate_parent ,
			'fun':          fun[name]
		} )
	nslist_table['nslist_omega_child'] = list()
	for node_id in range( len(node_table) ) :
		node = node_table[node_id]['name']
		name = 'smooth_omega_' + node
		smoothing = { 'name': name }
		if node == 'US' :
			smoothing['age_id']  = age_index['omega_parent']
			smoothing['time_id'] = time_index_rate_parent
		else :
			smoothing['age_id']  = age_index['omega_child']
			smoothing['time_id'] = time_index_rate_child
			nslist_table['nslist_omega_child'].append( (node, name) )
		#
		if node == 'US' :
			smoothing['fun'] = fun_omega('US')
		elif node == 'Alabama' :
			smoothing['fun'] = fun_omega('Alabama')
		elif node == 'California' :
			smoothing['fun'] = fun_omega('California')
		elif node == 'Massachusetts' :
			smoothing['fun'] = fun_omega('Massachusetts')
		elif node == 'Wisconsin' :
			smoothing['fun'] = fun_omega('Wisconsin')
		else :
			assert False
		smooth_table.append( smoothing )
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
			'smooth':    'smooth_mulcov_rate_value'
		} , {
			# alpha for iota and BMI
			'covariate': 'BMI',
			'type':      'rate_value',
			'effected':  'iota',
			'smooth':    'smooth_mulcov_rate_value'
		} , {
			# beta for prevalence and MS_2000
			'covariate': 'MS_2000',
			'type':      'meas_value',
			'effected':  'prevalence',
			'smooth':    'smooth_mulcov_meas_value'
		} , {
			# beta for prevalence and MS_2010
			'covariate': 'MS_2010',
			'type':      'meas_value',
			'effected':  'prevalence',
			'smooth':    'smooth_mulcov_meas_value'
		} , {
			# beta for prevalence and MS_2015
			'covariate': 'MS_2015',
			'type':      'meas_value',
			'effected':  'prevalence',
			'smooth':    'smooth_mulcov_meas_value'
		} , {
			# gamma for prevalence and one
			'covariate': 'one',
			'type':      'meas_std',
			'effected':  'prevalence',
			'smooth':    'smooth_mulcov_meas_std'
		}
	]
	# ----------------------------------------------------------------------
	# rate table:
	rate_table = [
		{	'name':          'pini',
			'parent_smooth': 'smooth_pini_parent',
			'child_smooth':  'smooth_pini_child',
			'child_nslist':  None,
		} , {
			'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
			'child_smooth':  'smooth_iota_child',
			'child_nslist':  None,
		} , {
			'name':          'rho',
			'parent_smooth': None,
			'child_smooth':  None,
		} , {
			'name':          'chi',
			'parent_smooth': 'smooth_chi_parent',
			'child_smooth':  'smooth_chi_child',
			'child_nslist':  None,
		} , {
			'name':          'omega',
			'parent_smooth': 'smooth_omega_US',
			'child_smooth':  None,
			'child_nslist':  'nslist_omega_child',
		}
	]
	# ----------------------------------------------------------------------
	# data table, avgint_table:
	data_table   = list()
	avgint_table = list()
	#
	# for each integrand, age, time, node
	n_integrand = len(integrand_list)
	n_age       = len(age_list)
	n_time      = len(time_list)
	n_node      = len(node_table)
	if fit_with_noise_in_data :
		n_repeat = 3
	else :
		n_repeat = 1
	for k1 in range(n_integrand * n_age * n_time * n_node * n_repeat) :
		den         = n_age * n_time * n_node * n_repeat
		i_integrand = int( k1 / den )
		k2          =      k1 % den
		#
		den         = n_time * n_node * n_repeat
		i_age       = int( k2 / den )
		k3          =      k2 % den
		#
		den         = n_node * n_repeat
		i_time      = int( k3 / den )
		k4          =      k3 % den
		#
		den         = n_repeat
		i_node      = int( k4 / den )
		#
		age         = age_list[i_age]
		time        = time_list[i_time]
		integrand   = integrand_list[i_integrand]
		node        = node_table[i_node]['name']
		#
		# sex
		if k1 % 2 == 0 :
			sex = -0.5
		else :
			sex = +0.5
		#
		# market scan
		if k1 % 4 == 0 :
			ms = (0, 0, 0)
		elif k1 % 4 == 1 :
			ms = (1, 0, 0)
		elif k1 % 4 == 2 :
			ms = (0, 1, 0)
		else :
			ms = (0, 0, 1)
		#
		# body mass index
		bmi = 20 + k1 % 16
		#
		row = {
			'node':        node,
			'integrand':   integrand,
			'density':     'log_gaussian',
			'weight':      'weight_constant',
			'age_lower':    age,
			'age_upper':    age,
			'time_lower':   time,
			'time_upper':   time,
			'sex':          sex,
			'MS_2000':      ms[0],
			'MS_2010':      ms[1],
			'MS_2015':      ms[2],
			'BMI':          bmi,
			'one':          1,
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
		{ 'name':'max_num_iter_fixed',     'value':'80'                },
		{ 'name':'print_level_fixed',      'value':'5'                 },
		{ 'name':'tolerance_fixed',        'value':'1e-2'              },

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
# -----------------------------------------------------------------------
def create_truth_var_table() :
	new             = False
	connection      = dismod_at.create_connection(file_name, new)
	var_table       = dismod_at.get_table_dict(connection, 'var')
	rate_table      = dismod_at.get_table_dict(connection, 'rate')
	covariate_table = dismod_at.get_table_dict(connection, 'covariate')
	integrand_table = dismod_at.get_table_dict(connection, 'integrand')
	node_table      = dismod_at.get_table_dict(connection, 'node')
	time_table      = dismod_at.get_table_dict(connection, 'time')
	age_table       = dismod_at.get_table_dict(connection, 'age')
	# -------------------------------------------------------------------------
	# create truth table
	tbl_name     = 'truth_var'
	col_name     = [ 'truth_var_value' ]
	col_type     = [ 'real' ]
	row_list     = list()
	for var_id in range( len(var_table) ) :
		row          = var_table[var_id]
		var_type     = row['var_type']
		age          = age_table[ row['age_id'] ] ['age']
		time         = time_table[ row['time_id'] ] ['time']
		if var_type.startswith('mulcov_') :
			covariate = covariate_table[row['covariate_id' ]]['covariate_name']
		#
		value = 0.0
		if var_type == 'rate' :
			node  = node_table[ row['node_id'] ] ['node_name']
			rate  = rate_table[ row['rate_id'] ] ['rate_name']
			value = true_rate(node, rate, age, time)
		#
		row_list.append( [ value ] )
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	connection.close()
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
# create truth_var table
create_truth_var_table()
# -----------------------------------------------------------------------------
# create predict table
cmd            = [ program, file_name, 'predict', 'truth_var' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at predict command failed')
#
# -----------------------------------------------------------------------------
# add data to data table
new             = False
connection      = dismod_at.create_connection(file_name, new)
predict_table   = dismod_at.get_table_dict(connection, 'predict')
avgint_table    = dismod_at.get_table_dict(connection, 'avgint')
density_table   = dismod_at.get_table_dict(connection, 'density')
#
# get column names and types for data table
tbl_name             = 'data'
(col_name, col_type) = dismod_at.get_name_type(connection, tbl_name)
#
# remove the primary key from col_name and col_type
assert col_name[0] == 'data_id'
del col_name[0]
del col_type[0]
#
# list of row values to place in data table
row_list = list()
#
# density_id for log_gaussian
density_id = None
for i in range( len( density_table ) ) :
	if density_table[i]['density_name'] == 'log_gaussian' :
		density_id = i
#
for predict_id in range( len(predict_table) ) :
	#
	# get prediction for average integrand
	row = predict_table[predict_id]
	assert row['sample_index'] == 0
	assert row['avgint_id']    == predict_id
	avg_integrand = row['avg_integrand']
	#
	# initial row using information in avgint table
	row = copy.copy( avgint_table[predict_id] )
	#
	# add information, that is not in avgint_table, to row
	eta        = 1e-7                     # a very small eta
	meas_value = avg_integrand            # no noise version of meas_value
	meas_std   = noise_cv * avg_integrand # noise in simulated data
	row['density_id'] = density_id
	row['hold_out']   = 0
	row['meas_std']   = meas_std
	row['meas_value'] = meas_value
	row['eta']        = eta
	row['nu']         = None
	#
	# values in same order as col_name
	row_value = list()
	for key in col_name :
		if key != 'data_id' :
			row_value.append( row[key] )
	#
	row_list.append( row_value )
#
# drop the old data table
command = 'DROP TABLE data'
dismod_at.sql_command(connection, command)
#
# create the new data table
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list )
# -----------------------------------------------------------------------------
# re-initailize to get data_subset table to correspond to new data
file_name      = 'example.db'
program        = '../../devel/dismod_at'
cmd            = [ program, file_name, 'init' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at init command failed')
#
# Initializing erases the truth_var table.
# Create a new version of truth_var table that will correspond to fit
create_truth_var_table()
#
# drop the start_var table created by init
new             = False
connection      = dismod_at.create_connection(file_name, new)
command = 'DROP TABLE start_var'
dismod_at.sql_command(connection, command)
#
# create new start_var table
rate_table      = dismod_at.get_table_dict(connection, 'rate')
var_table       = dismod_at.get_table_dict(connection, 'var')
truth_var_table = dismod_at.get_table_dict(connection, 'truth_var')
tbl_name        = 'start_var'
col_name        = [ 'start_var_value' ]
col_type        = [ 'real' ]
row_list        = list()
for var_id in range( len(var_table) ) :
	truth_var_value  = truth_var_table[var_id]['truth_var_value']
	var_type         = var_table[var_id]['var_type']
	rate_id          = var_table[var_id]['rate_id']
	start_var_value  = truth_var_value
	if var_type == 'rate' :
			if rate_table[rate_id]['rate_name'] != 'omega' :
				start_var_value = 3.0 * truth_var_value
	row_list.append( [start_var_value] )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
connection.close()
#
# Simulate a data set corresponding to the truth
number_simulate = '1'
cmd             = [ program, file_name, 'simulate', number_simulate ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at simulate command failed')
#
# Do a fit
cmd            = [ program, file_name, 'fit', 'fixed' ]
if fit_with_noise_in_data :
	simulate_index = '0'
	cmd += [ simulate_index ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at fit command failed')
# -----------------------------------------------------------------------------
# compare truth and fit
file_name      = 'example.db'
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table       = dismod_at.get_table_dict(connection, 'var')
truth_var_table = dismod_at.get_table_dict(connection, 'truth_var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
eps             = 0.15
ok              = True
max_err         = 0.0
for var_id in range( len(var_table) ) :
	truth_var_value = truth_var_table[var_id]['truth_var_value']
	fit_var_value   = fit_var_table[var_id]['fit_var_value']
	if truth_var_value != 0.0 :
		rel_err = fit_var_value / truth_var_value - 1.0
		flag    = abs( rel_err ) <= eps
		ok     &= flag
		if not flag :
			print('var_id = ', var_id, ', rel_err = ', rel_err)
		max_err = max( max_err, abs(rel_err) )
print(max_err)
assert ok
# -----------------------------------------------------------------------------
print('diabetes.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
