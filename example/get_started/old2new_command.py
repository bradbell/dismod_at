# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin old2new_command.py}
# {xrst_comment_ch #}
#
# old2new Command: Example and Test
# #################################
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end old2new_command.py}
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
# connection
file_name      = 'get_started.db'
connection     = dismod_at.create_connection(
   file_name, new = False, readonly = False
)
#
# data table
sql_cmd = 'ALTER TABLE data DROP COLUMN sample_size;'
dismod_at.sql_command(connection, sql_cmd)
#
# connection
connection.close()
#
# old2new command
program   = '../../devel/dismod_at'
file_name = 'get_started.db'
command   = 'old2new'
dismod_at.system_command_prc( [ program, file_name, command ] )
#
# data_table
file_name      = 'get_started.db'
connection     = dismod_at.create_connection(
   file_name, new = False, readonly = False
)
data_table = dismod_at.get_table_dict(connection, 'data')
connection.close()
#
# check sample_size column
for row in data_table :
   assert row['sample_size'] == None
#
# -----------------------------------------------------------------------
print('old2new_command: OK')
sys.exit(0)
# END PYTHON
