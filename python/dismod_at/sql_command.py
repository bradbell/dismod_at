# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin sql_command$$ $newlinech #$$
# $spell
#  sql
#  str
#  dismod
# $$
#
# $section Execute an SQL command$$
#
# $head Syntax$$
# $icode%result% = dismod_at.sql_command(%connection%, %command%)
# %$$
#
# $head connection$$
# is a $cref/connection/create_connection/connection/$$ for an
# sql_lite database.
#
# $head command$$
# is a $code str$$ containing the command that is executed.
#
# $head result$$
# Is a list corresponding to the rows returned by the command.
#
# $end
# ---------------------------------------------------------------------------
def sql_command(connection, command) :
   cursor = connection.cursor()
   cursor.execute(command)
   connection.commit()
   return cursor.fetchall()
