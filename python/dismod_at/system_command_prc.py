# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-19 University of Washington
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
# $$
#
# $section Print Run and Check a System Command$$
#
# $head Syntax$$
# $codei%dismod_at.system_command_prc(%command%)
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
# $head Standard Output and Error$$
# These output streams are not displayed or returned.
# If an error occurs, re-run the command as printed to get this information.
#
# $head Example$$
# Many of the $cref user$$ examples use this utility.
#
# $end
# ---------------------------------------------------------------------------
def system_command_prc(command) :
	import subprocess
	#
	# print the system command with arguments separated by spaces
	print( ' '.join(command) )
	# run the system command
	flag = subprocess.call( command )
	# check the system return value
	if flag != 0 :
		# inform user that the command failed and then exit
		sys.exit('command failed: flag = ' + str(flag))
	return
