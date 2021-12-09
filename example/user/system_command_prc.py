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

$section Example Using system_command_prc$$

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
if not os.path.exists('build/example/user') :
    os.makedirs('build/example/user')
os.chdir('build/example/user')
#
# command
command = [ 'echo', 'system_command_prc: Test Output' ]
#
# result is stdout
result  = dismod_at.system_command_prc(
	command                ,
	print_command = False  ,
	return_stdout = True   ,
	return_stderr = False  ,
	file_stdout   = None   ,
	file_stderr   = None   ,
	write_command = False  ,
)
assert result  == command[1] + '\n'
#
# result is None
result  = dismod_at.system_command_prc(
	command                ,
	print_command = False  ,
	return_stdout = False  ,
	return_stderr = False  ,
	file_stdout   = None   ,
	file_stderr   = None   ,
	write_command = False  ,
)
assert result is None
#
# result is stdout and stderr
result  = dismod_at.system_command_prc(
	command                ,
	print_command = False  ,
	return_stdout = True   ,
	return_stderr = True   ,
	file_stdout   = None   ,
	file_stderr   = None   ,
	write_command = False  ,
)
assert result.stdout  == command[1] + '\n'
assert result.stderr  == ''
#
# file contains stdout
file_name   = 'system_command_prc.stdout'
file_stdout = open(file_name, 'w')
dismod_at.system_command_prc(
	command                       ,
	print_command = False         ,
	return_stdout = False         ,
	return_stderr = False         ,
	file_stdout   = file_stdout   ,
	file_stderr   = None          ,
	write_command = False         ,
)
file_stdout.close()
file_stdout = open(file_name, 'r')
result      = file_stdout.read()
assert result  == command[1] + '\n'
os.remove(file_name)
#
print('system_command_prc.py: OK')
sys.exit(0)
# END PYTHON
