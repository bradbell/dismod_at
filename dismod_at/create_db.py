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
# is a $cref/connection/create_connection/connection/$$ for this database.
#
# $head integrand2eta$$
# is a $code dict$$ or $code collections.OrderedDict$$
# where the keys are $code str$$ human readable names of the 
# integrands the values are $code float$$ offsets in the log transformation 
# (when there is a log transformation in the
# $cref/data likelihood/data_table/likelihood_id/$$).
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
import dismod_at
def create_integrand(connection, integrand2eta ) :
	import collections
	col_name2type = collections.OrderedDict()
	col_name2type[ 'integrand_id' ]   = 'integer primary key'
	col_name2type[ 'integrand_name' ] = 'text'
	col_name2type[ 'eta' ]            = 'real'
	#
	dismod_at.create_table(connection, 'integrand', col_name2type)
	#
	cursor = connection.cursor();
	for name in integrand2eta :
		eta  = integrand2eta[name]
		cmd  = 'insert into integrand values(null,\''
		cmd += name + '\',' + str(eta) + ');'
		cursor.execute(cmd)
	#
	col_name = col_name2type.keys()
	col_type = col_name2type.values()
	return (col_name, col_type)
