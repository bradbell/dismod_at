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
def rate_table() :
	import dismod_at
	#
	# Connect to a new data base
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# primary key type
	ptype ='integer primary key'
	# ------------------------------------------------------------------------
	# create integrand table
	omega_id = 1
	col_name = [  'integrand_id', 'integrand_name'  ]
	col_type = [   ptype,          'text'           ]
	row_list = [ [ omega_id,       'omega'          ] ]
	tbl_name = 'intergrand'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# create node table
	world_id         = 0
	north_america_id = 1
	united_state_id  = 2
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
	ptype    = 'integer primary key'
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
	# create the data file
	col_name = [
		# required columns
		'data_id',
		'integrand_id',
		'node_id',
		'weight_grid_id',
		'meas_value',
		'meas_stdev',
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
		'real',                 # meas_stdev
		'text',                 # meas_density
		'real',                 # meas_eta
		'real',                 # age_lower
		'real',                 # age_upper
		'real',                 # time_lower
		'real',                 # time_upper
	]
	row = [
		None,                   # data_id
		omega_id,               # integrand_id
		None,                   # node_id
		constant_id,            # weight_grid_id
		None,                   # meas_value
		1e-5,                   # meas_stdev
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
	#
	print('get_started: OK')
# END PYTHON
