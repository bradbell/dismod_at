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
# ============================================================================
# $begin create_connection$$ $newlinech #$$
# $spell
#	str
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
# $icode%connection% = dismod_at.create_connection(%file_name%, %new%)
# %$$
#
# $head file_name$$
# is as $code str$$ containing the name of the file where the data base 
# is stored.
# 
# $head new$$
# is either $code True$$ or $code False$$.
# If it is true, and a data base with the same name already exists,
# the existing database is deleted before creating the connection.
#
# $head connection$$
# The return value is an sqlite3
# $href%https://docs.python.org/2/library/sqlite3.html#connection-objects
#	%connection object
# %$$
#
# $end
# ---------------------------------------------------------------------------
def create_connection(file_name, new) :
	import os
	if new and os.path.isfile(file_name) :
		os.remove(file_name)
	connection = sqlite3.connect(file_name)
	return connection
# ==========================================================================-
# $begin create_table$$ $newlinech #$$
# $spell
#	dismod
#	str
# $$
# $index create_table, database$$
# $index table, database create$$
# $index database, create_table$$
#
# $section Create a Database Table$$
#
# $head Syntax$$
# $codei%dismod_at.create_table(%connection%, %col_name2type%)
# %$$
#
# $head connection$$
# is a $cref/connection/create_connection/connection/$$ for this database.
#
# $head table_name$$
# is a $code str$$ that specifies the name of the table.
#
# $head col_name2type$$
# is as $code dict$$, or $code collections.OrderedDict$$,
# where the keys the names of the columns and each value is the type
# for the corresponding column.
# The valid types are 
# $code integer$$, $code real$$, $code text$$, and
# $code integer primary key$$.
# There must be one, and only one, column with type
# $code integer primary key$$.
#
# $end
# ---------------------------------------------------------------------------
def create_table(connection, tbl_name, col_name2type) :
	#
	cmd     = 'create table ' + tbl_name + '('
	count   = 0
	for name in col_name2type :
		cmd   += '\n\t' + name + ' ' + col_name2type[name]
		count += 1
		if count < len(col_name2type) :
			cmd += ','
	cmd += '\n\t);'
	#
	cursor  = connection.cursor()
	cursor.execute(cmd)
