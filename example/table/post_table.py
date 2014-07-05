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
# $begin post_table.py$$ $newlinech #$$
#
# $section post_table: Example and Test$$
#
# $index post, , example$$
# $index example, post table$$
# $index table, post example$$
#
# $code
# $verbatim%example/table/post_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def post_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	#
	col_name = [
		# required columns
		'sample_number',
		'variable_type',
		'variable_index',
		'age',
		'time'
	]
	col_type = [
		'integer',              # sample_number
		'text',                 # variable_type
		'integer',              # variable_index
		'real',                 # age
		'real'                  # time
	]
	n_multiplier   = 2
	n_rate         = 8
	row_list       = list()
	sample_number  = 0
	age_grid       = [ 0.0, 100.0]
	time_grid      = [ 1990., 2000.] 
	for i in range(n_multiplier + n_rate) :
		if i < n_multiplier :
			variable_type = 'multilplier'
			variable_index = i
		else :
			variable_type = 'rate'
			variable_index = i - n_multiplier
		for age in age_grid :
			for time in time_grid :
				row_list.append( [
					sample_number,
					variable_type,
					variable_index,
					age,
					time
				] )

	# create the post table
	tbl_name = 'post'
	dismod_at.create_table_(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	columns = ','.join(col_name)
	columns = 'post_id,' + columns
	cmd     = 'SELECT ' + columns + ' FROM post'
	count        = 0
	cursor       = connection.cursor()
	for row in cursor.execute(cmd) :
		check = row_list[count]
		check.insert(0, count)
		assert len(row) == len(check)
		for j in range( len(row) ) :
			assert row[j] == check[j]
		count += 1
	assert count == len( row_list )
	#
	print('post_table: OK')
# END PYTHON
