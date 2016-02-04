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
# $begin get_name_type.py$$ $newlinech #$$
# $spell
#	unicode
# $$
#
# $section get_name_type: Example and Test$$
#
# $code
# $srcfile%example/table/get_name_type.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
#
def get_name_type() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create temp table
	col_name = [ 'int_name', 'real_name', 'text_name'  ]
	col_type = [ 'integer',  'real',       'text'      ]
	row_list = [
		[ 1,          2.0,          'three'     ]
	]
	tbl_name = 'temp'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	check_name = [ 'temp_id' ] + col_name
	check_type = [ 'integer primary key'] + col_type
	#
	# get the column names and corresponding types
	(col_name, col_type) = dismod_at.get_name_type(connection, tbl_name)
	#
	assert col_name == check_name
	assert col_type == check_type
	#
	connection.close()
	print('get_name_type: OK')
# END PYTHON
