# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
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
# $code
# $verbatim%example/table/variable_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
def variable_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	#
	col_name = [  'fit_id',  'sample',  'offset', 'value' ]
	col_type = [ 'integer', 'integer', 'integer',  'real' ]
	n_fit          = 2
	n_sample       = 3
	n_offset       = 4
	row_list       = list()
	for fit_id in range(n_fit) :
		for sample in range(n_sample) :
			for offset in range(n_offset) :
				value = float( fit_id + sample + offset )
				row_list.append( [
					fit_id,
					sample,
					offset,
					value
				] )
	# create the variable table
	tbl_name = 'variable'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	# ------------------------------------------------------------------------
	# include primary key in test
	check_name = [ tbl_name + '_id' ] + col_name
	check_list = list()
	for i in range( len(row_list) ) :
		check_list.append( [i] + row_list[i] )
	#
	row_list = dismod_at.get_row_list(connection, tbl_name, check_name)
	assert row_list == check_list
	# ------------------------------------------------------------------------
	print('variable_table: OK')
# END PYTHON
