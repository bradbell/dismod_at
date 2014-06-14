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
# $begin user_weight.py$$ $newlinech #$$
#
# $section user_weight: Example and Text$$
#
# $index weight, example$$
# $index example, weight table$$
# $index table, weight example$$
#
# $index user, example$$
# $index example, user table$$
# $index table, user example$$
#
# $code
# $verbatim%example/user_weight.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def user_weight() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	# 
	# create a user table
	ptype    = 'integer primary key'
	col_name = [ 'user_id', 'user_name'     ]
	col_type = [ ptype,     'text'          ]
	row_list = [
	           [ 0,         'uniform'       ],
	           [ 1,         'time_constant' ]
	]
	tbl_name = 'user'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# create a weight table
	col_name = [ 'weight_id', 'user_id', 'age',  'time',  'weight' ]
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
	cmd += ' inner join user on weight.user_id = user.user_id'
	cmd += " where user.user_name = 'uniform'" 
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
	print('user_weight: OK')
# END PYTHON
