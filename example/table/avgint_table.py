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
# $begin avgint_table.py$$ $newlinech #$$
# $spell
#	avgint
# $$
#
# $section avgint_table: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# BEGIN PYTHON
def avgint_table() :
	import dismod_at
	import copy
	import collections
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	#
	col_name2type = collections.OrderedDict([
		# required columns
		('integrand_id', 'integer'),
		('density_id',   'integer'),
		('node_id',      'integer'),
		('weight_id',    'integer'),
		('age_lower',    'real'   ),
		('age_upper',    'real'   ),
		('time_lower',   'real'   ),
		('time_upper',   'real'   ),
		# covariates
		('x_sex',        'real'   ),
		('x_income',     'real'   ),
	] )
	col_name = list(col_name2type.keys())
	col_type = list(col_name2type.values())
	row_list = [ [
		1,                      # integrand_id
		0,                      # density_id
		3,                      # node_id
		4,                      # weight_id
		10.0,                   # age_lower
		90.0,                   # age_upper
		2000.,                  # time_lower
		2005.,                  # time_upper
		0.5,                    # x_sex
		1000.                  # x_income
	] ]

	# create the avgint table
	tbl_name = 'avgint'
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
	print('avgint_table: OK')
# END PYTHON
