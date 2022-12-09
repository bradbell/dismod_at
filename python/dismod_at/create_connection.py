# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin create_connection}
# {xrst_comment_ch #}
#
# Create a Python sqlite3 Database Connection
# ###########################################
#
# Syntax
# ******
#
#     *connection* = ``dismod_at.create_connection`` ( *file_name* , *new* )
#
# file_name
# *********
# is as ``str`` containing the name of the file where the data base
# is stored. If the file does not exist, an error message is printed
# and the program is terminated with a non-zero error status.
#
# new
# ***
# is either ``True`` or ``False`` .
# If it is true, and a data base with the same name already exists,
# the existing database is deleted before creating the connection.
#
# connection
# **********
# The return value is an sqlite3
# `connection object <https://docs.python.org/2/library/sqlite3.html#connection-objects>`_
# You must close this connection when you are done with it by executing
#
#     *connection* . ``close`` ()
#
# {xrst_end create_connection}
# ---------------------------------------------------------------------------
import sqlite3
def create_connection(file_name, new) :
   import os
   import sys
   if new :
      if  os.path.isfile(file_name) :
         os.remove(file_name)
   else :
      if not os.path.isfile(file_name) :
         msg = 'create_connection: ' + file_name + ' does not exist.'
         assert False, msg
   connection = sqlite3.connect(file_name)
   return connection
