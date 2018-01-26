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
# The $cref age_table$$ is uniformly spaced starting at age zero,
# ending at age 100. There is one extra age point added at age 21.
#
# $head Time Table$$
# The $cref time_table$$ is uniformly spaced starting at time 1990
# and ending at time 2020.
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
# $subhead smooth_omega$$
# The smoothings for omega are the same as for chi.
#
# $subhead smooth_iota_parent$$
# This smoothing's time grid includes every time point.
# The age grid for this smoothing has one point age zero,
# and all the age points that are greater than or equal 20.
# The value at each grid point has a U(1e-8, 1e-8) distribution if the age
# is less than or equal 20 and U(1e-8, 1) otherwise.
#
# $subhead smooth_iota_child$$
# This smoothing's time grid has two points,
# one at the minimum time and one at the maximum time.
# The age grid for this smoothing also has two points,
# at age zero the value has a U(0, 0) distribution,
# at age 100 is has an N(0, 1) distribution.
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
# interpolation of specified values at
# (start_age, start_time), (start_age, end_time), (end_age, start_time)
# and (end_age, end_time).
# There was an exception, if the age was less than or equal 20.0,
# the value zero was used for parent and child iota rate.
# See the function $code true_rate$$ below.
#
#
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
# values used to simulate truth
time_grid = { 'start':1990.0, 'end': 2020, 'number': 7 }
age_grid  = { 'start':0.0,    'end': 100,  'number': 22 }
#
def bilinear(grid_value, a, t) :
	# denominator
	da  = age_grid['end'] - age_grid['start']
	dt  = time_grid['end'] - time_grid['start']
	den = da * dt;
	num = 0.0
	#
	# value at start age and start time
	da   = age_grid['end'] - a
	dt   = time_grid['end'] - t
	num += grid_value['start_age, start_time'] * da * dt
	#
	# value at start age and end time
	da   = age_grid['end'] - a
	dt   = t - time_grid['start']
	num += grid_value['start_age, end_time'] * da * dt
	#
	# value at end age and start time
	da   = a - age_grid['start']
	dt   = time_grid['end'] - t
	num += grid_value['end_age, start_time'] * da * dt
	#
	# value at end age and end time
	da   = a - age_grid['start']
	dt   = t - time_grid['start']
	num += grid_value['end_age, end_time'] * da * dt
	#
	return num / den
