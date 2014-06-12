# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2013-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
# $begin integrand_table.py$$ $newlinech #$$
#
# $section integrand_table: Example and Text$$
#
# $index integrand, , example$$
# $index example, integrand table$$
# $index table, integrand example$$
#
# $code
# $verbatim%example/integrand_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def integrand_table() :
	import dismod_at
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	#
	# required columns
	col_name2type  =  {
		'integrand_id'   : 'integer primary key',
		'integrand_name' : 'text',
		'eta'            : 'real',
	}
	# create the integrand table
	tbl_name = 'integrand'
	dismod_at.create_table(connection, tbl_name, col_name2type)
	#
	name_tuple        = '( integrand_id, integrand_name, eta )'
	value_tuple_list  = [
		"( null, 'incidence', 1e-5 )",
		"( null, 'remission', 2e-5 )",
		"( null, 'mtall',     3e-5 )"
	]
	for value_tuple in value_tuple_list :
		cmd  = 'insert into integrand '
		cmd +=  name_tuple + ' values ' + value_tuple + ';'
		cursor.execute(cmd)
	#
	# check values in table
	row_list = list()
	cmd      = 'select integrand_id, integrand_name, eta from integrand;'
	for row in cursor.execute(cmd) :
		row_list.append(row)
	for i in range( len(row_list) ) :
		# check default value for primary key
		assert row_list[i][0] == i + 1
		# check value of eta
		check = 1e-5 * float(i + 1)
		assert abs( row_list[i][2] / check - 1.0 ) < 1e-10
	# check value of integrand_name
	assert row_list[0][1] == 'incidence'
	assert row_list[1][1] == 'remission'
	assert row_list[2][1] == 'mtall'
	print('integrand_table: OK')
# END PYTHON
