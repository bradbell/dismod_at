# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2014-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin get_started.py$$ $newlinech #$$
#
# $section Getting Started$$
#
# $index get_started, example$$
# $index example, get_started$$
#
# $code
# $verbatim%example/get_started.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def get_started() :
	import dismod_at
	import copy
	def constant_weight_fun(a, t) :
		return 1.0
	def constant_smooth_fun(a, t) :
		return ('uniform_01', 'uniform_01', 'uniform_01')
	#
	# ------------------------------------------------------------------------
	file_name = 'example.db'
	node_list = [
		{ 'name':'world',         'parent':None },
		{ 'name':'north_america', 'parent':'world' },
		{ 'name':'united_states', 'parent':'north_america' },
		{ 'name':'canada',        'parent':'north_america' }
	]
	fun = constant_weight_fun
	weight_list = [
		{ 'name':'constant',  'age':[50.], 'time':[2000.], 'fun':fun }
	]
	covariate_list = [
		{ 'name':'sex', 'reference':0.0 }
	] 
	data_list = []
	row = {
		'integrand':'mtother',
		'weight':'constant',
		'meas_std':1e-5,
		'meas_density':'gaussian',
		'meas_eta':1e-6,
		'age_lower':0.0,
		'age_upper':100.0,
		'time_lower':1990.0,
		'time_upper':2010.0
	}
	row['node']       = 'north_america'
	row['meas_value'] = 1e-5
	data_list.append( copy.copy(row) )
	row['node']       = 'united_states'
	row['meas_value'] = 1.5e-5
	data_list.append( copy.copy(row) )
	row['node']       = 'canada'
	row['meas_value'] = 0.5e-5
	data_list.append( copy.copy(row) )
	#
	like_list = [
		{	'name':'uniform_01', 
			'lower':0.0, 
			'upper':1.0, 
			'mean':0.1, 
			'std':None,
			'density':None,
			'eta':None
		},{	'name':'zero', 
			'lower':0.0, 
			'upper':0.0, 
			'mean':0.0,
			'std':None,
			'density':None,
			'eta':None
		}
	]
	smooth_list = [
		{	'name':'constant',
			'age':[50.0],
			'time':[2000.],
			'fun':constant_smooth_fun
		}
	] 
	rate_list = [
		{	'name':'omega',
			'parent':True,
			'smooth':'constant'
		},{
			'name':'omega',
			'parent':False,
			'smooth':'constant'
		}
	]
	multiplier_list = [
		{	'covariate':'sex',
			'type':'rate',
			'effected':'omega',
			'smooth':'constant'
		}
	]
	dismod_at.create_database(
		file_name, 
		node_list, 
		weight_list, 
		covariate_list,
		data_list,
		like_list,
		smooth_list,
		rate_list,
		multiplier_list
	)
	# -----------------------------------------------------------------------
	print('get_started: OK')
# END PYTHON
