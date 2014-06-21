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
	ptype                = 'integer primary key'
	inverted_exclamation = unichr( 10 * 16 + 1 ) # 00a1
	cent_sign            = unichr( 10 * 16 + 2 ) # 00a2
	pound_sign           = unichr( 10 * 16 + 3 ) # 00a3
	#
	# create table
	name_tuple        = '( temp_id, temp_name )'
	col_name = [ 'temp_id', 'temp_name'          ]
	col_type = [ ptype,     'text'               ]
	row_list = [ 
	           [ None,      inverted_exclamation ],
	           [ None,      cent_sign            ],
	           [ None,      pound_sign           ]
	]
	tbl_name = 'temp'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	row_list = list()
	cmd      = 'SELECT temp_id, temp_name FROM temp'
	for row in cursor.execute(cmd) :
		row_list.append(row)
	for i in range( len(row_list) ) :
		# check default value for primary key (where is this specified ?)
		assert row_list[i][0] == i + 1
	# check value of temp_name
	assert row_list[0][1] == inverted_exclamation
	assert row_list[1][1] == cent_sign
	assert row_list[2][1] == pound_sign
	#
	print('create_table: OK')
# END PYTHON
