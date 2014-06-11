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
# $begin create_integrand.py$$ $newlinech #$$
#
# $section create_integrand: Example and Text$$
#
# $index create_integrand, example$$
# $index example, create_integrand$$
#
# $code
# $verbatim%example/create_integrand.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def create_integrand() :
	import dismod_at
	import collections
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	#
	integrand2eta              = collections.OrderedDict()
	integrand2eta['incidence'] = 1e-5
	integrand2eta['remission'] = 2e-5
	integrand2eta['mtall']     = 3e-5
	(col_name,col_type) = dismod_at.create_integrand(connection, integrand2eta)
	assert col_name[0] == 'integrand_id'
	assert col_name[1] == 'integrand_name'
	assert col_name[2] == 'eta'
	#
	assert col_type[0] == 'integer primary key'
	assert col_type[1] == 'text'
	assert col_type[2] == 'real'
	#
	cursor     = connection.cursor()
	# check values in table
	row_list = list()
	for row in cursor.execute('select * from integrand') :
		row_list.append(row)
	for i in range( len(row_list) ) :
		assert row_list[i][0] == i + 1
		check = 1e-5 * float(i + 1)
		assert abs( row_list[i][2] / check - 1.0 ) < 1e-10
	assert row_list[0][1] == 'incidence'
	assert row_list[1][1] == 'remission'
	assert row_list[2][1] == 'mtall'
	print('create_integrand: OK')
# END PYTHON
