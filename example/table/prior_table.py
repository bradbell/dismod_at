# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
# $begin prior_table.py$$ $newlinech #$$
#
# $section prior_table: Example and Test$$
#
# $index prior, , example$$
# $index example, prior table$$
# $index table, prior example$$
#
# $code
# $srcfile%example/table/prior_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
def prior_table() :
	import dismod_at
	import copy
	import collections
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the prior table
	ptype    = 'integer primary key'
	col_name2type = collections.OrderedDict( [
		('prior_name',  'text'    ),
		('density_id',  'integer' ),
		('lower',       'real'    ),
		('upper',       'real'    ),
		('mean',        'real'    ),
		('std',         'real'    ),
		('eta',         'real'    )
	] )
	col_name = list(col_name2type.keys())
	col_type = list(col_name2type.values())
	uniform_density_id = 0;
	row_list = [ [
		'none',              # prior_name
		uniform_density_id,  # density_id
		None,                # lower
		None,                # upper
		0,                   # mean
		None,                # std
		None                 # eta
	],[
		'rate',              # prior_name
		uniform_density_id,  # density_id
		0.0,                 # lower
		1.0,                 # upper
		0.1,                 # mean
		None,                # std
		None                 # eta
	] ]
	tbl_name = 'prior'
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
	print('prior_table: OK')
# END PYTHON
