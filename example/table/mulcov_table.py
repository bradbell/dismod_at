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
# $begin mulcov_table.py$$ $newlinech #$$
# $spell
#	mulcov
# $$
#
# $section mulcov_table: Example and Test$$
#
# $index mulcov, example$$
# $index example, mulcov table$$
# $index table, mulcov example$$
#
# $code
# $verbatim%example/table/mulcov_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
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
	col_name = col_name2type.keys()
	col_type = col_name2type.values()
	row_list = [ [
		'meas_mean', # muitiplier_type
		-1,          # rate_id (-1 becasue this is an measurement covariate)
		2,           # integrand_id
		1,           # covariate_id
		2            # smooth_id
	],[
		'rate_mean',  # muitiplier_type
		1,            # rate_id
		-1,           # integrand_id (-1 because this is a rate covariate)
		2,            # covariate_id
		2             # smooth_id
	] ]
	tbl_name = 'mulcov'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in the uniform mulcov table
	columns = ','.join(col_name)
	columns = 'mulcov_id,' + columns
	cmd    = 'SELECT ' + columns + ' FROM mulcov'
	cursor = connection.cursor()
	count  = 0
	for row in cursor.execute(cmd) :
		check = row_list[count]
		check.insert(0, count)
		assert len(row) == len(check)
		for j in range( len(row) ) :
			assert row[j] == check[j]
		count += 1
	assert count == len( row_list )
	#
	print('mulcov_table: OK')
# END PYTHON
