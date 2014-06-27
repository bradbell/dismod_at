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
	#
	# Connect to a new data base
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	#
	# primary key type
	ptype ='integer primary key'
	# ------------------------------------------------------------------------
	# create integrand table
	mtother_id = 1
	col_name = [  'integrand_id', 'integrand_name'  ]
	col_type = [   ptype,          'text'           ]
	row_list = [ [ mtother_id,     'mtother'        ] ]
	tbl_name = 'intergrand'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# create node table
	world_id         = 0
	north_america_id = 1
	united_states_id = 2
	canada_id        = 3
	col_name = [ 'node_id',          'node_name',     'parent_id'      ]
	col_type = [ ptype,              'text',          'integer'        ]
	row_list = [
	           [ world_id,           'world',         None             ],
	           [ north_america_id,   'north_america', world_id         ],
	           [ united_states_id,   'united_states', north_america_id ],
	           [ canada_id,          'canada',        north_america_id ]
	]
	tbl_name = 'node'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------ 
	# create weight_grid table
	constant_id = 0
	col_name = [   'weight_grid_id', 'weight_grid_name'   ]
	col_type = [   ptype,            'text'               ]
	row_list = [ [ constant_id,      'constant'           ] ]
	tbl_name = 'weight_grid'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# create a weight table
	col_name = [  'weight_id', 'weight_grid_id', 'age',   'time',  'weight' ]
	col_type = [  ptype,       'integer',        'real',  'real',  'real'   ]
	row_list = [[ 1,            constant_id,     50.0,    2000.,   1.0    ] ]
	tbl_name = 'weight'
	# ------------------------------------------------------------------------
	# create the covariate table
	col_name = [ 'covariate_id', 'covariate_name',	'reference' ]
	col_type = [ ptype,          'text',             'real'     ] 
	row_list = [ ]
	tbl_name = 'covariate'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# create the data table
	col_name = [
		# required columns
		'data_id',
		'integrand_id',
		'node_id',
		'weight_grid_id',
		'meas_value',
		'meas_std',
		'meas_density',
		'meas_eta',
		'age_lower',
		'age_upper',
		'time_lower',
		'time_upper',
	]
	col_type = [
		'integer primary key',  # data_id
		'integer',              # integrand_id
		'integer',              # node_id
		'integer',              # weight_grid_id
		'real',                 # meas_value
		'real',                 # meas_std
		'text',                 # meas_density
		'real',                 # meas_eta
		'real',                 # age_lower
		'real',                 # age_upper
		'real',                 # time_lower
		'real',                 # time_upper
	]
	row = [
		None,                   # data_id
		mtother_id,             # integrand_id
		None,                   # node_id
		constant_id,            # weight_grid_id
		None,                   # meas_value
		1e-5,                   # meas_std
		'gaussian',             # meas_density
		1e-6,                   # meas_eta
		0.0,                    # age_lower
		100.0,                  # age_upper
		1990.0,                 # time_lower
		2010.0,                 # time_upper
	]
	node_id_index          = 2
	meas_value_index       = 4
	row_list               = list()
	#
	row[node_id_index]     = north_america_id
	row[meas_value_index]  = 1e-5
	row_list.append( copy.copy(row) )
	#
	row[node_id_index]     = united_states_id
	row[meas_value_index]  = 1.5e-5
	row_list.append( copy.copy(row) )
	#
	row[node_id_index]     = canada_id
	row[meas_value_index]  = 1.5e-5
	row_list.append( copy.copy(row) )
	#
	tbl_name = 'data'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# -----------------------------------------------------------------------
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
