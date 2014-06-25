# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2014-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin get_started.py$$ $newlinech #$$
#
# $section Getting Started$$
#
# $index get_started, example$$
# $index example, get_started$$
#
# $code
# $verbatim%example/get_started.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def rate_table() :
	import dismod_at
	#
	# Connect to a new data base
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# start by creating the data file
	col_name = [
		# required columns
		'data_id',
		'integrand_id',
		'location_id',
		'weight_id',
		'meas_value',
		'meas_stdev',
		'meas_density',
		'meas_eta',
		'age_lower',
		'age_upper',
		'time_lower',
		'time_upper',
		# covariates 
		'x_sex',
		'x_income',
		# comments
		'c_data_source',
	]
	col_type = [
		'integer primary key',  # data_id
		'integer',              # integrand_id
		'integer',              # location_id
		'integer',              # weight_id
		'real',                 # meas_value
		'real',                 # meas_stdev
		'text',                 # meas_density
		'real',                 # meas_eta
		'real',                 # age_lower
		'real',                 # age_upper
		'real',                 # time_lower
		'real',                 # time_upper
		'real',                 # x_sex
		'real',                 # x_income
		'text'                  # c_data_source
	]
	row_list = [ [
		0,                      # data_id
		1,                      # integrand_id
		3,                      # location_id
		4,                      # weight_id
		1e-4,                   # meas_value
		1e-5,                   # meas_stdev
		'log_gaussina',         # meas_density
		1e-5,                   # meas_eta
		0.0,                    # age_lower
		100.0,                  # age_upper
		2000.,                  # time_lower
		2005.,                  # time_upper
		0.5,                    # x_sex
		1000.,                  # x_income
		'www.healthdata.org'    # c_data_source
	] ]

	# create the data table
	tbl_name = 'data'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	columns = ','.join(col_name)
	cmd     = 'SELECT ' + columns + ' FROM data'
	count        = 0
	cursor       = connection.cursor()
	for row in cursor.execute(cmd) :
		assert len(row) == len(col_name)
		for j in range( len(row) ) :
			assert row[j] == row_list[count][j]
		count += 1
	assert count == 1
	#
	print('data_table: OK')
# END PYTHON
