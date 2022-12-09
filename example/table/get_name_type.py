# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin get_name_type.py}
# {xrst_comment_ch #}
#
# get_name_type: Example and Test
# ###############################
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end get_name_type.py}
# BEGIN PYTHON
#
def get_name_type() :
   import dismod_at
   #
   file_name      = 'example.db'
   new            = True
   connection     = dismod_at.create_connection(file_name, new)
   cursor         = connection.cursor()
   #
   # create temp table
   col_name = [ 'int_name', 'real_name', 'text_name'  ]
   col_type = [ 'integer',  'real',       'text'      ]
   row_list = [
      [ 1,          2.0,          'three'     ]
   ]
   tbl_name = 'temp'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   #
   check_name = [ 'temp_id' ] + col_name
   check_type = [ 'integer primary key'] + col_type
   #
   # get the column names and corresponding types
   (col_name, col_type) = dismod_at.get_name_type(connection, tbl_name)
   #
   assert col_name == check_name
   assert col_type == check_type
   #
   connection.close()
   print('get_name_type: OK')
# END PYTHON
