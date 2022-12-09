# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin system_command_prc}
# {xrst_spell
#     returncode
#     stderr
#     stdout
# }
# {xrst_comment_ch #}
#
# Print Run and Check a System Command
# ####################################
#
# Syntax
# ******
# {xrst_literal
#     BEGIN syntax
#     END syntax
# }
#
# Purpose
# *******
# This routine provides variations on the following steps:
#
# #. Print the system command as it would appear in the shell; i.e.,
#    with arguments separated by spaces.
# #. Run the system command and wait for it to complete.
# #. Check the integer value returned by the system command.
#    If it is non-zero, an assert exception is raised with
#    stderr as the message in the exception.
# #. Return the contents of standard out as a python string.
#
# command
# *******
# is a ``list`` with ``str`` elements. The first element is the
# program to execute and the other elements are arguments to the program.
#
# print_command
# *************
# If this argument is true (false) the command will (will not) be printed
# before it is executed.
#
# return_stdout
# *************
# If this argument is true, the command's standard output will be returned.
# If this argument is false and *file_stdout* is not None,
# standard error will be written to a file during the command execution.
# Otherwise, standard output will be printed during the command execution.
#
# return_stderr
# *************
# If this argument is true, the command's standard error will be returned.
# If this argument is false and *file_stderr* is not None,
# standard error will be written to a file during the command execution.
# Otherwise, if an error occurs, an assertion is generated with
# the commands standard error as the corresponding message.
#
# file_stdout
# ***********
# If *return_stdout* is true, this argument must be None.
# If this argument is not None, it is a file object that is opened for writing
# and standard output will be written to this file.
#
# file_stderr
# ***********
# If *return_stderr* is true, this argument must be None.
# If this argument is not None, it is a file object that is opened for writing
# and standard error will be written to this file.
#
# write_command
# *************
# If *write_command* is true (false) the command will
# (will not) be written to *file_stdout* .
# If *file_stdout* is None, *write_command* must be false.
#
# result
# ******
#
# #. If *return_stdout* and *return stderr* are both false,
#    *result* is ``None`` .
# #. If *return_stdout* is true and *return_stderr* is false,
#    *result* is a ``str`` with the contents of standard output.
# #. If *return_stderr* is true and *return_stdout* is false,
#    *result.stderr*
#    is an ``str`` with the contents of standard error,
#    and *result.returncode*
#    is an ``int`` with the command's return code..
# #. If both *return_stderr* and *return_stdout* are true,
#    *result.stderr*
#    is an ``str`` with the contents of standard error,
#    *result.stdout*
#    is an ``str`` with the contents of standard output,
#    and *result.returncode*
#    is an ``int`` with the command's return code..
#
# Example
# *******
# Many of the :ref:`user_example-name` examples use this utility.
#
# {xrst_end system_command_prc}
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
