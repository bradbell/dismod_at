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
# $begin node_table.py$$ $newlinech #$$
#
# $section node_table: Example and Text$$
#
# $index node, , example$$
# $index example, node table$$
# $index table, node example$$
#
# $code
# $verbatim%example/node_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def node_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the node  table
	ptype    = 'integer primary key'
	col_name = [ 'node_id', 'node_name',     'parent_id' ]
	col_type = [ ptype,     'text',          'integer'  ]
	row_list = [
	           [ 0,         'world',         None       ],
		       [ 1,         'north_america', 0          ],
		       [ 2,         'united_states', 1          ],
		       [ 3,         'canada',        1          ]
	]
	tbl_name = 'node'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	columns  = ','.join(col_name)
	cmd      = 'SELECT ' + columns + ' FROM node'
	count        = 0
	cursor       = connection.cursor()
	for row in cursor.execute(cmd) :
		assert len(row) == len(col_name)
		for j in range( len(row) ) :
			assert row[j] == row_list[count][j]
		count += 1
	assert count == len( row_list )
	#
	print('node_table: OK')
# END PYTHON
