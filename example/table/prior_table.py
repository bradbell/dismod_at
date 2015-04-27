# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
# 	     GNU Affero General Public License version 3.0 or later
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
# $verbatim%example/table/prior_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def prior_table() :
	import dismod_at
	import copy
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the prior table
	ptype    = 'integer primary key'
	col_name = [
		'prior_name', 'density_id', 'lower', 'upper', 'mean', 'std',   'eta'
	]
	col_type = [
		'text',      'integer',    'real',  'real',  'real', 'real',  'real'
	]
	uniform_density_id = 0;
	row_list = [ [
		'none',              # prior_name
		uniform_density_id,   # density_id
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
	#
	# check values in table
	columns = ','.join(col_name)
	columns = 'prior_id,' + columns
	cmd     = 'SELECT ' + columns + ' FROM prior'
	cursor.execute(cmd)
	fetchall = cursor.fetchall()
	assert len(fetchall) == len(row_list)
	for i in range( len(fetchall) ) :
		row   = copy.copy(row_list[i])
		row.insert(0, i)
		check = fetchall[i]
		assert len(row) == len(check)
		for j in range( len(check) ) :
			assert row[j] == check[j]
	#
	print('prior_table: OK')
# END PYTHON
