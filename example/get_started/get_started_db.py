# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin get_started_db.py$$ $newlinech #$$
# $spell
#	dismod
# $$
#
# $section Create get_started Input Tables: Example and Test$$
#
# $head Syntax$$
# $codei%(%n_smooth%, %rate_true%) = get_started_db.get_started_db()%$$
#
# $head Purpose$$
# Creates the database $code get_started.db$$ in the current working directory.
# This is a very simple case that is defined by the source code below
# and the specifications for $cref create_database$$.
#
# $head Source Code$$
# $code
# $verbatim%
#	example/get_started/get_started_db.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ---------------------------------------------------------------------------
def constant_weight_fun(a, t) :
	return 1.0
def gaussian_zero_fun(a, t) :
	# note that the a, t values are not used
	return ('prior_mean_zero', 'prior_mean_zero', 'prior_mean_zero')
def uniform_positive_fun(a, t) :
	# note that the a, t values are not used
	return ('prior_uniform_positive', 'prior_mean_zero', 'prior_mean_zero')
# ------------------------------------------------------------------------
def get_started_db (file_name) :
	import sys
	import os
	import copy
	import dismod_at
	# ----------------------------------------------------------------------
	# age table
	age_list    = [    0.0, 50.0,    100.0 ]
	#
	# time table
	time_list   = [ 1995.0, 2005.0, 2015.0 ]
	#
	# integrand table
	integrand_list = [
		{ 'name':'prevalence',  'eta':1e-6 },
		{ 'name':'Sincidence',  'eta':1e-6 },
		{ 'name':'remission',   'eta':1e-6 },
		{ 'name':'mtexcess',    'eta':1e-6 },
		{ 'name':'mtother',     'eta':1e-6 }
	]
	#
	# node table: world -> north_america
	#             north_america -> (united_states, canada)
	node_list = [
		{ 'name':'world',         'parent':'' },
		{ 'name':'north_america', 'parent':'world' },
		{ 'name':'united_states', 'parent':'north_america' },
		{ 'name':'canada',        'parent':'north_america' }
	]
	#
	# weight table: The constant function 1.0 (one age and one time point)
	fun = constant_weight_fun
	weight_list = [
		{ 'name':'constant',  'age_id':[1], 'time_id':[1], 'fun':fun }
	]
	#
	# covariate table: no covriates
	covariate_list = list()
	#
	# mulcov table
	mulcov_list = list()
	# --------------------------------------------------------------------------
	# data table: same order as list of integrands
	data_list = list()
	# values that are the same for all data rows
	row = {
		'node':        'world',
		'density':     'gaussian',
		'weight':      'constant',
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    0.0
	}
	# values that change between rows: (one data point for each integrand)
	for data_id in range( len(integrand_list) ) :
		meas_value        = 1e-2 * (data_id + 1)
		meas_std          = 0.2 * meas_value
		integrand         = integrand_list[data_id]['name']
		row['meas_value'] = meas_value
		row['meas_std']   = meas_std
		row['integrand']  = integrand
		if integrand == 'prevalence' :
			# prevalence is measured at age zero
			row['age_upper'] = 0.0
		else :
			# other integrands are averaged from age zero to one hundred
			row['age_upper'] = 100.0
		data_list.append( copy.copy(row) )
	# --------------------------------------------------------------------------
	# prior_table
	prior_list = [
		{   # prior_zero
			'name':     'prior_zero',
			'density':  'uniform',
			'lower':    0.0,
			'upper':    0.0,
			'mean':     0.0,
			'std':      None,
			'eta':      None
		},{ # prior_one
			'name':     'prior_one',
			'density':  'uniform',
			'lower':    1.0,
			'upper':    1.0,
			'mean':     1.0,
			'std':      None,
			'eta':      None
		},{ # prior_uniform_positive
			'name':     'prior_uniform_positive',
			'density':  'uniform',
			'lower':    1e-4,
			'upper':    None,
			'mean':     1e-1,
			'std':      None,
			'eta':      None
		},{ # prior_mean_zero
			'name':     'prior_mean_zero',
			'density':  'gaussian',
			'lower':    None,
			'upper':    None,
			'mean':     0.0,
			'std':      1e-2,
			'eta':      None
		}
	]
	# --------------------------------------------------------------------------
	# smooth table
	middle_age_id  = 1
	last_time_id   = 2
	smooth_list = [
		{   # smooth_mean_zero
			'name':                     'smooth_mean_zero',
			'age_id':                   [ middle_age_id ],
			'time_id':                  [ 0, last_time_id ],
			'mulstd_value_prior_name':  'prior_one',
			'mulstd_dage_prior_name':   'prior_one',
			'mulstd_dtime_prior_name':  'prior_one',
			'fun':                      gaussian_zero_fun
		},{ # smooth_uniform_positive
			'name':                     'smooth_uniform_positive',
			'age_id':                   [ middle_age_id ],
			'time_id':                  [ 0, last_time_id ],
			'mulstd_value_prior_name':  'prior_one',
			'mulstd_dage_prior_name':   'prior_one',
			'mulstd_dtime_prior_name':  'prior_one',
			'fun':                       uniform_positive_fun
		}
	]
	# --------------------------------------------------------------------------
	# rate table
	rate_list = [
		{
			'name':          'pini',
			'parent_smooth': 'smooth_uniform_positive',
			'child_smooth':  'smooth_mean_zero'
		},{
			'name':          'iota',
			'parent_smooth': 'smooth_uniform_positive',
			'child_smooth':  'smooth_mean_zero'
		},{
			'name':          'rho',
			'parent_smooth': 'smooth_uniform_positive',
			'child_smooth':  'smooth_mean_zero'
		},{
			'name':          'chi',
			'parent_smooth': 'smooth_uniform_positive',
			'child_smooth':  'smooth_mean_zero'
		},{
			'name':          'omega',
			'parent_smooth': 'smooth_uniform_positive',
			'child_smooth':  'smooth_mean_zero'
		}
	]
	# ------------------------------------------------------------------------
	# argument_list
	argument_list = [
		{ 'name':'parent_node_id','value':'0'            },
		{ 'name':'ode_step_size', 'value':'10.0'         },
		{ 'name':'tolerance',     'value':'1e-8'         },
		{ 'name':'max_num_iter',  'value':'100'          },
		{ 'name':'random_seed',   'value':'0'            },
		{ 'name':'number_sample', 'value':'1'            },
		{ 'name':'rate_info',     'value':'chi_positive' }
	]
	# --------------------------------------------------------------------------
	# create database
	dismod_at.create_database(
		file_name,
		age_list,
		time_list,
		integrand_list,
		node_list,
		weight_list,
		covariate_list,
		data_list,
		prior_list,
		smooth_list,
		rate_list,
		mulcov_list,
		argument_list
	)
	# -----------------------------------------------------------------------
	n_smooth  = len( smooth_list )
	rate_true = []
	for rate_id in range( len( data_list ) ) :
		# for this particular example
		data_id    = rate_id
		meas_value = data_list[data_id]['meas_value']
		rate_true.append(meas_value)
	#
	return (n_smooth, rate_true)
# END PYTHON
