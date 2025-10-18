# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2025 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin python_log_command}
# {xrst_comment_ch #}
#
# Log A Python Command
# ####################
#
# Prototype
# *********
# {xrst_literal
#     # BEGIN_PROTOTYPE
#     # END_PROTOTYPE
# }
#
# begin_end
# *********
# is this the beginning or end of a command.
#
# database
# is the database that this command acts on.
#
# command_name
# ************
# is the name of this command; e.g., db2csv.
#
# arg_list
# ********
# Is the argument list for this command starting with
# arg_list[0] which is the dismod_at database for this command.
#
# {xrst_end python_log_command}
# ---------------------------------------------------------------------------
import dismod_at
import time
#
# BEGIN_PROTOTYPE
def log_command( begin_end, database, command_name, arg_list ):
   assert begin_end == "begin" or begin_end == "end"
   assert type(database) == str
   assert type(command_name) == str
   assert type(arg_list) == list
   for arg in arg_list :
      assert type(arg) == str
# END_PROTOTYPE
   #
   #
   # unix_time
   unix_time = int( time.time() )
   #
   # connection
   connection    = dismod_at.create_connection(
      database, new = False, readonly = False
   )
   #
   # database: log table
   # create log table if it does not yet exist
   cmd  = 'create table if not exists log('
   cmd += ' log_id        integer primary key,'
   cmd += ' message_type  text               ,'
   cmd += ' table_name    text               ,'
   cmd += ' row_id        integer            ,'
   cmd += ' unix_time     integer            ,'
   cmd += ' message       text               )'
   dismod_at.sql_command(connection, cmd)
   #
   # log_id
   log_table     = dismod_at.get_table_dict(connection, 'log')
   log_id        = len(log_table)
   #
   # message
   message       = f'{begin_end} {database} {command_name}'
   for i in range( len(arg_list) ) :
      message += ' ' + arg_list[i]
   #
   # database: log_table
   cmd  = 'insert into log'
   cmd += ' (log_id,message_type,table_name,row_id,unix_time,message) values('
   cmd += str(log_id) + ','     # log_id
   cmd += '"command",'          # message_type
   cmd += 'null,'               # table_name
   cmd += 'null,'               # row_id
   cmd += str(unix_time) + ','  # unix_time
   cmd += '"' + message + '")'  # message
   dismod_at.sql_command(connection, cmd)
   #
   # connection
   connection.close()
