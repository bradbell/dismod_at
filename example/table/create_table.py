# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
# $begin create_table.py$$ $newlinech #$$
# $spell
#	unicode
# $$
#
# $index unicode, , example$$
# $index example, unicode table$$
# $index create_table, , example$$
# $index example, create_table$$
#
# $section create_table and Unicode: Example and Test$$
#
# $code
# $verbatim%example/table/create_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
def create_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	inverted_exclamation = chr( 10 * 16 + 1 ) # 00a1
	cent_sign            = chr( 10 * 16 + 2 ) # 00a2
	pound_sign           = chr( 10 * 16 + 3 ) # 00a3
	#
	# create table
	col_name = [ 'temp_name'          ]
	col_type = [ 'text'               ]
	row_list = [
	           [ inverted_exclamation ],
	           [ cent_sign            ],
	           [ pound_sign           ]
	]
	tbl_name = 'temp'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	row_list = list()
	cmd      = 'SELECT temp_id, temp_name FROM temp'
	for row in cursor.execute(cmd) :
		row_list.append(row)
	#
	for i in range( len(row_list) ) :
		assert row_list[i][0] == i
	#
	assert row_list[0][1] == inverted_exclamation
	assert row_list[1][1] == cent_sign
	assert row_list[2][1] == pound_sign
	#
	connection.close()
	print('create_table: OK')
# END PYTHON
