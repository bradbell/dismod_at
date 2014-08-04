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
# $begin run_table.py$$ $newlinech #$$
#
# $section run_table: Example and Test$$
#
# $index run, , example$$
# $index example, run table$$
# $index table, run example$$
#
# $code
# $verbatim%example/table/run_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def run_table() :
	import dismod_at
	import copy
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the run table
	col_name = [
		'parent_node_id','pini_smooth_id', 'ode_step_size','n_sample'
	]
	col_type = [
		'integer',       'integer',        'real',         'integer' 
	]
	row_list = [ [ 0, 1, 0.5, 100 ] ]
	tbl_name = 'run'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	columns  = ','.join(col_name)
	columns  = 'run_id,' + columns
	cmd      = 'SELECT ' + columns + ' FROM run'
	count        = 0
	cursor       = connection.cursor()
	for row in cursor.execute(cmd) :
		check = copy.copy( row_list[count] )
		check.insert(0, count)
		assert len(row) == len(check)
		for j in range( len(row) ) :
			assert row[j] == check[j]
		count += 1
	assert count == len( row_list )
	#
	print('run_table: OK')
# END PYTHON
