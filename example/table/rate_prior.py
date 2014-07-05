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
# $index rate, prior example$$
# $index example, rate prior table$$
# $index prior, rate example$$
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
	# Use smoothing_id = 0 for parent smoothing, smoothing_id = 1 for children.
	# Specify a smoothing for all rates and both parent and children.
	col_name = [ 'rate_id',  'is_parent', 'smooth_id' ]
	col_type = [ 'integer',  'integer',   'integer'   ]
	row_list = [
	           [ 0,               1,       0          ],
	           [ 1,               1,       0          ],
	           [ 2,               1,       0          ],
	           [ 3,               1,       0          ],
	           [ 0,               0,       1          ],
	           [ 1,               0,       1          ],
	           [ 2,               0,       1          ],
	           [ 3,               0,       1          ]
	]
	tbl_name = 'rate_prior'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	columns  = ','.join(col_name)
	columns  = 'rate_prior_id,' + columns
	cmd      = 'SELECT ' + columns + ' FROM rate_prior'
	count        = 0
	cursor       = connection.cursor()
	for row in cursor.execute(cmd) :
		check = row_list[count]
		check.insert(0, count)
		assert len(row) == len(check)
		for j in range( len(row) ) :
			assert row[j] == check[j]
		count += 1
	assert count == len( row_list )
	#
	print('rate_prior: OK')
# END PYTHON
