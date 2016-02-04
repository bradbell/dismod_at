# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-16 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
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
# $srcfile%example/table/node_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
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
		[ 'world',         None        ],
		[ 'north_america', 0           ],
		[ 'united_states', 1           ],
		[ 'canada',        1           ]
	]
	tbl_name = 'node'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# include primary key in test
	check_name = [ tbl_name + '_id' ] + col_name
	check_list = list()
	for i in range( len(row_list) ) :
		check_list.append( [i] + row_list[i] )
	#
	row_list = dismod_at.get_row_list(connection, tbl_name, check_name)
	assert row_list == check_list
	# ------------------------------------------------------------------------
	connection.close()
	print('node_table: OK')
# END PYTHON
