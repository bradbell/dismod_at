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
# $begin prior_table.py$$ $newlinech #$$
#
# $section prior_table: Example and Text$$
#
# $index prior, , example$$
# $index example, prior table$$
# $index table, prior example$$
#
# $code
# $verbatim%example/prior_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def prior_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# create the prior table
	ptype    = 'integer primary key'
	col_name = [ 
		'prior_id', 
		'prior_name',	
		'lower',	
		'upper',	
		'mean',	
		'std',	
		'likelihood_id'  
	]
	col_type = [ 
		ptype,     # prior_id 
		'text',    # prior_name	
		'real',    # lower	
		'real',    # upper	
		'real',    # mean	
		'real',    # std	
		'integer'  # likelihood_id
	]
	row_list = [ [ 
		1,         # prior_id 
		'none',    # prior_name	
		None,      # lower	
		None,      # upper	
		0,         # mean	
		None,      # std	
		1,         # likelihood_id
	],[
		2,         # prior_id 
		'rate',    # prior_name	
		0.0,       # lower	
		1.0,       # upper	
		0,         # mean	
		None,      # std	
		1,         # likelihood_id
	] ]

	tbl_name = 'prior'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in table
	columns = 'prior_id, prior_name, lower, upper, mean, std, likelihood_id'
	cmd = 'select ' + columns + ' from prior'
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
	print('prior_table: OK')
# END PYTHON
