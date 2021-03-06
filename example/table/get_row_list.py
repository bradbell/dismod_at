# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
# $begin get_row_list.py$$ $newlinech #$$
# $spell
#	covariate
# $$
#
# $section get_row_list: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# BEGIN PYTHON
def get_row_list() :
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
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	n_row = len(row_list)
	#
	# reverse the order of the columns
	col_name = [ 'reference',   'covariate_name' ]
	n_col    = len(col_name)
	row_list = dismod_at.get_row_list(connection, tbl_name, col_name)
	#
	assert len(row_list) == n_row
	for i in range(n_row) :
		assert len(row_list[i] ) == n_col
		assert isinstance( row_list[i][0], float)
		assert isinstance( row_list[i][1], str)
	assert row_list[0][0] == 0.0
	assert row_list[0][1] == 'sex'
	assert row_list[1][0] == 2000.0
	assert row_list[1][1] == 'income'
	#
	connection.close()
	print('get_row_list: OK')
# END PYTHON
