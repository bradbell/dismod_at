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
import sqlite3
def create_table(connection, tbl_name, col_name, col_type) :
	#
	cmd     = 'create table ' + tbl_name + '('
	count   = 0
	for col in col_name :
		cmd   += '\n\t' + col + ' ' + col_type[count]
		count += 1
		if count < len(col_name) :
			cmd += ','
	cmd += '\n\t);'
	#
	cursor  = connection.cursor()
	cursor.execute(cmd)
# -------------------------------------------------------------------------- */
# $begin create_connection$$ $newlinech #$$
# $spell
#	sqlite
#	dismod
# $$
# $index create_connection, database$$
# $index connection, database create$$
# $index database, create_connection$$
#
# $section Create a Python sqlite3 Database Connection$$
#
# $head Syntax$$
# $codei%import dismod_at
# %connection% = dismod_at.create_connection(%file_name%, %new%)
# %$$
#
# $head file_name$$
# is as string containing the name of the file where the data base is stored.
# 
# $head new$$
# is either $code True$$ or $code False$$.
# If it is true, and a data base with the same name already exists,
# the existing database is deleted before creating the connection.
#
# $end
# ---------------------------------------------------------------------------
def create_connection(file_name, new) :
	import os
	if new and os.path.isfile(file_name) :
		os.remove(file_name)
	connection = sqlite3.connect(file_name)
	return connection
# ===========================================================================
# $begin create_integrand$$ $newlinech #$$
# $spell
#	dismod
#	integrands
#	str
# $$
# $index create_integrand, table$$
# $index integrand, create table$$
# $index table, create_integrand$$
#
# $section Create and Fill Integrand Table$$
#
# $head Syntax$$
# $codei%(%col_name%, %col_type%) = dismod_at.create_integrand(%$$
# $icode%connection%, %integrand2eta%)%$$
#
# $head connection$$
# is a $cref/connection/create_connection/$$ for this database.
#
# $head integrand2eta$$
# is a $code dict$$ or $code collections.OrderedDict$$
# where the keys are $code str$$ human readable names of the 
# integrands the values are $code float$$ offsets in the log transformation 
# (when there is a log transformation in the
# $cref/data likelihood/create_data/data/likelihood_id/$$).
# If it is an $code OrderedDict$$, the rows in the table have the same 
# order as the dictionary.
#
# $head integrand$$
# The $code integrand$$ table is created in the database specified by 
# $icode connection$$.
#
# $subhead integrand_id$$
# The first column has type $code integer primary key$$. 
# It is intended that $code NULL$$ will be used for the value of this column
# during insertion. The actual value stored will auto-increment;
# i.e., be one larger with each insert. 
#
# $subhead integrand_name$$
# The next column has type $code text$$ and is the human readable
# name corresponding to this integrand.
# 
# $subhead eta$$
# The next column has type $code real$$ and is the offset, when using
# a log transformation with this integrand.
#
# $head col_name$$
# The return value $icode col_name$$ is a list of strings containing
# the column names in the same order as the in the table.
#
# $head col_type$$
# The return value $icode col_type$$ is a list of strings containing
# the column types in the same order as the columns in the table.
#
# $children%example/create_integrand.py
# %$$
# $head Example$$
# The file $cref create_integrand.py$$ contains an example and test
# of this operation.
# 
# $end
# ---------------------------------------------------------------------------
def create_integrand(connection, integrand2eta ) :
	col_name = ['integrand_id',        'integrand_name', 'eta' ]
	col_type = ['integer primary key', 'text',           'real' ]
	#
	create_table(connection, 'integrand', col_name, col_type)
	#
	cursor = connection.cursor();
	for name in integrand2eta :
		eta  = integrand2eta[name]
		cmd  = 'insert into integrand values(null,\''
		cmd += name + '\',' + str(eta) + ');'
		cursor.execute(cmd)
	#
	return (col_name, col_type)
