# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin system_command_prc$$ $newlinech #$$
# $spell
#  dismod
#  prc
#  str
#  stdout
#  stderr
#  returncode
# $$
#
# $section Print Run and Check a System Command$$
#
# $head Syntax$$
# $srcthisfile%0%# BEGIN syntax%# END syntax%1%$$
#
# $head Purpose$$
# This routine provides variations on the following steps:
# $list number$$
# Print the system command as it would appear in the shell; i.e.,
# with arguments separated by spaces.
# $lnext
# Run the system command and wait for it to complete.
# $lnext
# Check the integer value returned by the system command.
# If it is non-zero, an assert exception is raised with
# stderr as the message in the exception.
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
# If this argument is false and $icode file_stdout$$ is not None,
# standard error will be written to a file during the command execution.
# Otherwise, standard output will be printed during the command execution.
#
# $head return_stderr$$
# If this argument is true, the command's standard error will be returned.
# If this argument is false and $icode file_stderr$$ is not None,
# standard error will be written to a file during the command execution.
# Otherwise, if an error occurs, an assertion is generated with
# the commands standard error as the corresponding message.
#
# $head file_stdout$$
# If $icode return_stdout$$ is true, this argument must be None.
# If this argument is not None, it is a file object that is opened for writing
# and standard output will be written to this file.
#
# $head file_stderr$$
# If $icode return_stderr$$ is true, this argument must be None.
# If this argument is not None, it is a file object that is opened for writing
# and standard error will be written to this file.
#
# $head write_command$$
# If $icode write_command$$ is true (false) the command will
# (will not) be written to $icode file_stdout$$.
# If $icode file_stdout$$ is None, $icode write_command$$ must be false.
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
   return_stdout  = False ,
   return_stderr  = False ,
   file_stdout    = None  ,
   file_stderr    = None  ,
   write_command  = False ,
# )
# END syntax
   ) :
   import sys
   import subprocess
   #
   if file_stdout is not None :
      assert not return_stdout
   if file_stderr is not None :
      assert not return_stderr
   if file_stdout is None :
      assert not write_command
   #
   # capture_stderr
   capture_stderr = return_stderr or (file_stderr is not None)
   #
   # command_str
   command_str = ''
   for arg in command :
      if ' ' in arg :
         command_str += f" '{arg}'"
      else :
         command_str += f' {arg}'
   command_str = command_str[1:]
   #
   # print
   if print_command :
      print(command_str)
   #
   # write
   if write_command :
      file_stdout.write(command_str + '\n')
      file_stdout.flush()
   #
   # stdout
   if return_stdout :
      stdout = subprocess.PIPE
   else :
      stdout = file_stdout
   #
   # stderr
   if return_stderr or file_stderr is None :
      stderr = subprocess.PIPE
   else :
      stderr = file_stderr
   #
   # subprocess_return
   subprocess_return = subprocess.run(
      command,
      stdout   = stdout  ,
      stderr   = stderr  ,
      encoding = 'utf-8' ,
   )
   #
   # result
   if return_stderr :
      result = subprocess_return
   elif return_stdout :
      result = subprocess_return.stdout
   else :
      result = None
   #
   if capture_stderr :
      return result
   #
   # return_stderr is false so check the command return code
   returncode = subprocess_return.returncode
   if returncode != 0 :
      # print error messages
      msg  = f'system_command_prc failed: returncode = {returncode}\n'
      msg += subprocess_return.stderr
      #
      # raise an exception
      assert False, msg
   #
   return result
