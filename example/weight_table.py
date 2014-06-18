# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2013-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
# $begin weight_table.py$$ $newlinech #$$
#
# $section weight_table: Example and Text$$
#
# $index weight, example$$
# $index example, weight table$$
# $index table, weight example$$
#
# $index user, example$$
# $index example, grid table$$
# $index table, user example$$
#
# $code
# $verbatim%example/weight_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def weight_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	# 
	# create a grid table
	ptype    = 'integer primary key'
	col_name = [ 'grid_id', 'grid_name'     ]
	col_type = [ ptype,     'text'          ]
	row_list = [
	           [ 0,         'uniform'       ],
	           [ 1,         'time_constant' ]
	]
	tbl_name = 'grid'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# create a weight table
	col_name = [ 'weight_id', 'grid_id', 'age',  'time',  'weight' ]
	col_type = [ ptype,       'integer', 'real', 'real',  'real'   ]
	row_list = [
	           # uniform
	           [ None,        0,         0.0,    1980.,   1.0      ],
	           [ None,        0,       100.0,    1980.,   1.0      ],
	           [ None,        0,         0.0,    2015.,   1.0      ],
	           [ None,        0,       100.0,    2015.,   1.0      ],
	           # time_consant
	           [ None,        0,         0.0,    1980.,   0.5      ],
	           [ None,        0,       100.0,    1980.,   1.5      ],
	           [ None,        0,         0.0,    2015.,   0.5      ],
	           [ None,        0,       100.0,    2015.,   1.5      ]
	]
	tbl_name = 'weight'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in the uniform weight table
	row_list = list()
	cmd  = 'select age, time, weight from weight'
	cmd += ' inner join grid on weight.grid_id = grid.grid_id'
	cmd += " where grid.grid_name = 'uniform'" 
	for row in cursor.execute(cmd) :
		row_list.append(row)
	# 
	assert row_list[0][0] == 0.0
	assert row_list[0][1] == 1980.
	assert row_list[0][2] == 1.0
	# 
	assert row_list[1][0] == 100.0
	assert row_list[1][1] == 1980.
	assert row_list[1][2] == 1.0
	# 
	assert row_list[2][0] == 0.0
	assert row_list[2][1] == 2015.
	assert row_list[2][2] == 1.0
	# 
	assert row_list[3][0] == 100.0
	assert row_list[3][1] == 2015.
	assert row_list[3][2] == 1.0
	#
	print('weight_table: OK')
# END PYTHON
