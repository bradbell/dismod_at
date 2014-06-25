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
# $begin rate_prior.py$$ $newlinech #$$
#
# $section rate_prior: Example and Test$$
#
# $index rate, , example$$
# $index example, rate table$$
# $index table, rate example$$
#
# $code
# $verbatim%example/table/rate_prior.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def rate_prior() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the rate_prior table
	ptype    = 'integer primary key'
	col_name = [ 'rate_prior_id','is_parent', 'rate_name', 'smooth_grid_id']
	col_type = [ ptype,          'integer',   'text',      'integer'       ]
	row_list = [
	           [ 0,               1,           'iota',     1                ],
	           [ 1,               1,           'rho',      1                ],
	           [ 3,               1,           'chi',      1                ],
	           [ 4,               1,           'omega',    2                ],
	           [ 5,               0,           'iota',     3                ],
	           [ 6,               0,           'rho',      3                ],
	           [ 7,               0,           'chi',      4                ],
	           [ 8,               0,           'omega',    4                ]
	]
	tbl_name = 'rate_prior'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	columns  = ','.join(col_name)
	cmd      = 'SELECT ' + columns + ' FROM rate_prior'
	count        = 0
	cursor       = connection.cursor()
	for row in cursor.execute(cmd) :
		assert len(row) == len(col_name)
		for j in range( len(row) ) :
			assert row[j] == row_list[count][j]
		count += 1
	assert count == len( row_list )
	#
	print('rate_prior: OK')
# END PYTHON
