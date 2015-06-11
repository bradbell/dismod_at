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
# $begin rate_table.py$$ $newlinech #$$
#
# $section rate_table: Example and Test$$
#
# $index rate, , example$$
# $index example, rate table$$
# $index table, rate example$$
#
# $code
# $verbatim%example/table/rate_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def rate_table() :
	import dismod_at
	import copy
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the rate table
	col_name = [ 'rate_name', 'parent_smooth_id', 'child_smooth_id'  ]
	col_type = [ 'text',      'integer',          'integer'          ]
	row_list = [
		['pini',  0,   1],
		['iota',  2,   3],
		['rho',   2,   3],
		['chi',   2,   3],
		['omega', 2,   3]
	]
	tbl_name = 'rate'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# include primary key in test
	check_name = [ tbl_name + '_id' ] + col_name
	check_type = [ 'integer primary key' ] + col_type
	check_list = list()
	for i in range( len(row_list) ) :
		check_list.append( [i] + row_list[i] )
	#
	row_list = dismod_at.get_row_list(
		connection, tbl_name, check_name, check_type
	)
	assert row_list == check_list
	# ------------------------------------------------------------------------
	print('rate_table: OK')
# END PYTHON
