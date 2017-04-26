# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
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
# $srcfile%example/table/rate_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
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
	col_name = [
		'rate_name', 'parent_smooth_id', 'child_smooth_id', 'child_nslist_id'
	]
	col_type = [
		'text',      'integer',          'integer',          'integer'
	]
	row_list = [
		['pini',  0,   1,  None],
		['iota',  2,   3,  None],
		['rho',   2,   3,  None],
		['chi',   2,   3,  None],
		['omega', 2,   3,  0   ]
	]
	tbl_name = 'rate'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ----------------------------------------------------------------------
	# include primary key in test
	check_name = [ tbl_name + '_id' ] + col_name
	check_list = list()
	for i in range( len(row_list) ) :
		check_list.append( [i] + row_list[i] )
	#
	row_list = dismod_at.get_row_list(connection, tbl_name, check_name)
	assert row_list == check_list
	# ----------------------------------------------------------------------
	connection.close()
	print('rate_table: OK')
# END PYTHON
