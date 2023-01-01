# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin connection_file$$ $newlinech #$$
# $spell
#  str
#  sqlite
#  dismod
# $$
#
# $nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section Get File Name For a Database Connection$$
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
