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
# $begin mulcov_table.py$$ $newlinech #$$
# $spell
#	mulcov
# $$
#
# $section mulcov_table: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# BEGIN PYTHON
def mulcov_table() :
	import dismod_at
	import copy
	import collections
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create a mulcov table
	col_name2type = collections.OrderedDict( [
		('mulcov_type',  'text'    ),
		('rate_id',      'integer' ),
		('integrand_id', 'integer' ),
		('covariate_id', 'integer' ),
		('smooth_id',    'integer' )
	] )
	col_name = list(col_name2type.keys())
	col_type = list(col_name2type.values())
	row_list = [ [
		'meas_value', # muitiplier_type
		None,        # rate_id is null becasue measurement covariate
		2,           # integrand_id
		1,           # covariate_id
		2            # smooth_id
	],[
		'rate_value',  # muitiplier_type
		1,            # rate_id
		None,         # integrand_id is null because a rate covariate
		2,            # covariate_id
		2             # smooth_id
	] ]
	tbl_name = 'mulcov'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ----------------------------------------------------------------------
	# include primary key in test
	check_name = [ tbl_name + '_id' ] + col_name
	check_list = list()
	for i in range( len(row_list) ) :
		check_list.append( [i] + row_list[i] )
	#
	row_list = dismod_at.get_row_list(connection, tbl_name, check_name)
	assert row_list == check_list
	# ----------------------------------------------------------------------
	connection.close()
	print('mulcov_table: OK')
# END PYTHON
