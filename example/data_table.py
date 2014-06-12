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
# $begin data_table.py$$ $newlinech #$$
#
# $section data_table: Example and Text$$
#
# $index data, , example$$
# $index example, data table$$
# $index table, data example$$
#
# $code
# $verbatim%example/data_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def data_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	#
	# required columns
	col_name2type  =  {
		# required columns
		'data_id'       : 'integer primary key',
		'integrand_id'  : 'integer',
		'likelihood_id' : 'integer',
		'location_id'   : 'integer',
		'weighting_id'  : 'integer',
		'meas_value'    : 'real',
		'meas_stdev'    : 'real',
		'age_lower'     : 'real',
		'age_upper'     : 'real',
		'time_lower'    : 'real',
		'time_upper'    : 'real',
		# covariates 
		'x_sex'         : 'real',
		'x_income'      : 'real',
		# comments
		'c_data_source' : 'text'
	}
	# create the data table
	tbl_name = 'data'
	dismod_at.create_table(connection, tbl_name, col_name2type)
	#
	col_name  = col_name2type.keys()
	col_value = list()
	index     = 0
	for name in col_name :
		if name == 'data_id' :
			# primary key
			col_value.append(None)
		elif name == 'c_data_source' :
			col_value.append('www.healthdata.org')
		elif col_name2type[name] == 'integer' :
			col_value.append(index)
		else :
			assert col_name2type[name] == 'real'
			col_value.append(index + 0.5 )
		index += 1
	quote_string = False
	name_tuple   = dismod_at.unicode_tuple(col_name, quote_string)
	quote_string = True
	value_tuple  = dismod_at.unicode_tuple(col_value, quote_string)
	cmd = 'insert into data' + name_tuple + ' values ' + value_tuple + ';'
	#
	cursor     = connection.cursor()
	cursor.execute(cmd)
	#
	# check values in table
	for row in cursor.execute('select * from data') :
		index = 0
		for name in col_name :
			if name == 'data_id' :
				# primary key starts at one, but where is this specified ?
				assert row[index] == 1 
			elif name == 'c_data_source' :
				assert row[index] == 'www.healthdata.org'
			elif col_name2type[name] == 'integer' :
				assert row[index] == index
			else :
				assert col_name2type[name] == 'real'
			index += 1
	print('data_table: OK')
# END PYTHON