# ===========================================================================
# $begin create_data$$ $newlinech #$$
# $spell
#	covariate
#	covariates
#	sqlalchemy
#	stdev
#	len
#	dismod
# $$
# $index create_data, table$$
# $index data, create table$$
# $index table, create_data$$
#
# $section Create An Empty Data Table$$
# 
# $head Syntax$$
# $codei%(%col_name%, %col_type%) = dismod_at.create_data(%$$
# $icode%connection%, %covariate_list%)%$$
#
# $head connection$$
# is a $cref/connection/create_connection/$$ for this database.
#
# $head covariate_list$$
# is a list of strings where each string is the name 
# of a covariate that will correspond to a column in the data file.
# Each covariate name must begin with the characters $code c_$$.
#
# $head data$$
# The $code data$$ table is created in the database specified by 
# $icode connection$$.
#
# $subhead data_id$$
# The first column has type $code integer primary key$$. 
# It is intended that $code NULL$$ will be used for the value of this column
# during insertion. The actual value stored will auto-increment;
# i.e., be one larger with each insert. 
#
# $subhead integrand_id$$
# The next column has type $code integer$$ and is the 
# $cref/integrand_id/create_integrand/integrand/integrand_id/$$
# for this row by the primary key in the $code integrand$$ table.
#
# $subhead likelihood_id$$
# The next column has type $code integer$$ and identifies the data likelihood
# for this row by the primary key in the $code likelihood$$ table.
#
# $subhead location_id$$
# The next column has type $code integer$$ and identifies the location
# for this row by the primary key in the $code location$$ table.
#
# $subhead meas_value$$
# The next column has type $code real$$ and is the measured value
# for this row; i.e., the measurement of the integrand at the specified
# location etc. 
#
# $subhead meas_stdev$$
# The next column has type $code real$$ and is the standard deviation 
# corresponding to the measurement value.
# This standard for the data and is in the same units at the data.
#
# $subhead age_lower$$
# The next column has type $code real$$ and is the lower age limit 
# for this measurement and must be between the 
# minimum and maximum age specified by the $code age_grid$$ table.
#
# $subhead age_upper$$
# The next column has type $code real$$ and is the upper age limit
# for this measurement and must be between the 
# minimum and maximum age specified by the $code age_grid$$ table.
#
# $subhead time_lower$$
# The next column has type $code real$$ and is the lower time limit 
# for this measurement and must be between the 
# minimum and maximum time specified by the $code time_grid$$ table.
#
# $subhead time_upper$$
# The next column has type $code real$$ and is the upper time limit
# for this measurement and must be between the 
# minimum and maximum time specified by the $code time_grid$$ table.
#
# $subhead Covariates$$
# The covariate columns follow in the same order as
# $codei%len(%covariate_list%)%$$ and have type $code real$$. 
# The corresponding values in the table are the covariates for this row.
#
# $head col_name$$
# The return value $icode col_name$$ is a list of strings containing
# the column names in the same order as the in the table.
#
# $head col_type$$
# The return value $icode col_type$$ is a list of strings containing
# the column types in the same order as the columns in the table.
#
# $children%example/create_data.py
# %$$
# $head Example$$
# The file $cref create_data.py$$ contains an example and test
# of this operation.
#
# $end
# ---------------------------------------------------------------------------
def create_data(connection , covariate_list ) :
	import collections
	schema  = collections.OrderedDict()
	schema['data_id']       = 'integer primary key'
	schema['integrand_id']  = 'integer'
	schema['likelihood_id'] = 'integer'
	schema['location_id']   = 'integer'
	schema['meas_value']    = 'real'
	schema['meas_stdev']    = 'real'
	schema['age_lower']     = 'real'
	schema['age_upper']     = 'real'
	schema['time_lower']    = 'real'
	schema['time_upper']    = 'real'
	for covariate in covariate_list : 
		assert covariate.startswith('c_')
		schema[covariate]   = 'real'
	#
	col_name  = schema.keys()
	col_type = schema.values()
	#
	create_table(connection, 'data', col_name, col_type)
	#
	return (col_name, col_type)
# ===========================================================================
