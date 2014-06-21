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
# $begin multiplier_table.py$$ $newlinech #$$
#
# $section multiplier_table: Example and Test$$
#
# $index multiplier, example$$
# $index example, multiplier table$$
# $index table, multiplier example$$
#
# $code
# $verbatim%example/multiplier_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def multiplier_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	# 
	# create a multiplier table
	col_name = [ 
		'multiplier_id', 
		'multiplier_use',
		'covariate_id', 
		'integrand_id',
		'rate_name',
  		'smoothing_id'
	]
	ptype    = 'integer primary key'
	col_type = [ 
		ptype,     # multiplier_id
		'text',    # multiplier_use
		'integer', # covariate_id
		'integer', # integrand_id
		'text',    # rate_name
  		'integer'  # smoothing_id
	]
	row_list = [ [
		0,      # multiplier_id
		'mean', # muitiplier_use
		1,      # covariate_id
		None,   # integrand_id
		'iota', # rate_name
		2       # smoothing_id
	] ]
	tbl_name = 'multiplier'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in the uniform multiplier table
	columns = ','.join(col_name)
	cmd    = 'SELECT ' + columns + ' FROM multiplier'
	cursor = connection.cursor()
	count  = 0
	for row in cursor.execute(cmd) :
		for j in range( len(row) ) :
			assert row[j] == row_list[count][j]
		count += 1
	assert count == len( row_list )
	#
	print('multiplier_table: OK')
# END PYTHON
