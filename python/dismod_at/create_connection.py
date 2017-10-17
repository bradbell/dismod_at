# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin create_connection$$ $newlinech #$$
# $spell
#	str
#	sqlite
#	dismod
# $$
# $index database, connection$$
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
# You must close this connection when you are done with it by executing
# $codei%
#	%connection%.close()
# %$$
#
# $end
# ---------------------------------------------------------------------------
import sqlite3
def create_connection(file_name, new) :
	import os
	import sys
	if new and os.path.isfile(file_name) :
		os.remove(file_name)
	else :
		if not os.path.isfile(file_name) :
			msg = 'create_connection: ' + file_name + ' does not exist.'
			sys.exit(msg)
	connection = sqlite3.connect(file_name)
	return connection
