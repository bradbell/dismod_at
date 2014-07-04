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
# $section node_table: Example and Test$$
#
# $index node, , example$$
# $index example, node table$$
# $index table, node example$$
#
# $code
# $verbatim%example/table/node_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
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
	# create the node table
	col_name = [ 'node_name',     'parent'    ]
	col_type = [ 'text',          'integer'   ]
	row_list = [
	           [ 'world',         -1          ],
		       [ 'north_america', 0           ],
		       [ 'united_states', 1           ],
		       [ 'canada',        1           ]
	]
	tbl_name = 'node'
	dismod_at.create_table_(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	columns  = ','.join(col_name)
	columns  = 'node_id,' + columns
	cmd      = 'SELECT ' + columns + ' FROM node'
	count    = 0
	cursor   = connection.cursor()
	for row in cursor.execute(cmd) :
		row_list[count].insert(0, count)
		assert len(row) == len(col_name) + 1
		for j in range( len(row) ) :
			assert row[j] == row_list[count][j]
		count += 1
	assert count == len( row_list )
	#
	print('node_table: OK')
# END PYTHON
