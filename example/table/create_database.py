# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin create_database.py$$ $newlinech #$$
#
# $section create_database: Example and Test$$
#
# $code
# $verbatim%example/create_database.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
#
def constant_weight_fun(a, t) :
	return 1.0
def smooth_uniform_01_fun(a, t) :
	# note that the da and dt values are not used
	return ('uniform_01', 'uniform_01', 'uniform_01')
def smooth_gaussian_01_fun(a, t) :
	# note that the da and dt values are not used
	return ('gaussian_01', 'gaussian_01', 'gaussian_01')
def column_as_list(row_list, column) :
	ret = list()
	for	row in row_list :
		ret.append( row[column] )
	return ret
# ------------------------------------------------------------------------
def create_database() :
	import dismod_at
	import copy
	#
	# file_name, age_list, time_list, integrand_list
	file_name      = 'example.db'
	age_list       = [ 50.0 ]
	time_list      = [ 2000.0 ]
	integrand_list = [ { 'name':'mtother',    'eta':1e-6 } ]
	#
	# node_list
	node_list = [
		{ 'name':'world',         'parent':'' },
		{ 'name':'north_america', 'parent':'world' },
		{ 'name':'united_states', 'parent':'north_america' },
		{ 'name':'canada',        'parent':'north_america' }
	]
	# weight_list
	fun = constant_weight_fun
	weight_list = [
		{ 'name':'constant',  'age_id':[0], 'time_id':[0], 'fun':fun }
	]
	# covariate_list
	covariate_list = [
		{ 'name':'sex', 'reference':0.0, 'max_difference':0.6 }
	]
	# data_list
	data_list = []
	row = {
		'integrand':'mtother',
		'density':'log_gaussian',
		'weight':'constant',
		'meas_std':1e-5,
		'age_lower':0.0,
		'age_upper':100.0,
		'time_lower':1990.0,
		'time_upper':2010.0,
		'sex':0.5
	}
	row['node']       = 'north_america'
	row['meas_value'] = 1.0e-5
	data_list.append( copy.copy(row) )
	row['node']       = 'united_states'
	row['meas_value'] = 1.5e-5
	data_list.append( copy.copy(row) )
	row['node']       = 'canada'
	row['meas_value'] = 0.5e-5
	data_list.append( copy.copy(row) )
	#
	# prior_list
	prior_list = [
		{	'name':'zero',
			'density':'uniform',
			'lower':0.0,
			'upper':0.0,
			'mean':0.0,
			'std':None,
			'eta':None
		},{	'name':'one',
			'density':'uniform',
			'lower':1.0,
			'upper':1.0,
			'mean':1.0,
			'std':None,
			'eta':None
		},{	'name':'uniform_01',
			'density':'uniform',
			'lower':0.0,
			'upper':1.0,
			'mean':0.1,
			'std':None,
			'eta':None
		},{	'name':'gaussian_01',
			'density':'gaussian',
			'lower':None,
			'upper':None,
			'mean':0.0,
			'std':1.0,
			'eta':None
		},{	'name':'log_gaussian',
			'density':'log_gaussian',
			'lower':None,
			'upper':None,
			'mean':0.0,
			'std':None,
			'eta':1e-6
		}
	]
	#
	# smooth list
	smooth_list = [
		{	'name':'uniform_01_constant',
			'age_id':[0],
			'time_id':[0],
			'mulstd_value_prior_name':'one',
			'mulstd_dage_prior_name':'one',
			'mulstd_dtime_prior_name':'one',
			'fun':smooth_uniform_01_fun
		},{	'name':'gaussian_01_constant',
			'age_id':[0],
			'time_id':[0],
			'mulstd_value_prior_name':'one',
			'mulstd_dage_prior_name':'one',
			'mulstd_dtime_prior_name':'one',
			'fun':smooth_gaussian_01_fun
		}
	]
	#
	# rate_list
	rate_list = [
		{
			'name':'pini',
			'parent_smooth':'uniform_01_constant',
			'child_smooth':'gaussian_01_constant'
		},{
			'name':'iota',
			'parent_smooth':'uniform_01_constant',
			'child_smooth':'gaussian_01_constant'
		},{
			'name':'rho',
			'parent_smooth':'uniform_01_constant',
			'child_smooth':'gaussian_01_constant'
		},{
			'name':'chi',
			'parent_smooth':'uniform_01_constant',
			'child_smooth':'gaussian_01_constant'
		},{
			'name':'omega',
			'parent_smooth':'uniform_01_constant',
			'child_smooth':'gaussian_01_constant'
		}
	]
	#
	# mulcov_list
	mulcov_list = [
		{	'covariate':'sex',
			'type':'rate_mean',
			'effected':'omega',
			'smooth':'uniform_01_constant'
		}
	]
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
		mulcov_list
	)
	# -----------------------------------------------------------------------
	# Check database
	# -----------------------------------------------------------------------
	#
	# connection
	new        = False;
	connection = dismod_at.create_connection(file_name, new)
	#
	# age_table
	tbl_name = 'age'
	col_name = [ 'age' ]
	row_list = dismod_at.get_row_list(connection, tbl_name, col_name)
	col_list = column_as_list(row_list, 0)
	assert col_list == age_list
	#
	# time_table
	tbl_name = 'time'
	col_name = [ 'time' ]
	row_list = dismod_at.get_row_list(connection, tbl_name, col_name)
	col_list = column_as_list(row_list, 0)
	assert col_list == time_list
	#
	# intergrand_table
	tbl_name   = 'integrand'
	col_name   = [ 'integrand_name', 'eta' ]
	row_list   = dismod_at.get_row_list(connection, tbl_name, col_name)
	check_list = [ ['mtother', 1e-6] ]
	assert row_list == check_list
	#
	# weight_table
	tbl_name   = 'weight'
	col_name   = [ 'weight_name', 'n_age', 'n_time' ]
	row_list   = dismod_at.get_row_list(connection, tbl_name, col_name)
	check_list = [ [ 'constant', 1, 1 ] ]
	assert row_list == check_list
	#
	# weight_grid_table
	tbl_name   = 'weight_grid'
	col_name   = [ 'weight_id', 'age_id', 'time_id', 'weight' ]
	row_list   = dismod_at.get_row_list(connection, tbl_name, col_name)
	check_list = [ [ 0, 0, 0, 1.0 ] ]
	assert row_list == check_list
	#
	# covariate_table
	tbl_name   = 'covariate'
	col_name   = [ 'covariate_name', 'reference' ]
	row_list   = dismod_at.get_row_list(connection, tbl_name, col_name)
	check_list = [ [ 'sex', 0.0 ] ]
	assert row_list == check_list
	#
	# node_table
	tbl_name   = 'node'
	col_name   = [ 'node_name', 'parent' ]
	row_list   = dismod_at.get_row_list(connection, tbl_name, col_name)
	check_list = [
		['world',        -1],
		['north_america', 0],
		['united_states', 1],
		['canada',        1]
	]
	assert row_list == check_list
	#
	# data_table
	tbl_name   = 'data'
	col_name   = [ 'integrand_id', 'node_id', 'meas_value', 'meas_std'  ]
	row_list   = dismod_at.get_row_list(connection, tbl_name, col_name)
	check_list = [
		[ 0, 1, 1.0e-5, 1e-5 ],
		[ 0, 2, 1.5e-5, 1e-5 ],
		[ 0, 3, 0.5e-5, 1e-5 ]
	]
	assert row_list == check_list
	#
	# prior_table
	tbl_name   = 'prior'
	col_name   = [ 'prior_name', 'density_id', 'lower', 'upper'  ]
	row_list   = dismod_at.get_row_list(connection, tbl_name, col_name)
	check_list = [
		[ 'zero',          0, 0.0,   0.0 ],
		[ 'one',           0, 1.0,   1.0 ],
		[ 'uniform_01',    0, 0.0,   1.0 ],
		[ 'gaussian_01',   1, None, None ],
		[ 'log_gaussian',  3, None, None ]
	]
	assert row_list == check_list
	#
	# smooth_table
	tbl_name   = 'smooth'
	col_name   = [
		'smooth_name',
		'n_age',
		'n_time',
		'mulstd_value_prior_id',
		'mulstd_dage_prior_id',
		'mulstd_dtime_prior_id'
	]
	row_list   = dismod_at.get_row_list(connection, tbl_name, col_name)
	check_list = [
		[ 'uniform_01_constant',  1, 1, 1, 1, 1 ],
		[ 'gaussian_01_constant', 1, 1, 1, 1, 1 ]
	]
	assert row_list == check_list
	#
	# smooth_grid_table
	tbl_name = 'smooth_grid'
	col_name = [
		'smooth_id',      'age_id',        'time_id',
		'value_prior_id', 'dage_prior_id', 'dtime_prior_id'
	]
	row_list   = dismod_at.get_row_list(connection, tbl_name, col_name)
	check_list = [
		[ 0, 0, 0, 2, -1, -1 ],
		[ 1, 0, 0, 3, -1, -1 ]
	]
	assert row_list == check_list
	#
	# rate_table
	tbl_name = 'rate'
	col_name = [ 'rate_name', 'parent_smooth_id', 'child_smooth_id' ]
	row_list   = dismod_at.get_row_list(connection, tbl_name, col_name)
	check_list = [
		[ 'pini',   0, 1 ],
		[ 'iota',   0, 1 ],
		[ 'rho',    0, 1 ],
		[ 'chi',    0, 1 ],
		[ 'omega',  0, 1 ]
	]
	assert row_list == check_list
	#
	# mulcov_table
	tbl_name = 'mulcov'
	col_name = [
		'mulcov_type', 'rate_id', 'integrand_id', 'covariate_id', 'smooth_id'
	]
	row_list   = dismod_at.get_row_list(connection, tbl_name, col_name)
	check_list = [ [ 'rate_mean', 4, -1, 0, 0 ] ]
	assert row_list == check_list
	# -----------------------------------------------------------------------
	connection.close()
	print('create_database: OK')
# END PYTHON
