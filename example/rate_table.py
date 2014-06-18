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
# $begin rate_table.py$$ $newlinech #$$
#
# $section rate_table: Example and Text$$
#
# $index rate, example$$
# $index example, rate table$$
#
# $code
# $verbatim%example/rate_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def rate_table() :
	import dismod_at
	import copy
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	# 
	# rate table names
	col_name = [
		'rate_id', 
		'grid_id', 
		'age',  
		'time',  
		'rate_prior_id',
		'dage_prior_id',
		'dtime_prior_id',
	]
	#
	# rate table types
	ptype = 'integer primary key'
	col_type = [
		ptype,      # rate_id
		'integer',  # grid_id
		'real',     # age
		'real',     # time
		'integer',  # rate_prior_id
		'integer',  # dage_prior_id
		'integer',  # dtime_prior_id
	]
	#
	# rate table values
	row_list = list()
	inf      = float('inf')
	default  = [
		None,       # rate_id
		1,          # grid_id            (rho_prior)
		None,       # age                (age  index is 2)
		None,       # time               (time index is 3)
		1,          # rate_prior_id
		1,          # dage_prior_id
		1           # dtime_prior_id
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
	tbl_name = 'rate'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in the table
	cmd  = 'select '
	for name in col_name :
		cmd += name
		if name != col_name[-1] :
			cmd += ', ' 
	cmd += ' from rate'
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
	print('rate_table: OK')
# END PYTHON
