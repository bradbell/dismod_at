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
# $begin fit_table.py$$ $newlinech #$$
#
# $section fit_table: Example and Test$$
#
# $code
# $verbatim%example/table/fit_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def fit_table() :
	import dismod_at
	import copy
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the fit table
	col_name = [
		'parent_node_id','ode_step_size','tolerance'
	]
	col_type = [
		'integer',       'real',         'real'
	]
	row_list = [ [ 0, 0.5, 100 ] ]
	tbl_name = 'fit'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# include primary key in test
	check_name = [ tbl_name + '_id' ] + col_name
	check_list = list()
	for i in range( len(row_list) ) :
		check_list.append( [i] + row_list[i] )
	#
	row_list = dismod_at.get_row_list(connection, tbl_name, check_name)
	assert row_list == check_list
	# ------------------------------------------------------------------------
	print('fit_table: OK')
# END PYTHON
