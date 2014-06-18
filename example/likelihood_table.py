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
# $begin likelihood_table.py$$ $newlinech #$$
#
# $section likelihood_table: Example and Text$$
#
# $index likelihood, , example$$
# $index example, likelihood table$$
# $index table, likelihood example$$
#
# $code
# $verbatim%example/likelihood_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def likelihood_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the likelihood table
	ptype    = 'integer primary key'
	col_name = [ 'likelihood_id', 'likelihood_name'  ]
	col_type = [ ptype,           'text'             ]
	row_list = [
	           [ 1,               'gaussian'         ],
	           [ 2,               'laplace',         ],
	           [ 3,               'log_gaussian',    ],
	           [ 4,               'log_laplace',     ]
	]
	tbl_name = 'likelihood'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	cmd = 'select likelihood_id, likelihood_name from likelihood'
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
	print('likelihood_table: OK')
# END PYTHON
