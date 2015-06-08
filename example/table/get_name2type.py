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
# $begin get_name2type.py$$ $newlinech #$$
# $spell
#	unicode
# $$
#
# $section get_name2type: Example and Test$$
#
# $code
# $verbatim%example/table/get_name2type.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
#
def get_name2type() :
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
	# get the column names and corresponding types
	name2type = dismod_at.get_name2type(connection, tbl_name)
	#
	# check results
	count = 0
	for name in name2type :
		if count == 0 :
			assert name == 'temp_id'
			assert name2type[name] == 'integer primary key'
		else :
			assert name == col_name[count-1]
			assert name2type[name] == col_type[count-1]
		count += 1
	#
	print('get_name2type: OK')
# END PYTHON
