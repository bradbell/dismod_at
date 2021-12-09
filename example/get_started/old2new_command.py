# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin old2new_command.py$$ $newlinech #$$
# $spell
#	var
# $$
#
# $section old2new Command: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
# ---------------------------------------------------------------------------
# check execution is from distribution directory
example = 'example/get_started/old2new_command.py'
if sys.argv[0] != example  or len(sys.argv) != 1 :
	usage  = 'python3 ' + example + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
#
# import dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
	sys.path.insert(0, local_dir)
import dismod_at
#
# import get_started_db example
sys.path.append( os.getcwd() + '/example/get_started' )
import get_started_db
#
# change into the build/example/get_started directory
if not os.path.exists('build/example/get_started') :
    os.makedirs('build/example/get_started')
os.chdir('build/example/get_started')
# ---------------------------------------------------------------------------
# create get_started.db
get_started_db.get_started_db()
# -----------------------------------------------------------------------
# connect to database
new            = False
file_name      = 'get_started.db'
connection     = dismod_at.create_connection(file_name, new)
subgroup_table = dismod_at.get_table_dict(connection, 'subgroup')
#
# Check that there is only one row in subgroup table
assert len(subgroup_table) == 1
#
# Drop subgroup table
sql_cmd = 'DROP TABLE subgroup'
dismod_at.sql_command(connection, sql_cmd)
#
# Drop subgroup_id column from data table
sql_cmd = 'ALTER TABLE data RENAME COLUMN subgroup_id TO not_used'
dismod_at.sql_command(connection, sql_cmd)
#
# Drop subgroup_id column from avgint table
sql_cmd = 'ALTER TABLE avgint RENAME COLUMN subgroup_id TO not_used'
dismod_at.sql_command(connection, sql_cmd)
#
# Drop group_id column from mulcov table
sql_cmd = 'ALTER TABLE mulcov RENAME COLUMN group_id TO not_used_one'
dismod_at.sql_command(connection, sql_cmd)
#
# Drop subgroup_smooth_id column from mulcov table
sql_cmd = 'ALTER TABLE mulcov RENAME COLUMN subgroup_smooth_id TO not_used_two'
dismod_at.sql_command(connection, sql_cmd)
#
# Rename group_smooth_id column to smooth_id in mulcov table
sql_cmd = 'ALTER TABLE mulcov RENAME COLUMN group_smooth_id TO smooth_id'
dismod_at.sql_command(connection, sql_cmd)
#
# close the database
connection.close()
#
# use old2new_command to restore the database
program   = '../../devel/dismod_at'
file_name = 'get_started.db'
command   = 'old2new'
dismod_at.system_command_prc( [ program, file_name, command ] )
#
# check that restore worked by running init command
command   = 'init'
dismod_at.system_command_prc( [ program, file_name, command ] )
#
# -----------------------------------------------------------------------
print('old2new_command: OK')
sys.exit(0)
# END PYTHON
