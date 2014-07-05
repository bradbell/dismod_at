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
# $begin covariate_table.py$$ $newlinech #$$
# $spell
# 	covariate
# $$
#
# $section covariate_table: Example and Test$$
#
# $index covariate, table example$$
# $index example, covariate table$$
# $index table, covariate example$$
#
# $code
# $verbatim%example/table/covariate_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def covariate_table() :
	import dismod_at
	import copy
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the covariate table
	col_name = [ 'covariate_name',	'reference' ]
	col_type = [ 'text',             'real'     ] 
	row_list = [
	           [ 'sex',              0.0        ],
	           [ 'income',           2000.0     ]
	]
	tbl_name = 'covariate'
	dismod_at.create_table_(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	columns = ','.join(col_name)
	columns = 'covariate_id,' + columns
	cmd     = 'SELECT ' + columns + ' FROM covariate'
	cursor.execute(cmd)
	fetchall = cursor.fetchall()
	assert len(fetchall) == len(row_list)
	for i in range( len(fetchall) ) :
		row   = copy.copy( row_list[i] )
		row.insert(0, i)
		check = fetchall[i]
		assert len(row) == len(check)
		for j in range( len(check) ) :
			assert row[j] == check[j]
	#
	print('covariate_table: OK')
# END PYTHON
