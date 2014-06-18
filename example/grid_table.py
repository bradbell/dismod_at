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
# $begin grid_table.py$$ $newlinech #$$
#
# $section grid_table: Example and Text$$
#
# $index user, , example$$
# $index example, grid table$$
# $index table, user example$$
#
# $code
# $verbatim%example/grid_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def grid_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the grid table
	ptype    = 'integer primary key'
	col_name = [ 'grid_id', 'grid_name'  ]
	col_type = [ ptype,           'text' ]
	row_list = [
	           # rate priors 
	           [ 1,               'iota_prior'        ],
	           [ 2,               'rho_prior',        ],
	           [ 3,               'chi_prior',        ],
	           [ 4,               'omega_prior',      ],
	          # weighting fucntions
	           [ 5,               'constant'          ],
	           [ 6,               'age_constant'      ],
	           [ 7,               'time_constant'     ]
	]
	tbl_name = 'grid'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	cmd = 'select grid_id, grid_name from grid'
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
	print('grid_table: OK')
# END PYTHON
