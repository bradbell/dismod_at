# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
# $begin replace_table.py$$ $newlinech #$$
# $spell
#	unicode
# $$
#
# $section replace_table: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# BEGIN PYTHON
def replace_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create my table
	col_name = [ 'int_name', 'real_name', 'text_name'  ]
	col_type = [ 'integer',  'real',       'text'      ]
	row_list = [
		[ 1,          2.0,          'three'     ]
	]
	tbl_name = 'my'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# original table
	my_table = dismod_at.get_table_dict(connection, tbl_name)
	#
	# original values
	assert len(my_table)         == 1
	row = my_table[0]
	assert row['int_name']  == 1
	assert row['real_name'] == 2.0
	assert row['text_name'] == 'three'
	#
	# new row in the table
	row = { 'int_name': 2, 'real_name': 3.0, 'text_name': 'four' }
	my_table.append(row)
	dismod_at.replace_table(connection, tbl_name, my_table)
	#
	# check the new table
	new_table = dismod_at.get_table_dict(connection, 'my')
	assert new_table == my_table
	#
	connection.close()
	print('get_name_type: OK')
# END PYTHON
