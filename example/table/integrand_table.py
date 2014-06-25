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
# $begin integrand_table.py$$ $newlinech #$$
#
# $section integrand_table: Example and Test$$
#
# $index integrand, , example$$
# $index example, integrand table$$
# $index table, integrand example$$
#
# $code
# $verbatim%example/table/integrand_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def integrand_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the integrand table
	ptype    = 'integer primary key'
	col_name = [ 'integrand_id', 'integrand_name' ]
	col_type = [ ptype,          'text',          ]
	row_list = [
	           [ None,           'incidence',     ],
	           [ None,           'remission',     ],
	           [ None,           'mtall',         ]
	]
	tbl_name = 'integrand'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	cmd = 'SELECT integrand_id, integrand_name FROM integrand'
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
	print('integrand_table: OK')
# END PYTHON
