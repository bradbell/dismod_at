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
# $begin rate_table.py$$ $newlinech #$$
#
# $index rate, example$$
# $index example, rate table$$
#
# $section rate_table: Example and Text$$
# The use of $code None$$ for lower bound,
# upper bounds, and standard deviations is explained 
# $cref/here/table/Infinity and Nan/$$.
#
# $code
# $verbatim%example/rate_table.py%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# BEGIN PYTHON
from __future__ import print_function
def rate_table() :
	import dismod_at
	import copy
	#
	file_name      = 'example.db'
	new            = True
	connection     = dismod_at.create_connection(file_name, new)
	cursor         = connection.cursor()
	# 
	# create a user table
	ptype    = 'integer primary key'
	col_name = [ 'user_id', 'user_name'       ]
	col_type = [ ptype,     'text'            ]
	row_list = [
	           [ 0,         'iota_prior'      ],
	           [ 1,         'rho_prior'       ],
	           [ 2,         'chi_prior'       ],
	           [ 3,         'omega_prior'     ]
	]
	# 
	# create a likelihood table
	col_name = [ 'likelihood_id', 'likelihood_name'  ]
	col_type = [ ptype,     'text'            ]
	row_list = [
	           [ 0,         'gaussian'        ],
	           [ 1,         'laplace'         ],
	           [ 2,         'log_gaussian'    ],
	           [ 3,         'log_laplace'     ]
	]
	tbl_name = 'likelihood'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# rate table names
	col_name = [
		'rate_id', 
		'user_id', 
		'age',  
		'time',  
		'rate_lower',
		'rate_upper',
		'rate_mean',
		'rate_std',
		'rate_likelihood_id',
		'da_lower',
		'da_upper',
		'da_mean',
		'da_std',
		'da_likelihood_id',
		'dt_lower',
		'dt_upper',
		'dt_mean',
		'dt_std',
		'dt_likelihood_id' 
	]
	#
	# rate table types
	col_type = [
		ptype,      # rate_id
		'integer',  # user_id
		'real',     # age
		'real',     # time
		'real',     # rate_lower
		'real',     # rate_upper
		'real',     # rate_mean
		'real',     # rate_std
		'integer',  # rate_likelihood_id
		'real',     # da_lower
		'real',     # da_upper
		'real',     # da_mean
		'real',     # da_std
		'integer',  # da_likelihood_id
		'real',     # dt_lower
		'real',     # dt_upper
		'real',     # dt_mean
		'real',     # dt_std
		'integer'   # dt_likelihood_id
	]
	#
	# rate table values
	row_list = list()
	inf      = float('inf')
	default  = [
		None,       # rate_id
		1,          # user_id            (rho_prior)
		None,       # age                (age  index is 2)
		None,       # time               (time index is 3)
		0.0,        # rate_lower
		1.0,        # rate_upper
		0.1,        # rate_mean
		None,       # rate_std           (plus infinity)
		2,          # rate_likelihood_id (log_gaussian)
		None,       # da_lower           (minus infinity)
		None,       # da_upper           (plus infinity)
		0.0,        # da_mean
		None,       # da_std             (plus infinity)
		2,          # da_likelihood_id   (log_gaussian)
		0.0,        # dt_lower
		1.0,        # dt_upper
		0.1,        # dt_mean
		None,       # dt_std             (plus inifnity)
		2           # dt_likelihood_id   (log_gaussian)
	]
	age_grid  = [ 0.0, 50.0, 100.0 ]
	time_grid = [ 1980., 1990., 2000., 2010 ]  
	age_time  = list()
	for age in age_grid :
		for time in time_grid :
			default[2] = age
			default[3] = time
			row_list.append( copy.copy(default) )
			age_time.append( (age, time) )
	#
	# write the table
	tbl_name = 'rate'
	dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
	#
	# check values in the table
	cmd  = 'select '
	for name in col_name :
		cmd += name
		if name != col_name[-1] :
			cmd += ', ' 
	cmd += ' from rate'
	count        = 0
	cursor       = connection.cursor()
	for row in cursor.execute(cmd) :
		assert len(row) == len(col_name)
		for j in range( len(row) ) :
			if j == 2 :
				assert row[j] == age_time[count][0]
			elif j == 3 :
				assert row[j] == age_time[count][1]
			elif j > 3 :
				assert row[j] == row_list[count][j]
		count += 1
	assert count == len(row_list)
	#
	print('rate_table: OK')
# END PYTHON
