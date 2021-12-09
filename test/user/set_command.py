#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
import sys
import os
import copy
import subprocess
# ---------------------------------------------------------------------------
# check execution is from distribution directory
script = 'test/user/set_command.py'
if sys.argv[0] != script  or len(sys.argv) != 1 :
	usage  = 'python3 ' + script + '\n'
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
# change into the build/test/user directory
if not os.path.exists('build/test/user') :
    os.makedirs('build/test/user')
os.chdir('build/test/user')
# ---------------------------------------------------------------------------
# create get_started.db
get_started_db.get_started_db()
# -----------------------------------------------------------------------
program    = '../../devel/dismod_at'
file_name  = 'get_started.db'
cmd        = [ program, file_name, 'set', 'avgint', 'data' ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at command failed.')
# -----------------------------------------------------------------------
# connect to database
new          = False
connection   = dismod_at.create_connection(file_name, new)
avgint_table = dismod_at.get_table_dict(connection, 'avgint')
data_table   = dismod_at.get_table_dict(connection, 'data')
assert avgint_table == data_table
#
connection.close()
# -----------------------------------------------------------------------
print('set_command.py: OK')
# END PYTHON
