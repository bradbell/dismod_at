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
# $section covariate_table: Example and Text$$
#
# $index covariate, table example$$
# $index example, covariate table$$
# $index table, covariate example$$
#
# $code
# $verbatim%example/covariate_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def covariate_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the covariate table
	ptype    = 'integer primary key'
	col_name = [ 'covariate_id', 'covariate_name',	'reference' ]
	col_type = [ ptype,          'text',             'real'     ] 
	row_list = [
	           [ 1,              'sex',              0.0        ],
	           [ 2,              'income',           2000.0     ]
	]
	tbl_name = 'covariate'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	columns = ','.join(col_name)
	cmd     = 'SELECT ' + columns + ' FROM covariate'
	cursor.execute(cmd)
	fetchall = cursor.fetchall()
	assert len(fetchall) == len(row_list)
	for i in range( len(fetchall) ) :
		row   = row_list[i]
		check = fetchall[i]
		assert len(row) == len(check)
		for j in range( len(check) ) :
			assert row[j] == check[j]
	#
	print('covariate_table: OK')
# END PYTHON
