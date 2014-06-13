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
# $begin weight_table.py$$ $newlinech #$$
#
# $section weight_table: Example and Text$$
#
# $index weight, , example$$
# $index example, weight table$$
# $index table, weight example$$
#
# $code
# $verbatim%example/weight_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def weight_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	# --------------------------------------------------------------------
	# Create a user table
	col_name2type  =  {
		'user_id'   : 'integer primary key',
		'user_name' : 'text'
	}
	tbl_name = 'user'
	dismod_at.create_table(connection, tbl_name, col_name2type)
	#
	name_tuple    = '( user_id, user_name )'
	value_tuple   = "( 0, 'uniform')"
	cmd  = 'insert into user ' + name_tuple + ' values ' + value_tuple + ';'
	cursor.execute(cmd)
	value_tuple   = "( 1, 'time_constant')"
	cmd  = 'insert into user ' + name_tuple + ' values ' + value_tuple + ';'
	cursor.execute(cmd)
	# ---------------------------------------------------------------------
	# Create a weight table
	# 
	col_name2type  =  {
		'weight_id'   : 'integer primary key',
		'user_id'     : 'integer',
		'age'         : 'real',
		'time'        : 'real',
		'weight'      : 'real'
	}
	tbl_name = 'weight'
	dismod_at.create_table(connection, tbl_name, col_name2type)
	#
	name_tuple        = '( weight_id, user_id, age, time, weight )'
	value_tuple_list  = [
		'( null, 0,   0.0, 1980., 1.0)',
		'( null, 0, 100.0, 1980., 1.0)',
		'( null, 0,   0.0, 2015., 1.0)',
		'( null, 0, 100.0, 2015., 1.0)',
		#
		'( null, 1,   0.0, 1980., 0.5)',
		'( null, 1, 100.0, 1980., 1.5)',
		'( null, 1,   0.0, 2015., 0.5)',
		'( null, 1, 100.0, 2015., 1.5)',
	]
	for value_tuple in value_tuple_list :
		cmd  = 'insert into weight '
		cmd +=  name_tuple + ' values ' + value_tuple + ';'
		cursor.execute(cmd)
	#
	# check values in the uniform weight table
	row_list = list()
	cmd  = 'select age, time, weight from weight'
	cmd += ' inner join user on weight.user_id = user.user_id'
	cmd += " where user.user_name = 'uniform'" 
	for row in cursor.execute(cmd) :
		row_list.append(row)
	# 
	assert row_list[0][0] == 0.0
	assert row_list[0][1] == 1980.
	assert row_list[0][2] == 1.0
	# 
	assert row_list[1][0] == 100.0
	assert row_list[1][1] == 1980.
	assert row_list[1][2] == 1.0
	# 
	assert row_list[2][0] == 0.0
	assert row_list[2][1] == 2015.
	assert row_list[2][2] == 1.0
	# 
	assert row_list[3][0] == 100.0
	assert row_list[3][1] == 2015.
	assert row_list[3][2] == 1.0
	#
	print('weight_table: OK')
# END PYTHON
