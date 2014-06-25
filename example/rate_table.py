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
# $section rate_table: Example and Test$$
#
# $index rate, , example$$
# $index example, rate table$$
# $index table, rate example$$
#
# $code
# $verbatim%example/rate_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def rate_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the rate table
	ptype    = 'integer primary key'
	col_name = [ 'rate_id',      'rate_name'  ]
	col_type = [ ptype,          'text'       ]
	row_list = [
	           [ None,           'iota'       ],
	           [ None,           'rho'        ],
	           [ None,           'chi'        ],
	           [ None,           'omega'      ]
	]
	tbl_name = 'rate'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	cmd = 'SELECT rate_id, rate_name FROM rate'
	cursor.execute(cmd)
	fetchall = cursor.fetchall()
	assert len(fetchall) == len(row_list)
	for i in range( len(fetchall) ) :
		row   = row_list[i]
		check = fetchall[i]
		assert len(row) == len(check)
		for j in range( len(check) ) :
			if col_type[j] == ptype :
				# check default value for primary key
				assert check[j] == i + 1
			else :
				assert row[j] == check[j]
	#
	print('rate_table: OK')
# END PYTHON
