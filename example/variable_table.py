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
# $begin variable_table.py$$ $newlinech #$$
#
# $section variable_table: Example and Test$$
#
# $index variable, , example$$
# $index example, variable table$$
# $index table, variable example$$
#
# $code
# $verbatim%example/variable_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def variable_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the variable table
	ptype    = 'integer primary key'
	col_name = [ 'variable_id', 'variable_type'    ]
	col_type = [ ptype,          'text',           ]
	row_list = [
	           [ None,           'multiplier',     ],
	           [ None,           'parent_iota',    ],
	           [ None,           'parent_rho',     ],
	           [ None,           'parent_chi',     ],
	           [ None,           'parent_omega'    ],
	           [ None,           'child_iota',     ],
	           [ None,           'child_rho',      ],
	           [ None,           'child_chi',      ],
	           [ None,           'child_omega'     ]
	]
	tbl_name = 'variable'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	cmd = 'SELECT variable_id, variable_type FROM variable'
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
	print('variable_table: OK')
# END PYTHON
