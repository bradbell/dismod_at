# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin connection_file}
# {xrst_comment_ch #}
#
# Get File Name For a Database Connection
# #######################################
#
# Syntax
# ******
#
#     *file_name* = ``dismod_at.connection_file_name`` ( *connection* )
#
# Purpose
# *******
# This function can be used for reporting errors that occur
# when using a database connection.
#
# connection
# **********
# The is a database connection returned by :ref:`create_connection-name` .
#
# file_name
# *********
# The return value is a ``str`` containing the name of the
# file where the data base is stored.
# {xrst_toc_hidden
#    example/user/connection_file.py
# }
# Example
# *******
# The file :ref:`user_connection_file.py-name` contains an example and test of
# this routine.
#
# {xrst_end connection_file}
# ---------------------------------------------------------------------------
import sqlite3
def connection_file(connection) :
   cursor  = connection.cursor()
   command = 'PRAGMA database_list'
   cursor.execute(command)
   rows      = cursor.fetchall()
   file_name = None
   for row in rows :
      if row[1] == 'main' :
         file_name = row[2]
   return file_name
