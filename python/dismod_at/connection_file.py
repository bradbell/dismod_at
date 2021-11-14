# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin connection_file$$ $newlinech #$$
# $spell
#	str
#	sqlite
#	dismod
# $$
#
# $section Get File Name For a Database Connection$$
#
# $head Syntax$$
# $icode%file_name% = dismod_at.connection_file_name(%connection%)
# %$$
#
# $head Purpose$$
# This function can be used for reporting errors that occur
# when using a database connection.
#
# $head connection$$
# The is a database connection returned by $cref create_connection$$.
#
# $head file_name$$
# The return value is a $code str$$ containing the name of the
# file where the data base is stored.
#
# $children%example/user/connection_file.py
# %$$
# $head Example$$
# The file $cref user_connection_file.py$$ contains an example and test of
# this routine.
#
# $end
# ---------------------------------------------------------------------------
import sqlite3
def connection_file(connection) :
    cursor  = connection.cursor()
    command = 'PRAGMA database_list'
    cursor.execute(command)
    rows      = cursor.fetchall()
    file_name = None
    for row in rows :
        if row[1] == 'main' :
            file_name = row[2]
    return file_name
