# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Density Estimation as Functions of Age and Time
#           Copyright (C) 2014-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
# $begin density_table.py$$ $newlinech #$$
#
# $section density_table: Example and Test$$
#
# $index density, , example$$
# $index example, density table$$
# $index table, density example$$
#
# $code
# $verbatim%example/table/density_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def density_table() :
	import dismod_at
	import copy
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the density table
	col_name = [ 'density_name'  ]
	col_type = [ 'text'       ]
	row_list = [ ['gaussian'], ['laplace'], ['log_gaussian'], ['log_laplace'] ]
	tbl_name = 'density'
	dismod_at.create_table_(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	cmd = 'SELECT density_id, density_name FROM density'
	cursor.execute(cmd)
	fetchall = cursor.fetchall()
	assert len(fetchall) == len(row_list)
	for i in range( len(fetchall) ) :
		row   = copy.copy( row_list[i] )
		row.insert(0, i)
		check = fetchall[i]
		assert len(row) == len(check)
		for j in range( len(check) ) :
			assert row[j] == check[j]
	#
	print('density_table: OK')
# END PYTHON