#
def true_rate(node, rate, a, t) :
	# default
	grid_value = dict()
	ret = 0.0
	# -------------------------------------------------------------------------
	if rate == 'pini' :
		if node == 'US' :
			grid_value['start_age, start_time'] = 1e-2
			grid_value['start_age, end_time']   = 1e-2
		elif node in ['Alabama', 'Wisconsin'] :
			grid_value['start_age, start_time'] = 2e-2
			grid_value['start_age, end_time']   = 2e-2
		elif node in [ 'California', 'Massachusetts' ] :
			grid_value['start_age, start_time'] = 0.5e-2
			grid_value['start_age, end_time']   = 0.5e-2
		else :
			assert False
		#
		# pini is constant in age
		grid_value['end_age, start_time'] = grid_value['start_age, start_time']
		grid_value['end_age, end_time']   = grid_value['start_age, end_time']
		#
		ret = bilinear(grid_value, a, t)
	# -------------------------------------------------------------------------
	elif rate == 'iota' :
		if node == 'US' :
			grid_value['start_age, start_time'] = 1e-3
			grid_value['start_age, end_time']   = 2e-3
			grid_value['end_age, start_time']   = 3e-3
			grid_value['end_age, end_time']     = 4e-3
		elif node in ['Alabama', 'Wisconsin'] :
			grid_value['start_age, start_time'] = 2e-3
			grid_value['start_age, end_time']   = 4e-3
			grid_value['end_age, start_time']   = 6e-3
			grid_value['end_age, end_time']     = 8e-3
		elif node in [ 'California', 'Massachusetts' ] :
			grid_value['start_age, start_time'] = 0.5e-3
			grid_value['start_age, end_time']   = 1.0e-3
			grid_value['end_age, start_time']   = 1.5e-3
			grid_value['end_age, end_time']     = 2.0e-3
		else :
			assert False
		#
		# set iota truth to zero when age is less than or equal 20.
		if a <= 20.0 :
			ret = 0.0
		else :
			ret = bilinear(grid_value, a, t)
	# -------------------------------------------------------------------------
	elif rate == 'omega' :
		if node == 'US' :
			grid_value['start_age, start_time'] = 2e-4
			grid_value['start_age, end_time']   = 1e-4
			grid_value['end_age, start_time']   = 2e-1
			grid_value['end_age, end_time']     = 1e-1
		elif node in ['Alabama', 'Wisconsin'] :
			grid_value['start_age, start_time'] = 4e-4
			grid_value['start_age, end_time']   = 2e-4
			grid_value['end_age, start_time']   = 4e-1
			grid_value['end_age, end_time']     = 2e-1
		elif node in [ 'California', 'Massachusetts' ] :
			grid_value['start_age, start_time'] = 1.0e-4
			grid_value['start_age, end_time']   = 0.5e-4
			grid_value['end_age, start_time']   = 1.0e-1
			grid_value['end_age, end_time']     = 0.5e-1
		else :
			assert False
		#
		ret = bilinear(grid_value, a, t)
	# -------------------------------------------------------------------------
	elif rate == 'chi' :
		if node == 'US' :
			grid_value['start_age, start_time'] = 2e-3
			grid_value['start_age, end_time']   = 1e-3
			grid_value['end_age, start_time']   = 2e-1
			grid_value['end_age, end_time']     = 1e-1
		elif node in ['Alabama', 'Wisconsin'] :
			grid_value['start_age, start_time'] = 4e-3
			grid_value['start_age, end_time']   = 2e-3
			grid_value['end_age, start_time']   = 4e-1
			grid_value['end_age, end_time']     = 2e-1
		elif node in [ 'California', 'Massachusetts' ] :
			grid_value['start_age, start_time'] = 1.0e-3
			grid_value['start_age, end_time']   = 0.5e-3
			grid_value['end_age, start_time']   = 1.0e-1
			grid_value['end_age, end_time']     = 0.5e-1
		else :
			assert False
		#
		ret = bilinear(grid_value, a, t)
	# -------------------------------------------------------------------------
	if ret < 0.0 :
		import pdb; pdb.set_trace()
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
	def fun_mulcov(a, t) :
		return ('prior_N(0,1)', None, None)
	#
	# priors used in smoothing for iota
	def fun_iota_parent(a, t) :
		if a <= 20.0 :
			return ('prior_U(1e-8,1e-8)', 'prior_diff_rate', 'prior_diff_rate')
		else :
			return ('prior_U(1e-8,1)', 'prior_diff_rate', 'prior_diff_rate')
	def fun_iota_child(a, t) :
		if a <= 20.0 :
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
	def fun_pini_parent(a, t) :
		return ('prior_U(0,1)', None, 'prior_diff_rate')
	def fun_pini_child(a, t) :
		return ('prior_N(0,1)', None, 'prior_diff_rate')
	# ----------------------------------------------------------------------
	fun                    = dict()
	fun['mulcov']          = fun_mulcov
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
	start                     = age_grid['start']
	end                       = age_grid['end']
	number                    = age_grid['number']
	d_age                     = (end - start) / (number - 2)
	age_index                 = dict()
	age_list                  = [ start + j * d_age for j in range(number-1) ]
	age_list                 += [ 21.0 ]
	age_list                  = sorted( age_list )
	age_index_all             = range(number)
	iota_start                = None
	for i in range(number) :
		if age_list[i] == 21.0 :
			assert iota_start == None
			iota_start = i
	#
	tmp = list( range(iota_start, number) )
	if iota_start > 1 :
		age_index['iota_parent']  = [0, iota_start - 1] + tmp
	else :
		age_index['iota_parent']  = [ 0 ] + tmp
	age_index['chi_parent']       = age_index_all
	age_index['omega_parent']     = age_index['chi_parent']
	age_index['pini_parent']      = [ 0 ]
	#
	age_index['iota_child']   = [ 0, number-1 ]
	age_index['chi_child']    = [ 0, number-1 ]
	age_index['omega_child']  = [ 0, number-1 ]
	age_index['pini_child']   = [ 0 ]
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
			# prior_U(1e-8,1e-8)
			'name':     'prior_U(1e-8,1e-8)',
			'density':  'uniform',
			'mean':     1e-8,
			'lower':    1e-8,
			'upper':    1e-8,
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
		{	'name':          'pini',
			'parent_smooth': 'smooth_pini_parent',
			'child_smooth':  'smooth_pini_child',
		} , {
			'name':          'iota',
			'parent_smooth': 'smooth_iota_parent',
			'child_smooth':  'smooth_iota_child',
		} , {
			'name':          'rho',
			'parent_smooth': None,
			'child_smooth':  None,
		} , {
			'name':          'chi',
			'parent_smooth': 'smooth_chi_parent',
			'child_smooth':  'smooth_chi_child',
		} , {
			'name':          'omega',
			'parent_smooth': 'smooth_omega_parent',
			'child_smooth':  'smooth_omega_child',
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
	for k1 in range(n_integrand * n_age * n_time * n_node) :
		i_integrand = int( k1 / (n_age * n_time * n_node) )
		k2          = k1 % (n_age * n_time * n_node)
		#
		i_age       = int( k2 / (n_time * n_node) )
		k3          = k2 % n_time
		#
		i_time      = int( k3 / n_node )
		i_node      = k3 % n_node
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
# -----------------------------------------------------------------------
# read database tables
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table       = dismod_at.get_table_dict(connection, 'var')
rate_table      = dismod_at.get_table_dict(connection, 'rate')
covariate_table = dismod_at.get_table_dict(connection, 'covariate')
integrand_table = dismod_at.get_table_dict(connection, 'integrand')
node_table      = dismod_at.get_table_dict(connection, 'node')
time_table      = dismod_at.get_table_dict(connection, 'time')
age_table       = dismod_at.get_table_dict(connection, 'age')
# -----------------------------------------------------------------------------
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
		covariate = covariate_table[ row['covariate_id' ] ] ['covariate_name']
	#
	value = 0.0
	if var_type == 'rate' :
		node  = node_table[ row['node_id'] ] ['node_name']
		rate  = rate_table[ row['rate_id'] ] ['rate_name']
		value = true_rate(node, rate, age, time)
		if value != 0.0 :
			if node != 'US' :
				parent_value  = true_rate('US', rate, age, time)
				value     = math.log( value  / parent_value )
	row_list.append( [ value ] )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
connection.close()
# -----------------------------------------------------------------------------
print('diabetes.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
