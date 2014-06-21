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
# $begin smoothing_table.py$$ $newlinech #$$
#
# $section smoothing_table: Example and Test$$
#
# $index smoothing, example$$
# $index example, smoothing table$$
#
# $code
# $verbatim%example/smoothing_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def smoothing_table() :
	import dismod_at
	import copy
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	# 
	# create name_smoothing table
	ptype    = 'integer primary key'
	col_name = [ 'smoothing_id', 'smoothing_name' ]
	col_type = [ ptype,         'text'          ]
	row_list = [
	           [ 0,             'constant'      ],
	           [ 1,             'age_only'      ],
	           [ 2,             'time_only'     ],
	           [ 3,             'age_time'      ] 
	]
	tbl_name = 'name_smoothing'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# 
	# smoothing table column names
	col_name = [
		'smoothing_primary', 
		'smoothing_id', 
		'age',  
		'time',  
		'value_like_id',
		'dage_like_id',
		'dtime_like_id',
	]
	#
	# smoothing table column types
	col_type = [
		ptype,      # smoothing_primary
		'integer',  # smoothing_id
		'real',     # age
		'real',     # time
		'integer',  # value_like_id
		'integer',  # dage_like_id
		'integer',  # dtime_like_id
	]
	#
	# smoothing table values
	row_list = list()
	default  = [
		None,       # smoothing_primary
		3,          # smoothing_id       (age_time)
		None,       # age                (age  index is 2)
		None,       # time               (time index is 3)
		1,          # value_like_id
		1,          # dage_like_id
		1           # dtime_like_id
	]
	age_grid  = [ 0.0, 50.0, 100.0 ]
	time_grid = [ 1980., 1990., 2000., 2010 ]  
	age_time  = list()
	for age in age_grid :
		for time in time_grid :
			default[2] = age
			default[3] = time
			row_list.append( copy.copy(default) )
			age_time.append( (age, time) )
	#
	# write the table
	tbl_name = 'smoothing'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in the table
	columns = ','.join(col_name)
	cmd     = 'SELECT ' + columns + ' FROM smoothing'
	cmd    += ' JOIN name_smoothing USING(smoothing_id) '
	cmd    += ' WHERE smoothing_name = "age_time"'
	count        = 0
	cursor       = connection.cursor()
	for row in cursor.execute(cmd) :
		assert len(row) == len(col_name)
		for j in range( len(row) ) :
			if j == 2 :
				assert row[j] == age_time[count][0]
			elif j == 3 :
				assert row[j] == age_time[count][1]
			elif j > 3 :
				assert row[j] == row_list[count][j]
		count += 1
	assert count == len(row_list)
	#
	print('smoothing_table: OK')
# END PYTHON
