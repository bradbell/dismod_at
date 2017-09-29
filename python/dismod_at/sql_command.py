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
# $begin sql_command$$ $newlinech #$$
# $spell
#	sql
#	str
#	dismod
# $$
#
# $section Execute an SQL command$$
#
# $head Syntax$$
# $codei%dismod_at.sql_command(%connection%, %command%)
# %$$
#
# $head connection$$
# is a $cref/connection/create_connection/connection/$$ for an
# sql_lite database.
#
# $head command$$
# is a $code str$$ containing the command that is executed.
#
# $end
# ---------------------------------------------------------------------------
def sql_command(connection, command) :
	cursor = connection.cursor()
	cursor.execute(command)
	connection.commit()
