# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
"""
{xrst_begin user_connection_file.py}

Example Using connection_file
#############################

Source Code
***********
{xrst_literal
   # BEGIN PYTHON
   # END PYTHON
}

{xrst_end user_connection_file.py}
$end
"""
# BEGIN PYTHON
import sys
import os
#
test_program = 'example/user/connection_file.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
   usage  = 'python3 ' + test_program + '\n'
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
# change into the build/example/user directory
if not os.path.exists('build/example/user') :
   os.makedirs('build/example/user')
os.chdir('build/example/user')
#
# connection
new        = True
database   = 'example.db'
connection = dismod_at.create_connection(database , new)
#
# file_name
file_name  = dismod_at.connection_file(connection)
#
# check
assert os.path.samefile(file_name,  database)
#
print('connection_file.py: OK')
sys.exit(0)
# END PYTHON
