#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 220 or later
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
#	cv
#	std
#	integrand
#	integrands
#	py
#	str
#	bool
#	var
#	rel
#	bmi
# $$
#
# $section An Example Fitting Simulated Diabetes Data$$
#
# $head Running This example$$
# If $icode python3$$ is the name of the python 3 program on your system,
# $codei%
#	%python3% example/user/diabetes.py
# %$$
# will run this program.
#
# $head Node Table$$
# The $cref node_table$$ only contains the
# $cref/parent/node_table/parent/$$ and child nodes specified by
# the $code node_list$$.
#
# $head Constraints$$
# The model rate $cref/omega/rate_table/rate_name/omega/$$
# is constrained to have the value used during simulation of the data.
# The model rate $cref/rho/rate_table/rate_name/rho/$$
# is constrained to be zero.
#
# $head Weight Table and Grid$$
# There is one weighting, with the constant value one,
# in the $cref weight_table$$ and $cref weight_grid_table$$.
#
# $head Covariates$$
#
# $subhead Covariate Table$$
# The covariate table has the following values:
# $table
# covariate_name    $cnext reference      $cnext max_difference $rnext
# sex               $cnext 0              $cnext 0.6            $rnext
# bmi               $cnext 28             $cnext null           $rnext
# ms_2000           $cnext 0              $cnext null
# $tend
#
# $subhead Data Table$$
# The $cref/covariate/data_table/Covariates/$$ columns in the
# data table have the following values:
# $icode sex$$ is $code 0.5$$ for male and $code -0.5$$ for female,
# $icode bmi$$ is body mass index $codei%20 <= %bmi% <= 36%$$,
# $icode ms_2000$$ is $code 1.0$$ if this is year 2000 market scan data
# and $code 0.0$$ otherwise.
#
# $subhead Multiplier Table$$
# The bmi and sex covariates multipliers are constrained to be zero
# (are not yet used). The ms_2000 multiplier has an N(0, 1) prior
# and true value of 0.25.
#
# $head Truth Table$$
# The values in the $cref truth_var_table$$ are generated using bilinear
# interpolation of the log of values specified at
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
# $head Problem Parameters$$
#
# $subhead node_list$$
# This is a $code list$$ with $code str$$ elements.
# The first element of this list is the parent node,
# the others are the child nodes. There must be an even number of children;
# i.e., an odd number of elements in this list.
# The case with zero child; i.e., one element in the list, is OK:
# $srccode%py%
node_list = [ 'US', 'Alabama', 'California', 'Massachusetts', 'Wisconsin' ]
node_list = [ 'US' ]
# %$$
#
# $subhead integrand_list$$
# This is a $code list$$ with $code str$$ elements that are
# $cref/integrand names/integrand_table/integrand_name/$$
# that will have measurements in the $cref data_table$$
# and $cref simulate_table$$.
# As mentioned in the
# $cref/constraints/user_diabetes.py/Constraints/$$ above, the rates
# $icode rho$$ and $icode omega$$ are know during the estimation (fitting)
# process.
# The integrands must inform the estimation of
# the model rates for
# $cref/pini/rate_table/rate_name/pini/$$,
# $cref/iota/rate_table/rate_name/iota/$$, and
# $cref/chi/rate_table/rate_name/chi/$$:
# $srccode%py%
integrand_list = [ 'mtspecific', 'prevalence' ]
# %$$
#
# $subhead age_grid$$
# This is a $code dict$$ with $code float$$ values containing
# the start age, end age, number of age grid points. and
# standard deviation in the log-Gaussian used to smooth the
# $cref/parent rates/model_variables/Fixed Effects, theta/Parent Rates/$$
# age differences.
# This is also the set of ages in the $cref age_table$$.
# The interval between age grid points is the end age, minus the start age,
# divided by the number of grid points minus one.
# The standard deviation is for the log-Gaussian in the prior used to smooth
# the difference of parent rates between age grid points:
# $srccode%py%
age_grid  = { 'start':0.0, 'end':100, 'number':9, 'std':0.2 }
# %$$
#
# $subhead time_grid$$
# This is a $code dict$$ with $code float$$ values containing
# the start time, end time, number of time grid points. and
# standard deviation in the log-Gaussian used to smooth the
# $cref/parent rates/model_variables/Fixed Effects, theta/Parent Rates/$$
# time differences.
# This is also the set of times in the $cref time_table$$.
# The interval between time grid points is the end time, minus the start time,
# divided by the number of grid points minus one.
# The standard deviation is for the log-Gaussian in the prior used to smooth
# the difference of parent rates between time grid points:
# $srccode%py%
time_grid = { 'start':1990.0, 'end': 2020, 'number':2, 'std':1.0  }
# %$$
#
# $subhead ode_step_size$$
# This is a $code str$$ that specifies the
# $cref/ode_step_size/option_table/ode_step_size/$$.
# It is suggest that this value be less than the intervals in the
# age and time grids:
# $srccode%py%
ode_step_size = '5.0'
# %$$
#
# $subhead meas_cv$$
# This is a $code float$$ that specifies the measurement standard deviations
# $cref/meas_std/data_table/meas_std/$$ by
# $codei%
#	%meas_std% = %meas_cv% * %meas_value%
# %$$
# For this example, the data table column
# $cref/meas_value/data_table/meas_value/$$ does not have any noise; i.e.,
# the values in that column are the corresponding
# $cref/average integrand/avg_integrand/Average Integrand, A_i/$$.
# The $icode meas_std$$ determines the noise level used by the
# $cref simulate_command$$:
# $srccode%py%
meas_cv = 0.1
# %$$
#
# $subhead meas_repeat$$
# This is a positive $code int$$ that specifies
# the number of times each noiseless measurement is repeated.
# Note that the simulated measurements will be different, because
# the noise for each measurement will be different.
# There are $icode meas_repeat$$
# data points for each integrand in the integrand list,
# each age in the age grid,
# each time in the time grid,
# each node in the node list.
# In addition if an age is not the first age and time is not the first time,
# there is a data point in the middle of the age-time interval that ends
# at that (age, time):
# $srccode%py%
meas_repeat = 3
# %$$
#
# $subhead fit_with_nose_in_data$$
# This is a $code bool$$ that specifies if measurement noise is included
# when fitting the data; i.e., if simulated measurements are used to
# fit the $cref model_variables$$.
# Otherwise, the measurements without noise
# $cref/meas_value/data_table/meas_value/$$
# are used to fit the model variables:
# $srccode%py%
fit_with_noise_in_data = True
# %$$
#
# $subhead quasi_fixed$$
# This is a $code str$$ that is either $code true$$ or $code false$$.
# If true, a Quasi-Newton method is used to
# optimize the fixed effects.
# This only requires function values and
# first derivatives for the objective and constraints.
# If it is false, a Newton method is used.
# This requires second derivatives in which case initialization
# and function evaluations take longer:
# $srccode%py%
quasi_fixed = 'true'
# %$$
#
# $subhead truth2start$$
# This is a $code float$$ that is used to map
# $codei%
#	%start_var_value% = %truth2start% * %truth_var_value%
# %$$
# for each model variable that is not constrained to a specific value.
# The notation
# $cref/truth_var_value/truth_var_table/truth_var_value/$$ is the true value
# used to simulate the data and
# $cref/start_var_value/start_var_table/start_var_value/$$ is the initial
# value of the variable during the fit.
# An error will result if the starting value for a variable is not within
# the upper and lower limits for a variable.
# $icode truth2start$$:
# $srccode%py%
truth2start = 0.3
# %$$
#
# $subhead max_abs_rel_err$$
# This is a $code float$$ that specifies the maximum absolute relative error.
# To be specific
# $codei%
#	%max_abs_rel_err% >= %fit_var_value% / %truth_var_value% - 1.0
# %$$
# where for each model variable,
# $cref/truth_var_value/truth_var_table/truth_var_value/$$ is the true value
# used to simulate the data and
# $cref/fit_var_value/fit_var_table/fit_var_value/$$ is result of the fit.
# A python assertion is generated if the condition above is not satisfied.
# $srccode%py%
max_abs_rel_err = 0.2
# %$$
#
# $head Source Code$$
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
	parent_node = node_list[0]
	if node != parent_node :
		even_child = node_list.index(node) % 2 == 0
	# default
	grid_value = dict()
	# -------------------------------------------------------------------------
	if rate == 'pini' :
		if node == parent_node :
			grid_value['start_age, start_time'] = 1e-2
			grid_value['start_age, end_time']   = 1e-2
		elif even_child :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
		else :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
		#
		# pini is constant in age
		grid_value['end_age, start_time'] = grid_value['start_age, start_time']
		grid_value['end_age, end_time']   = grid_value['start_age, end_time']
		#
		ret = log_bilinear(grid_value, a, t)
		if node != parent_node :
			ret = math.log(ret)
	# -------------------------------------------------------------------------
	elif rate == 'iota' :
		if node == parent_node :
			grid_value['start_age, start_time'] = 1e-3
			grid_value['start_age, end_time']   = 2e-3
			grid_value['end_age, start_time']   = 1e-2
			grid_value['end_age, end_time']     = 2e-2
		elif even_child :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
			grid_value['end_age, start_time']   = 1.0
			grid_value['end_age, end_time']     = 1.0
		else :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
			grid_value['end_age, start_time']   = 1.0
			grid_value['end_age, end_time']     = 1.0
		#
		ret = log_bilinear(grid_value, a, t)
		if node != parent_node :
			ret = math.log(ret)
	# -------------------------------------------------------------------------
	elif rate == 'omega' :
		if node == parent_node :
			grid_value['start_age, start_time'] = 3e-3
			grid_value['start_age, end_time']   = 2e-3
			grid_value['end_age, start_time']   = 3e-1
			grid_value['end_age, end_time']     = 2e-1
		elif even_child :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
			grid_value['end_age, start_time']   = 1.0
			grid_value['end_age, end_time']     = 1.0
		else :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
			grid_value['end_age, start_time']   = 1.0
			grid_value['end_age, end_time']     = 1.0
		#
		ret = log_bilinear(grid_value, a, t)
		if node != parent_node :
			ret = math.log(ret)
	# -------------------------------------------------------------------------
	elif rate == 'chi' :
		if node == parent_node :
			grid_value['start_age, start_time'] = 4.0e-3
			grid_value['start_age, end_time']   = 2.0e-3
			grid_value['end_age, start_time']   = 1.0e-1
			grid_value['end_age, end_time']     = 0.5e-1
		elif even_child :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
			grid_value['end_age, start_time']   = 1.0
			grid_value['end_age, end_time']     = 1.0
		else :
			grid_value['start_age, start_time'] = 1.0
			grid_value['start_age, end_time']   = 1.0
			grid_value['end_age, start_time']   = 1.0
			grid_value['end_age, end_time']     = 1.0
		#
		ret = log_bilinear(grid_value, a, t)
		if node != parent_node :
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
	def fun_mulcov_sex(a, t) :
		return('prior_sex',          None, None)
	def fun_mulcov_bmi(a, t) :
		return('prior_bmi',         None, None)
	def fun_mulcov_ms_2000(a, t) :
		return('prior_ms_2000',     None, None)
	#
	# priors used in smoothing for iota
	def fun_iota_parent(a, t) :
		return ( \
			'prior_pos_parent_value', 'prior_parent_age', 'prior_parent_time')
	def fun_iota_child(a, t) :
		return ('prior_N(0,1)', 'prior_child_age', 'prior_child_time')
	#
	# priors used in smoothing for chi
	def fun_chi_parent(a, t) :
		return ( \
			'prior_pos_parent_value', 'prior_parent_age', 'prior_parent_time')
	def fun_chi_child(a, t) :
		return ('prior_N(0,1)', 'prior_child_age', 'prior_child_time')
	#
	# use const_value to constrain omega to true value
	def fun_omega(node) :
		# Use default argument to bind the value of node at point of
		# definition instead of at point of call.
		def fun_omega_node(a, t, node = node) :
			true_value = true_rate(node, 'omega', a, t)
			return (true_value, 'prior_U(-inf,inf)', 'prior_U(-inf,inf)')
		return fun_omega_node
	#
	# priors used in smoothing for pini
	def fun_pini_parent(a, t) :
		return ('prior_pini_parent_value', None, 'prior_parent_time')
	def fun_pini_child(a, t) :
		return ('prior_N(0,1)', None, 'prior_child_time')
	# ----------------------------------------------------------------------
	fun                       = dict()
	fun['mulcov_sex']         = fun_mulcov_sex
	fun['mulcov_bmi']         = fun_mulcov_bmi
	fun['mulcov_ms_2000']     = fun_mulcov_ms_2000
	fun['constant_one']       = fun_constant_one
	fun['iota_parent']        = fun_iota_parent
	fun['iota_child']         = fun_iota_child
	fun['chi_parent']         = fun_chi_parent
	fun['chi_child']          = fun_chi_child
	fun['pini_parent']        = fun_pini_parent
	fun['pini_child']         = fun_pini_child
	# ----------------------------------------------------------------------
	# age lists
	start                     = age_grid['start']
	end                       = age_grid['end']
	number                    = age_grid['number']
	interval                  = (end - start) / (number - 1)
	age_index                 = dict()
	age_list                  = [ start + j * interval for j in range(number) ]
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
	end                    = time_grid['end']
	number                 = time_grid['number']
	interval               = (end - start) / (number - 1)
	time_list              = [ start + i * interval for i in range(number) ]
	time_index_all         = range(number)
	time_index_rate_parent = time_index_all
	time_index_rate_child  = [0, number-1]
	# ----------------------------------------------------------------------
	# node table:
	parent_node = node_list[0]
	node_table = [ { 'name':parent_node, 'parent':'' } ]
	for i in range(1, len(node_list) ) :
		node_table.append( { 'name':node_list[i], 'parent':parent_node } )
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
	# prior_table
	prior_table = [
		{
			# prior_N(0,1)
			'name':     'prior_N(0,1)',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      1.0,
		} , {
			# prior_U(-inf,inf)
			'name':     'prior_U(-inf,inf)',
			'density':  'uniform',
			'mean':     0.0,
		} , {
			# prior_pini_parent_value
			'name':     'prior_pini_parent_value',
			'density':  'uniform',
			'mean':     0.1,  # setting start_var table so mean has no affect
			'lower':    0.0,
			'upper':    1.0,
			'eta':      1e-5, # used for log-scaling during optimization
		} , {
			# prior_pos_parent_value
			'name':     'prior_pos_parent_value',
			'density':  'uniform',
			'mean':     0.1, # setting start_var table so mean has no affect
			'lower':    1e-8,
			'upper':    1.0,
			'eta':      0.0, # used for log-scaling during optimization
		} , {
			# prior_parent_age
			'name':     'prior_parent_age',
			'density':  'log_gaussian',
			'mean':     0.0,
			'std':      age_grid['std'],
			'eta':      1e-5,
		} , {
			# prior_parent_time
			'name':     'prior_parent_time',
			'density':  'log_gaussian',
			'mean':     0.0,
			'std':      time_grid['std'],
			'eta':      1e-5,
		} , {
			# prior_child_age
			'name':     'prior_child_age',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      0.2,
			'eta':      1e-5,
		} , {
			# prior_child_time
			'name':     'prior_child_time',
			'density':  'gaussian',
			'mean':     0.0,
			'std':      0.2,
			'eta':      1e-5,
		} , {
			# prior_sex
			'name':     'prior_sex',
			'density':  'uniform',
			'mean':     0.0,
			'lower':    -2.0,
			'upper':    +2.0,
		} , {
			# prior_bmi
			'name':     'prior_bmi',
			'density':  'uniform',
			'mean':     0.0,
			'lower':    -0.1,
			'upper':    +0.1
		} , {
			# prior_ms_2000
			'name':     'prior_ms_2000',
			'density':  'uniform',
			'mean':     0.0,
			'lower':    -1.0,
			'upper':    +1.0,
			'mean':     0.0,
		}
	]
	# ----------------------------------------------------------------------
	# smooth table and nslist_table
	smooth_table   = list()
	nslist_table   = dict()
	#
	# smooth_mulcov
	for cov in [ 'sex', 'bmi', 'ms_2000' ] :
		smooth_table.append( {
			'name':     'smooth_mulcov_' + cov,
			'age_id':   [0],
			'time_id':  [0],
			'fun':      fun['mulcov_' + cov]
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
	for node in node_list :
		name = 'smooth_omega_' + node
		smoothing = { 'name': name }
		if node == parent_node :
			smoothing['age_id']  = age_index['omega_parent']
			smoothing['time_id'] = time_index_rate_parent
		else :
			smoothing['age_id']  = age_index['omega_child']
			smoothing['time_id'] = time_index_rate_child
			nslist_table['nslist_omega_child'].append( (node, name) )
		#
		smoothing['fun'] = fun_omega(node)
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
		{'name':'bmi',     'reference':27.0, 'max_difference':None } ,
		{'name':'ms_2000', 'reference':0.0,  'max_difference':None } ,
	]
	#
	# mulcov table:
	mulcov_table = [
		{
			# alpha for iota and sex
			'covariate': 'sex',
			'type':      'rate_value',
			'effected':  'iota',
			'smooth':    'smooth_mulcov_sex'
		} , {
			# alpha for iota and bmi
			'covariate': 'bmi',
			'type':      'rate_value',
			'effected':  'iota',
			'smooth':    'smooth_mulcov_bmi'
		} , {
			# beta for prevalence and ms_2000
			'covariate': 'ms_2000',
			'type':      'meas_value',
			'effected':  'prevalence',
			'smooth':    'smooth_mulcov_ms_2000'
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
			'parent_smooth': 'smooth_omega_' + parent_node,
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
	for k1 in range(n_integrand * n_age * n_time * n_node * meas_repeat) :
		den         = n_age * n_time * n_node * meas_repeat
		i_integrand = int( k1 / den )
		k2          =      k1 % den
		#
		den         = n_time * n_node * meas_repeat
		i_age       = int( k2 / den )
		k3          =      k2 % den
		#
		den         = n_node * meas_repeat
		i_time      = int( k3 / den )
		k4          =      k3 % den
		#
		den         = meas_repeat
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
		if k1 % 6 < 3 :
			ms_2000 = 1.0
		else :
			ms_2000 = 0.0
		#
		# body mass index
		bmi = 20 + k1 % 17
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
			'ms_2000':      ms_2000,
			'bmi':          bmi,
			'one':          1,
		}
		avgint_table.append( copy.copy(row) )
		#
		if i_age > 0 and i_time > 0 :
			age  = (age_list[i_age]   + age_list[i_age-1])   / 2.0
			time = (time_list[i_time] + time_list[i_time-1]) / 2.0
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
			'ms_2000':      ms_2000,
			'bmi':          bmi,
			'one':          1,
		}
		avgint_table.append( copy.copy(row) )
	# ----------------------------------------------------------------------
	# option_table
	option_table = [
		{ 'name':'rate_case',              'value':'iota_pos_rho_zero' },
		{ 'name':'parent_node_name',       'value':parent_node         },
		{ 'name':'ode_step_size',          'value':ode_step_size       },
		{ 'name':'random_seed',            'value':'0'                 },

		{ 'name':'quasi_fixed',            'value':quasi_fixed         },
		{ 'name':'max_num_iter_fixed',     'value':'300'               },
		{ 'name':'print_level_fixed',      'value':'5'                 },
		{ 'name':'tolerance_fixed',        'value':'1e-4'              },
		{ 'name':'derivative_test_fixed',  'value':'trace-adaptive'    },

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
		value = None
		#
		row          = var_table[var_id]
		var_type     = row['var_type']
		age          = age_table[ row['age_id'] ] ['age']
		time         = time_table[ row['time_id'] ] ['time']
		if var_type.startswith('mulcov_') :
			covariate = covariate_table[row['covariate_id' ]]['covariate_name']
			if covariate == 'sex' :
				value = 0.5
			elif covariate == 'bmi' :
				value = 0.02
			elif covariate == 'ms_2000' :
				value = 0.25
		elif var_type == 'rate' :
			node  = node_table[ row['node_id'] ] ['node_name']
			rate  = rate_table[ row['rate_id'] ] ['rate_name']
			value = true_rate(node, rate, age, time)
		else :
			assert False
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
	meas_std   = meas_cv * avg_integrand # noise in simulated data
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
	start_var_value  = truth2start * truth_var_value
	if var_type == 'rate' :
			if rate_table[rate_id]['rate_name'] == 'omega' :
				start_var_value  = truth_var_value
	row_list.append( [start_var_value] )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
connection.close()
#
if False :
	# copy start_var table to scale_var table
	cmd = [ program, file_name, 'set' , 'scale_var', 'start_var' ]
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at set command failed')
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
cmd            = [ program, file_name, 'fit', 'both' ]
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
ok              = True
max_err         = 0.0
for var_id in range( len(var_table) ) :
	truth_var_value = truth_var_table[var_id]['truth_var_value']
	fit_var_value   = fit_var_table[var_id]['fit_var_value']
	if truth_var_value != 0.0 :
		rel_err = fit_var_value / truth_var_value - 1.0
		flag    = abs( rel_err ) <= max_abs_rel_err
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
