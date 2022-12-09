# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin sql_command}
# {xrst_spell
#     sql
# }
# {xrst_comment_ch #}
#
# Execute an SQL command
# ######################
#
# Syntax
# ******
#
#     *result* = ``dismod_at.sql_command`` ( *connection* , *command* )
#
# connection
# **********
# is a :ref:`create_connection@connection` for an
# sql_lite database.
#
# command
# *******
# is a ``str`` containing the command that is executed.
#
# result
# ******
# Is a list corresponding to the rows returned by the command.
#
# {xrst_end sql_command}
# ---------------------------------------------------------------------------
def sql_command(connection, command) :
   cursor = connection.cursor()
   cursor.execute(command)
   connection.commit()
   return cursor.fetchall()
