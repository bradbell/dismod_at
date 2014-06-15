# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2013-14 University of Washington
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
# $index weight, example$$
# $index example, weight table$$
# $index table, weight example$$
#
# $index user, example$$
# $index example, user table$$
# $index table, user example$$
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
	# create a user table
	ptype    = 'integer primary key'
	col_name = [ 'user_id', 'user_name'     ]
	col_type = [ ptype,     'text'          ]
	row_list = [
	           [ 0,         'iota_prior'    ]
	]
	tbl_name = 'user'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# rate table names
	col_name = [
		'rate_id', 
		'user_id', 
		'age',  
		'time',  
		'rate_lower',
		'rate_upper',
		'rate_mean',
		'rate_std',
		'da_lower',
		'da_upper',
		'da_mean',
		'da_std',
		'dt_lower',
		'dt_upper',
		'dt_mean',
		'dt_std' 
	]
	#
	# rate table types
	col_type = [
		ptype,      # rate_id
		'integer',  # user_id
		'real',     # age
		'real',     # time
		'real',     # rate_lower
		'real',     # rate_upper
		'real',     # rate_mean
		'real',     # rate_std
		'real',     # da_lower
		'real',     # da_upper
		'real',     # da_mean
		'real',     # da_std
		'real',     # dt_lower
		'real',     # dt_upper
		'real',     # dt_mean
		'real'      # dt_std
	]
	#
	# rate table values
	row_list = list()
	inf      = float('inf')
	default  = [
		None,       # rate_id
		0,          # user_id for iota_prior
		None,       # age
		None,       # time
		0.0,        # rate_lower
		1.0,        # rate_upper
		0.1,        # rate_mean
		inf,        # rate_std
		-inf,       # da_lower
		+inf,       # da_upper
		0.0,        # da_mean
		inf,        # da_std
		0.0,        # dt_lower
		1.0,        # dt_upper
		0.1,        # dt_mean
		inf         # dt_std
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
