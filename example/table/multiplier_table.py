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
# $verbatim%example/table/multiplier_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def multiplier_table() :
	import dismod_at
	import copy
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	# 
	# create a multiplier table
	col_name = [ 
		'multiplier_type',
		'rate_id',
		'integrand_id',
		'covariate_id', 
  		'smooth_id'
	]
	col_type = [ 
		'text',    # multiplier_type
		'integer', # rate_id
		'integer', # integrand_id
		'integer', # covariate_id
  		'integer'  # smooth_id'
	]
	row_list = [ [
		'mean', # muitiplier_type
		-1,     # rate_id (-1 becasue this is an integrand multiplier)
		2,      # integrand_id
		1,      # covariate_id
		2       # smooth_id
	],[
		'rate', # muitiplier_type
		1,      # rate_id 
		-1,     # integrand_id (-1 because this is a rate multiplier)
		2,      # covariate_id
		2       # smooth_id
	] ]
	tbl_name = 'multiplier'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in the uniform multiplier table
	columns = ','.join(col_name)
	columns = 'multiplier_id,' + columns
	cmd    = 'SELECT ' + columns + ' FROM multiplier'
	cursor = connection.cursor()
	count  = 0
	for row in cursor.execute(cmd) :
		check = row_list[count]
		check.insert(0, count)
		assert len(row) == len(check)
		for j in range( len(row) ) :
			assert row[j] == check[j]
		count += 1
	assert count == len( row_list )
	#
	print('multiplier_table: OK')
# END PYTHON
