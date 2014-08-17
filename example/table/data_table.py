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
# $begin data_table.py$$ $newlinech #$$
#
# $section data_table: Example and Test$$
#
# $index data, , example$$
# $index example, data table$$
# $index table, data example$$
#
# $code
# $verbatim%example/table/data_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def data_table() :
	import dismod_at
	import copy
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	#
	col_name = [
		# required columns
		'integrand_id',
		'density_id',
		'node_id',
		'weight_id',
		'meas_value',
		'meas_std',
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
		'integer',              # integrand_id
		'integer',              # density_id
		'integer',              # node_id
		'integer',              # weight_id
		'real',                 # meas_value
		'real',                 # meas_std
		'real',                 # age_lower
		'real',                 # age_upper
		'real',                 # time_lower
		'real',                 # time_upper
		'real',                 # x_sex
		'real',                 # x_income
		'text'                  # c_data_source
	]
	row_list = [ [
		1,                      # integrand_id
		0,                      # density_id
		3,                      # node_id
		4,                      # weight_id
		1e-4,                   # meas_value
		1e-5,                   # meas_std
		10.0,                   # age_lower
		90.0,                   # age_upper
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
	columns = 'data_id,' + columns
	cmd     = 'SELECT ' + columns + ' FROM data'
	count        = 0
	cursor       = connection.cursor()
	for row in cursor.execute(cmd) :
		check = copy.copy( row_list[count] )
		check.insert(0, count)
		assert len(row) == len(check)
		for j in range( len(row) ) :
			assert row[j] == check[j]
		count += 1
	assert count == len( row_list )
	#
	print('data_table: OK')
# END PYTHON
