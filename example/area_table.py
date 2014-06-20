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
# $begin area_table.py$$ $newlinech #$$
#
# $section area_table: Example and Text$$
#
# $index area, , example$$
# $index example, area table$$
# $index table, area example$$
#
# $code
# $verbatim%example/area_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def area_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the area  table
	col_name = [
		'area_id', 
		'area_name',
		'parent_id', 
		'iota_smoothind_id',
		'rho_smoothing_id',
		'chi_smoothing_id',
		'omega_smoothing_id'
	]
	ptype    = 'integer primary key'
	col_type = [
		ptype,      # area_id
		'text',     # area_name
		'integer',  # parent_id
		'integer',  # iota_smoothind_id
		'integer',  # rho_smoothing_id
		'integer',  # chi_smoothing_id
		'integer'   # omega_smoothing_id
	]
	row_list = [
		[ 0,  'world',         None, 1, 1, 1, 1 ],
		[ 1,  'north_america', 0,    1, 1, 1, 1 ],
		[ 2,  'united_states', 1,    1, 1, 1, 1 ],
		[ 3,  'canada',        1,    1, 1, 1, 1 ]
	]
	tbl_name = 'area'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	columns  = ','.join(col_name)
	cmd      = 'SELECT ' + columns + ' FROM area'
	count        = 0
	cursor       = connection.cursor()
	for row in cursor.execute(cmd) :
		assert len(row) == len(col_name)
		for j in range( len(row) ) :
			assert row[j] == row_list[count][j]
		count += 1
	assert count == len( row_list )
	#
	print('area_table: OK')
# END PYTHON
