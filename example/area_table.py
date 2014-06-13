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
# $begin area_table.py$$ $newlinech #$$
#
# $section area_table: Example and Text$$
#
# $index area, , example$$
# $index example, area table$$
# $index table, area example$$
#
# $code
# $verbatim%example/area_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def area_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# required columns
	col_name2type  =  {
		'area_id'   : 'integer primary key',
		'area_name' : 'text',
		'parent_id' : 'integer',
	}
	# create the area table
	tbl_name = 'area'
	dismod_at.create_table(connection, tbl_name, col_name2type)
	#
	name_tuple        = '( area_id, area_name, parent_id )'
	value_tuple_list  = [
		"( 0, 'world',         null )",
		"( 1, 'north_america', 0    )",
		"( 2, 'united_states', 1    )",
		"( 3, 'canada',        1    )"
	]
	for value_tuple in value_tuple_list :
		cmd  = 'insert into area '
		cmd +=  name_tuple + ' values ' + value_tuple + ';'
		cursor.execute(cmd)
	#
	# check values in table
	row_list = list()
	cmd      = 'select area_id, area_name, parent_id from area;'
	for row in cursor.execute(cmd) :
		row_list.append(row)
	#
	assert row_list[0][0] == 0
	assert row_list[0][1] == 'world'
	assert row_list[0][2] == None
	#
	assert row_list[1][0] == 1
	assert row_list[1][1] == 'north_america'
	assert row_list[1][2] == 0
	#
	assert row_list[2][0] == 2
	assert row_list[2][1] == 'united_states'
	assert row_list[2][2] == 1
	#
	assert row_list[3][0] == 3
	assert row_list[3][1] == 'canada'
	assert row_list[3][2] == 1
	#
	assert len(row_list) == 4
	#
	print('area_table: OK')
# END PYTHON
