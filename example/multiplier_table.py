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
# $begin multiplier_table.py$$ $newlinech #$$
#
# $section multiplier_table: Example and Text$$
#
# $index multiplier, example$$
# $index example, multiplier table$$
# $index table, multiplier example$$
#
# $code
# $verbatim%example/multiplier_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def multiplier_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	# 
	# create a multiplier table
	ptype    = 'integer primary key'
	col_name = [ 'multiplier_id', 'grid_id', 'age',  'time',  'prior_id' ]
	col_type = [ ptype,           'integer', 'real', 'real',  'integer'  ]
	row_list = [
	           [ 1,               1,          0.0,    1980.,   2         ],
	           [ 2,               1,          0.0,    2015.,   1         ],
	           [ 3,               1,        100.0,    1980.,   2         ],
	           [ 4,               1,        100.0,    2015.,   1         ]
	]
	tbl_name = 'multiplier'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in the uniform multiplier table
	cmd    = 'select multiplier_id,grid_id,age,time,prior_id from multiplier'
	cursor = connection.cursor()
	count  = 0
	for row in cursor.execute(cmd) :
		assert len(row) == len(col_name)
		for j in range( len(row) ) :
			assert row[j] == row_list[count][j]
		count += 1
	assert count == len( row_list )
	#
	print('multiplier_table: OK')
# END PYTHON
