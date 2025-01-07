# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin create_connection}
# {xrst_spell
#     readonly
# }
# {xrst_comment_ch #}
#
# Create a Python sqlite3 Database Connection
# ###########################################
#
# Prototype
# *********
# {xrst_literal
#     BEGIN_PROTOTYPE
#     END_PROTOTYPE
# }
#
# file_name
# *********
# is the name of the file where the data base is stored.
#
# new
# ***
# If *new* is true, a new data base is created with the specified file name.
# If an old version of the file exists, its contents are lost.
# If *new* if false,
# the existing data base corresponding to the file name is used.
#
# readonly
# ********
# If *readonly* is true, the database is opened in read only mode.
#
# connection
# **********
# The return value is an sqlite3
# `connection object <https://docs.python.org/2/library/sqlite3.html#connection-objects>`_
#
# Close
# *****
# You must close this connection when you are done with it by executing
# *connection*\ .\ ``close``\ ()
#
# One Thread
# **********
# The connection can only be used the thread that created it.
#
# {xrst_end create_connection}
# ---------------------------------------------------------------------------
import sqlite3
# BEGIN_PROTOTYPE
# connection =
def create_connection(file_name, new = False, readonly = False) :
   assert type(file_name) == str
   assert type(new)       == bool
   assert type(readonly)  == bool
# END_PROTOTYPE
   import os
   import sys
   #
   if new :
      # remove old version of this file
      if  os.path.isfile(file_name) :
         os.remove(file_name)
   else :
      # make sure this file exists
      if not os.path.isfile(file_name) :
         msg = 'create_connection: ' + file_name + ' does not exist.'
         assert False, msg
   if readonly :
      file_name_plus_mode =  f'file:{file_name}?mode=ro'
      connection = sqlite3.connect(
         file_name_plus_mode, uri = True, check_same_thread = True
      )
   else :
      connection = sqlite3.connect(file_name, check_same_thread = True)
   return connection
