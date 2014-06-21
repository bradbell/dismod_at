# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2014-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
# $begin weight_table.py$$ $newlinech #$$
#
# $section weight_table: Example and Test$$
#
# $index weight, example$$
# $index example, weight table$$
# $index table, weight example$$
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
	# create name_weight table
	ptype    = 'integer primary key'
	col_name = [ 'weight_id', 'weight_name'   ]
	col_type = [ ptype,       'text'          ]
	row_list = [
	           [ 0,           'constant'      ],
	           [ 1,           'age_linear'    ],
	           [ 2,           'bilinear'      ] 
	]
	tbl_name = 'name_weight'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# create a weight table
	col_name = [ 'weight_primary','weight_id', 'age',  'time',  'weight' ]
	col_type = [ ptype,           'integer',   'real', 'real',  'real'   ]
	row_list = [
	           # constant
	           [ 1,               0,          50.0,    2000.,   1.0      ],
	           # age_linear  
	           [ 2,               1,           0.0,    2000.,   0.5      ],
	           [ 3,               1,         100.0,    2000.,   1.5      ],
	           # bilinear  
	           [ 4,               2,           0.0,    1990.,   0.5      ],
	           [ 5,               2,         100.0,    1990.,   1.0      ],
	           [ 6,               2,           0.0,    2010.,   1.0      ],
	           [ 7,               2,         100.0,    2010.,   1.5      ]
	]
	tbl_name = 'weight'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in the uniform weight table
	columns = ','.join(col_name)
	cmd     = 'SELECT ' + columns + ' FROM weight'
	cmd    += ' INNER JOIN name_weight USING (weight_id)'
	cmd    += ' WHERE weight_name = "bilinear"' 
	#
	count        = 3
	cursor       = connection.cursor()
	for row in cursor.execute(cmd) :
		assert len(row) == len(col_name)
		for j in range( len(row) ) :
			assert row[j] == row_list[count][j]
		count += 1
	assert count == len( row_list )
	#
	print('weight_table: OK')
# END PYTHON
