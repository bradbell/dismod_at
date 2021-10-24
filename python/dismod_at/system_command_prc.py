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
#	returncode
# $$
#
# $section Print Run and Check a System Command$$
#
# $head Syntax$$
# $srcthisfile%0%# BEGIN syntax%# END syntax%1%$$
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
# $head print_command$$
# If this argument is true (false) the command will (will not) be printed
# before it is executed.
#
# $head return_stdout$$
# If this argument is true, the command's standard output will be returned.
# Otherwise, standard output will be printed during the command execution.
#
# $head return_stderr$$
# If this argument is true, the command's standard error will be returned.
# Otherwise, if an error occurs, standard error will be printed
# and $code system_command_prc$$ will terminate execution.
#
# $head result$$
# $list number$$
# If $icode return_stdout$$ and $icode return stderr$$ are both false,
# $icode result$$ is $code None$$.
# $lnext
# If $icode return_stdout$$ is true and $icode return_stderr$$ is false,
# $icode result$$ is a $code str$$ with the contents of standard output.
# $lnext
# If $icode return_stderr$$ is true and $icode return_stdout$$ is false,
# $icode result.stderr$$
# is an $code str$$ with the contents of standard error,
# and $icode result.returncode$$
# is an $code int$$ with the command's return code..
# $lnext
# If both $icode return_stderr$$ and $icode return_stdout$$ are true,
# $icode result.stderr$$
# is an $code str$$ with the contents of standard error,
# $icode result.stdout$$
# is an $code str$$ with the contents of standard output,
# and $icode result.returncode$$
# is an $code int$$ with the command's return code..
# $lend
#
#
# $head Example$$
# Many of the $cref user_example$$ examples use this utility.
#
# $end
# ---------------------------------------------------------------------------
def system_command_prc(
# BEGIN syntax
# result = system_command_prc(
	command                ,
	print_command  = True  ,
	return_stdout  = True  ,
	return_stderr  = False ,
# )
# END syntax
	) :
	import sys
	import subprocess
	#
	# print the system command with arguments separated by spaces
	if print_command :
		print( ' '.join(command) )
	#
	# run the system command
	if return_stderr and return_stdout :
		result = subprocess.run(
			command,
			stdout=subprocess.PIPE,
			stderr=subprocess.PIPE,
			encoding='utf-8'
		)
		return result
	#
	if return_stderr :
		result = subprocess.run(
			command,
			stderr=subprocess.PIPE,
			encoding='utf-8'
		)
		return result
	#
	if return_stdout :
		result = subprocess.run(
			command,
			stdout=subprocess.PIPE,
			stderr=subprocess.PIPE,
			encoding='utf-8'
		)
	else :
		result = subprocess.run(
			command,
			stderr=subprocess.PIPE,
			encoding='utf-8'
		)
	#
	# return_stderr is false so check the command return code
	if result.returncode != 0 :
		# print error messages
		print('system_command_prc failed: returncode = ' , result.returncode)
		print( result.stderr )
		#
		# raise an exception
		assert result.returncode == 0
	#
	if return_stdout :
		return result.stdout
	return None
