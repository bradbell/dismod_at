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
# $begin get_table_dict.py$$ $newlinech #$$
# $spell
#	covariate
# $$
#
# $section get_table_dict: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# BEGIN PYTHON
def get_table_dict() :
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
	table_dict = dismod_at.get_table_dict(connection, tbl_name)
	assert len(table_dict) == n_row
	for i in range(n_row) :
		assert len( table_dict[i] )            == 2
		assert table_dict[i]['covariate_name'] == row_list[i][0]
		assert table_dict[i]['reference']      == row_list[i][1]
	#
	connection.close()
	print('get_table_dict: OK')
# END PYTHON
