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
# $begin smooth_grid.py$$ $newlinech #$$
#
# $section smooth_grid: Example and Test$$
#
# $index smooth, example$$
# $index example, smooth table$$
#
# $code
# $verbatim%example/table/smooth_grid.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def smooth_grid() :
	import dismod_at
	import copy
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	# 
	# create smooth table
	ptype    = 'integer primary key'
	col_name = [ 'smooth_name', 'n_age',   'n_time'   ]
	col_type = [ 'text',        'integer', 'integer'  ]
	row_list = [
	           [ 'constant',    1,          1         ],
	           [ 'age_only',    3,          1         ],
	           [ 'time_only',   1,          2         ],
	           [ 'bilinear',    3,          2         ] 
	]
	tbl_name = 'smooth'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# 
	# smooth_grid table column names
	col_name = [
		'smooth_id', 
		'age_id',  
		'time_id',  
		'value_like_id',
		'dage_like_id',
		'dtime_like_id',
	]
	#
	# smooth_grid table column types
	col_type = [
		'integer',  # smooth_id
		'integer',  # age_id
		'integer',  # time_id
		'integer',  # value_like_id
		'integer',  # dage_like_id
		'integer',  # dtime_like_id
	]
	#
	# smooth_grid table values
	row_list = list()
	default  = [
		3,          # smooth_id          (smooth_id == 3 is bilinear)
		None,       # age_id             (age_id  index is 1 in default)
		None,       # time_id            (time_id index is 2 in default)
		1,          # value_like_id
		2,          # dage_like_id
		3           # dtime_like_id
	]
	age_time_list = list()
	for age_id in [0, 1, 2] :             # n_age is 3
		for time_id in [0, 1] :          # n_time is 2
			default[1] = age_id
			default[2] = time_id
			row        = copy.copy(default)
			if age_id == 2 :
				row[4] = -1 # dage_like
			if time_id == 1 :
				row[5] = -1 # dtime_like
			row_list.append( row )
			age_time_list.append( (age_id, time_id) )
	#
	# write the table
	tbl_name = 'smooth_grid'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in the table
	columns = ','.join(col_name)
	columns = 'smooth_grid_id,' + columns
	cmd     = 'SELECT ' + columns + ' FROM smooth_grid'
	cmd    += ' JOIN smooth USING(smooth_id) '
	cmd    += ' WHERE smooth_name = "bilinear"'
	count        = 0
	cursor       = connection.cursor()
	for row in cursor.execute(cmd) :
		assert len(row) == 7
		assert row[0] == count
		assert row[1] == 3
		assert row[2] == age_time_list[count][0]
		assert row[3] == age_time_list[count][1]
		assert row[4] == 1
		if row[2] == 2 :
			assert row[5] == -1
		else :
			assert row[5] == 2
		if row[3] == 1 :
			assert row[6] == -1
		else :
			assert row[6] == 3
		count += 1
	assert count == len(row_list)
	#
	print('smooth_grid: OK')
# END PYTHON
