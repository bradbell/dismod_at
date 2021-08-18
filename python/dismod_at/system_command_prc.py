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
# $icode%stdout% = dismod_at.system_command_prc(%command%)
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
# If it is non-zero, print the contents of stderr and then
# raise an assertion.
# $lnext
# Return the contents of standard out as a python string.
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
		command, stdout=subprocess.PIPE,
		stderr=subprocess.PIPE,
		encoding='utf-8'
	)
	# check the command return cide
	if result.returncode != 0 :
		# print error messages
		print('system_command_prc failed: returncode = ' , result.returncode)
		print( result.stderr )
		#
		# raise an exception
		assert result.returncode == 0
    #
	return result.stdout
