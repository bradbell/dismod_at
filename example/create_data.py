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
# $begin create_data.py$$ $newlinech #$$
#
# $section create_data: Example and Text$$
#
# $index create_data, example$$
# $index example, create_data$$
#
# $code
# $verbatim%example/create_data.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def create_data() :
	import dismod_at
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	#
	covariate_list         = [ 'c_sex', 'c_income' ]
	(col_name, col_type) = dismod_at.create_data(connection, covariate_list)
	#
	# place values in table
	index      = 0
	cmd        = 'insert into data values('
	for col in col_name :
		if col == 'data_id':
			# primary key
			cmd += 'null'
		elif col_type[index] == 'integer' :
			cmd += str(index)
		else :
			cmd += str( index + 0.5 )
		index += 1
		if index < len(col_name) :
			cmd += ','
	cmd += ');'
	#
	cursor     = connection.cursor()
	cursor.execute(cmd)
	#
	# check values in table
	for row in cursor.execute('select * from data') :
		index = 0
		for col in col_name :
			if col == 'data_id' :
				# primary key starts at one, but where is this specified ?
				assert row[index] == 1 
			elif col_type[index] == 'integer' :
				assert row[index] == index
			else :
				assert col_type[index] == 'real'
				assert row[index] == index + 0.5
			index += 1
	print('create_data: OK')
# END PYTHON
