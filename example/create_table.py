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
# $section create_table and Unicode: Example and Text$$
#
# $code
# $verbatim%example/create_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def create_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	col_name2type  =  {
		'temp_id'   : 'integer primary key',
		'temp_name' : 'text'
	}
	# create the integrand table
	tbl_name = 'temp'
	dismod_at.create_table(connection, tbl_name, col_name2type)
	#
	inverted_exclamation = unichr( 10 * 16 + 1 ) # 00a1
	cent_sign            = unichr( 10 * 16 + 2 ) # 00a2
	pound_sign           = unichr( 10 * 16 + 3 ) # 00a3
	name_tuple        = '( temp_id, temp_name )'
	value_tuple_list  = [ 
		"( null, '" + inverted_exclamation + "' )",
		"( null, '" + cent_sign            + "' )",
		"( null, '" + pound_sign           + "' )"
	]
	for value_tuple in value_tuple_list :
		cmd  = 'insert into temp '
		cmd +=  name_tuple + ' values ' + value_tuple + ';'
		cursor.execute(cmd)
	#
	# check values in table
	row_list = list()
	cmd      = 'select temp_id, temp_name from temp;'
	for row in cursor.execute(cmd) :
		row_list.append(row)
	for i in range( len(row_list) ) :
		# check default value for primary key
		assert row_list[i][0] == i + 1
	# check value of temp_name
	assert row_list[0][1] == inverted_exclamation
	assert row_list[1][1] == cent_sign
	assert row_list[2][1] == pound_sign
	print('create_table: OK')
# END PYTHON
