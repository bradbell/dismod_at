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
#	avgint
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
# note that the a, t values are not used for this example
def constant_weight_fun(a, t) :
	return 1.0
def fun_rate_child(a, t) :
	return ('prior_gauss_zero', 'prior_gauss_zero', 'prior_gauss_zero')
def fun_rate_parent(a, t) :
	return ('prior_rate_parent', 'prior_gauss_zero', 'prior_gauss_zero')
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
	integrand_dict = [
		{ 'name':'prevalence',  'eta':1e-6 },
		{ 'name':'Sincidence',  'eta':1e-6 },
		{ 'name':'remission',   'eta':1e-6 },
		{ 'name':'mtexcess',    'eta':1e-6 },
		{ 'name':'mtother',     'eta':1e-6 }
	]
	#
	# node table: world -> north_america
	#             north_america -> (united_states, canada)
	node_dict = [
		{ 'name':'world',         'parent':'' },
		{ 'name':'north_america', 'parent':'world' },
		{ 'name':'united_states', 'parent':'north_america' },
		{ 'name':'canada',        'parent':'north_america' }
	]
	#
	# weight table: The constant function 1.0 (one age and one time point)
	fun = constant_weight_fun
	weight_dict = [
		{ 'name':'constant',  'age_id':[1], 'time_id':[1], 'fun':fun }
	]
	#
	# covariate table: no covriates
	covariate_dict = list()
	#
	# mulcov table
	mulcov_dict = list()
	# --------------------------------------------------------------------------
	# data table: same order as list of integrands
	data_dict = list()
	# values that are the same for all data rows
	row = {
		'node':        'world',
		'density':     'gaussian',
		'weight':      'constant',
		'hold_out':     False,
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    0.0
	}
	# values that change between rows: (one data point for each integrand)
	for integrand_id in range( len(integrand_dict) ) :
		rate_id           = integrand_id
		meas_value        = 1e-2 * (rate_id + 1)
		meas_std          = 0.2 * meas_value
		integrand         = integrand_dict[integrand_id]['name']
		row['meas_value'] = meas_value
		row['meas_std']   = meas_std
		row['integrand']  = integrand
		if integrand == 'prevalence' :
			# prevalence is measured at age zero
			row['age_upper'] = 0.0
		else :
			# other integrands are averaged from age zero to one hundred
			row['age_upper'] = 100.0
		# data_id = rate_id = integand_id
		data_dict.append( copy.copy(row) )
	#
	# add one outlyer at end of data table with hold_out true
	row['hold_out']   = True # if outlyer were false, fit would fail
	row['integrand']  = data_dict[0]['integrand']
	row['meas_std']   = data_dict[0]['meas_std']
	row['age_upper']  = data_dict[0]['age_upper']
	row['meas_value'] = 10. * data_dict[0]['meas_value']
	data_dict.append( copy.copy(row) )
	# --------------------------------------------------------------------------
	# prior_table
	prior_dict = [
		{   # prior_zero
			'name':     'prior_zero',
			'density':  'uniform',
			'lower':    0.0,
			'upper':    0.0,
			'mean':     0.0,
			'std':      None,
			'eta':      None
		},{ # prior_rate_parent
			'name':     'prior_rate_parent',
			'density':  'uniform',
			'lower':    1e-4,
			'upper':    None,
			'mean':     1e-1,
			'std':      None,
			'eta':      None
		},{ # prior_gauss_zero
			'name':     'prior_gauss_zero',
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
	smooth_dict = [
		{   # smooth_rate_child
			'name':                     'smooth_rate_child',
			'age_id':                   [ middle_age_id ],
			'time_id':                  [ 0, last_time_id ],
			'mulstd_value_prior_name':  '',
			'mulstd_dage_prior_name':   '',
			'mulstd_dtime_prior_name':  '',
			'fun':                      fun_rate_child
		},{ # smooth_rate_parent
			'name':                     'smooth_rate_parent',
			'age_id':                   [ middle_age_id ],
			'time_id':                  [ 0, last_time_id ],
			'mulstd_value_prior_name':  '',
			'mulstd_dage_prior_name':   '',
			'mulstd_dtime_prior_name':  '',
			'fun':                       fun_rate_parent
		}
	]
	# --------------------------------------------------------------------------
	# rate table
	rate_dict = [
		{
			'name':          'pini',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child'
		},{
			'name':          'iota',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child'
		},{
			'name':          'rho',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child'
		},{
			'name':          'chi',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child'
		},{
			'name':          'omega',
			'parent_smooth': 'smooth_rate_parent',
			'child_smooth':  'smooth_rate_child'
		}
	]
	# ------------------------------------------------------------------------
	# option_dict
	option_dict = [
		{ 'name':'parent_node_id',         'value':'0'            },
		{ 'name':'number_sample',          'value':'1'            },
		{ 'name':'ode_step_size',          'value':'10.0'         },
		{ 'name':'random_seed',            'value':'0'            },
		{ 'name':'rate_info',              'value':'iota_pos_rho_pos' },

		{ 'name':'quasi_fixed',            'value':'true'         },
		{ 'name':'derivative_test_fixed',  'value':'first-order'  },
		{ 'name':'max_num_iter_fixed',     'value':'100'          },
		{ 'name':'print_level_fixed',      'value':'0'            },
		{ 'name':'tolerance_fixed',        'value':'1e-10'        },

		{ 'name':'derivative_test_random', 'value':'second-order' },
		{ 'name':'max_num_iter_random',    'value':'100'          },
		{ 'name':'print_level_random',     'value':'0'            },
		{ 'name':'tolerance_random',       'value':'1e-10'        }
	]
	# --------------------------------------------------------------------------
	# avgint table: same order as list of integrands
	avgint_dict = list()
	# values that are the same for all data rows
	row = {
		'node':        'world',
		'weight':      'constant',
		'time_lower':   2000.0,
		'time_upper':   2000.0,
		'age_lower':    0.0
	}
	# values that change between rows: (one data point for each integrand)
	for avgint_id in range( len(integrand_dict) ) :
		integrand         = integrand_dict[avgint_id]['name']
		row['integrand']  = integrand
		if integrand == 'prevalence' :
			# prevalence is measured at age zero
			row['age_upper'] = 0.0
		else :
			# other integrands are averaged from age zero to one hundred
			row['age_upper'] = 100.0
		avgint_dict.append( copy.copy(row) )
	# --------------------------------------------------------------------------
	# create database
	dismod_at.create_database(
		file_name,
		age_list,
		time_list,
		integrand_dict,
		node_dict,
		weight_dict,
		covariate_dict,
		data_dict,
		prior_dict,
		smooth_dict,
		rate_dict,
		mulcov_dict,
		option_dict,
		avgint_dict
	)
	# -----------------------------------------------------------------------
	n_smooth  = len( smooth_dict )
	rate_true = []
	for rate_id in range( len( data_dict ) ) :
		# for this particular example
		data_id    = rate_id
		meas_value = data_dict[data_id]['meas_value']
		rate_true.append(meas_value)
	#
	return (n_smooth, rate_true)
# END PYTHON
