# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
"""
$begin user_system_command_prc.py$$
$spell
	prc
$$

$section Example using system_command_prc$$

$head Discussion$$
This example demonstrates some of the different argument cases for the
$cref system_command_prc$$ function.

$head Source Code$$
$srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
$end
$end
"""
# BEGIN PYTHON
import sys
import os
import time
import distutils.dir_util
#
test_program = 'example/user/system_command_prc.py'
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
distutils.dir_util.mkpath('build/example/user')
os.chdir('build/example/user')
#
# command
command = [ 'echo', 'system_command_prc: Test Output' ]
#
# return_stdout=True, return_stderr=False
result  = dismod_at.system_command_prc(
	command, print_command=False, return_stdout=True, return_stderr=False
)
assert result  == command[1] + '\n'
#
# return_stdout=False, return_stderr=False
result  = dismod_at.system_command_prc(
	command, print_command=False, return_stdout=False, return_stderr=False
)
assert result is None
#
# return_stdout=True, return_stderr=True
result  = dismod_at.system_command_prc(
	command, print_command=False, return_stdout=True, return_stderr=True
)
assert result.stdout  == command[1] + '\n'

#
print('system_command_prc.py: OK')
sys.exit(0)
# END PYTHON
