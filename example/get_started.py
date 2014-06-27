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
	data_list = []
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
	dismod_at.create_database(
		file_name, 
		node_list, 
		weight_list, 
		covariate_list,
		data_list
	)
	# -----------------------------------------------------------------------
	new        = False
	connection = dismod_at.create_connection(file_name, new)
	#
	# create the like table
	col_name = [ 
		'like_id', 
		'like_name',	
		'lower',	
		'upper',	
		'mean',	
		'std',	
		'density',
		'eta'  
	]
	ptype    = 'integer primary key'
	col_type = [ 
		ptype,            # like_id 
		'text',           # like_name	
		'real',           # lower	
		'real',           # upper	
		'real',           # mean	
		'real',           # std	
		'text',           # density
		'real'            # eta
	]
	uniform_01_id = 1
	zero_id       = 2
	row_list = [ [ 
		uniform_01_id,    # like_id 
		'uniform_01',     # like_name	
		0.0,              # lower	
		1.0,              # upper	
		0.5,              # mean	
		None,             # std	    (substititue for infinity for sql)
		'gaussian',       # density (not used because std = infinity)
		1e-6              # eta     (not used because std = infinity)
	], [
		zero_id,          # like_id 
		'zero',           # like_name	
		0.0,              # lower	
		0.0,              # upper	
		0.0,              # mean	
		None,             # std	    (substititue for infinity for sql)
		'gaussian',       # density (not used because std = infinity)
		1e-6              # eta     (not used because std = infinity)
	] ]
	tbl_name = 'like'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# -----------------------------------------------------------------------
	# create smooth_grid table
	constant_id = 0
	col_name = [ 'smooth_grid_id', 'smooth_grid_name' ]
	col_type = [ ptype,            'text'             ]
	row_list = [ [ constant_id,    'constant'      ]  ]
	tbl_name = 'smooth_grid'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ----------------------------------------------------------------------- 
	# smooth table column names
	col_name = [
		'smooth_id', 
		'smooth_grid_id', 
		'age',  
		'time',  
		'value_like_id',
		'dage_like_id',
		'dtime_like_id',
	]
	col_type = [
		ptype,      # smooth_id
		'integer',  # smooth_grid_id
		'real',     # age
		'real',     # time
		'integer',  # value_like_id
		'integer',  # dage_like_id
		'integer',  # dtime_like_id
	]
	row_list = list()
	constant_grid_id = 1
	row_list  = [ [
		None,            # smooth_id
		constant_grid_id,# smooth_grid_id 
		50.0,            # age
		2000.0,          # time
		uniform_01_id,   # value_like_id
		zero_id,         # dage_like_id
		zero_id          # dtime_like_id
	] ]
	tbl_name = 'smooth'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------ 
	# multiplier table
	col_name = [ 
		'multiplier_id', 
		'multiplier_use',
		'covariate_id', 
		'integrand_id',
		'rate_name',
  		'smooth_grid_id'
	]
	col_type = [ 
		ptype,     # multiplier_id
		'text',    # multiplier_use
		'integer', # covariate_id
		'integer', # integrand_id
		'text',    # rate_name
  		'integer'  # smooth_grid_id'
	]
	row_list = [ ]
	tbl_name = 'multiplier'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ----------------------------------------------------------------------- 
	# rate table
	col_name = [ 'rate_id',      'rate_name'  ]
	col_type = [ ptype,          'text'       ]
	row_list = [
	           [ None,           'iota'       ],
	           [ None,           'rho'        ],
	           [ None,           'chi'        ],
	           [ None,           'omega'      ]
	]
	tbl_name = 'rate'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ----------------------------------------------------------------------- 
	# rate_prior table
	col_name = [ 'rate_prior_id','is_parent', 'rate_name', 'smooth_grid_id']
	col_type = [ ptype,          'integer',   'text',      'integer'       ]
	row_list = [
	           [ 4,               1,           'omega',    constant_grid_id ],
	           [ 8,               0,           'omega',    constant_grid_id ]
	]
	tbl_name = 'rate_prior'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ----------------------------------------------------------------------- 
	#
	print('get_started: OK')
# END PYTHON
