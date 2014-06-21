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
# $begin misc_table.py$$ $newlinech #$$
#
# $section misc_table: Example and Text$$
#
# $index misc, , example$$
# $index example, misc table$$
# $index table, misc example$$
#
# $code
# $verbatim%example/misc_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def misc_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the misc  table
	ptype    = 'integer primary key'
	col_name = [ 'misc_id','misc_name',          'misc_integer', 'misc_real' ]
	col_type = [ ptype,    'text',               'integer',      'real'      ]
	row_list = [
	           [ 0,        'parent_node_id',     1,              None        ],
	           [ 1,        'parent_smoothing_id',2,              None        ],
	           [ 1,        'child_smoothing_id', 3,              None        ] 
	]
	tbl_name = 'misc'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	columns  = ','.join(col_name)
	cmd      = 'SELECT ' + columns + ' FROM misc'
	count        = 0
	cursor       = connection.cursor()
	for row in cursor.execute(cmd) :
		assert len(row) == len(col_name)
		for j in range( len(row) ) :
			assert row[j] == row_list[count][j]
		count += 1
	assert count == len( row_list )
	#
	print('misc_table: OK')
# END PYTHON
