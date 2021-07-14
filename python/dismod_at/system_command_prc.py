# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin system_command_prc$$ $newlinech #$$
# $spell
#	dismod
#	prc
#	str
#	stdout
#	stderr
# $$
#
# $section Print Run and Check a System Command$$
#
# $head Syntax$$
# $icode%stdout%, %stderr% = dismod_at.system_command_prc(%command%)
# %$$
#
# $head Purpose$$
# This routine combines the following steps:
# $list number$$
# Print the system command as it would appear in the shell; i.e.,
# with arguments separated by spaces.
# $lnext
# Run the system command and wait for it to complete.
# $lnext
# Check the integer value returned by the system command.
# If it is non-zero, exit this python program with message saying
# that the command failed.
# $lend
#
# $head command$$
# is a $code list$$ with $code str$$ elements. The first element is the
# program to execute and the other elements are arguments to the program.
#
# $head stdout$$
# This is the standard output result for this command.
#
# $head Example$$
# Many of the $cref user_example$$ examples use this utility.
#
# $end
# ---------------------------------------------------------------------------
def system_command_prc(command) :
	import sys
	import subprocess
	#
	# print the system command with arguments separated by spaces
	print( ' '.join(command) )
	#
	# run the system command
	result = subprocess.run(
		command, stdout=subprocess.PIPE, stderr=subprocess.PIPE
	)
	# check the system return value
	if result.returncode != 0 :
		# print the commands error messages
		print( result.stderr )
		#
		# inform user that the command failed and then exit
		sys.exit('command failed: returncode = ' + str(returncode))
	return result.stdout
