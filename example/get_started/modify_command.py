# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin modify_command.py$$ $newlinech #$$
# $spell
#	init
#	dismod
# $$
#
# $section modify Command: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
import csv
import math
# ---------------------------------------------------------------------------
# check execution is from distribution directory
example = 'example/get_started/modify_command.py'
if sys.argv[0] != example  or len(sys.argv) != 1 :
	usage  = 'python3 ' + example + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
#
# distribution directory
dist_dir  = os.getcwd()
#
# directory where test files are stored
test_dir  = dist_dir + '/build/example/get_started'
#
# import sandbox version of dismod_at
local_dir = dist_dir + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
	sys.path.insert(0, local_dir)
import dismod_at
#
# import get_started_db example
sys.path.append( os.getcwd() + '/example/get_started' )
import get_started_db
#
# change into the build/example/get_started directory
if not os.path.exists(test_dir) :
    os.makedirs(test_dir)
os.chdir(test_dir)
# ---------------------------------------------------------------------------
# create get_started.db
get_started_db.get_started_db()
# -----------------------------------------------------------------------
# change into distribution directory to run sandbox version of dismodat.py
# return to test_dir when done.
os.chdir(dist_dir)
print( os.getcwd() )
program          = 'bin/dismodat.py'
file_name        = test_dir + '/get_started.db'
command          = 'modify'
table_name       = 'data'       # table we are modifiying
column_name      = 'density_id' # column we are modifying
row_expression   = 'data_id==0' # row we are modifiying
value_expression = '6'          # value we are placeing in that row, column
system_cmd       = [
	program,
	file_name,
	command,
	table_name,
	column_name,
	row_expression,
	value_expression
]
dismod_at.system_command_prc( system_cmd )
# -----------------------------------------------------------------------
# get data table
new        = False
connection = dismod_at.create_connection(file_name, new)
data_table = dismod_at.get_table_dict( connection, 'data')
connection.close()
data_id     = 0
column_name = 'density_id'
assert data_table[data_id][column_name] == 6
# -----------------------------------------------------------------------
print('modify_command.py: OK')
# END PYTHON
