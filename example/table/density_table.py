# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin density_table.py}
# {xrst_comment_ch #}
#
# density_table: Example and Test
# ###############################
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end density_table.py}
# BEGIN PYTHON
def density_table() :
   import dismod_at
   import copy
   #
   file_name      = 'example.db'
   connection     = dismod_at.create_connection(
      file_name, new = True, readonly = False
   )
   cursor         = connection.cursor()
   #
   # create the density table
   col_name = [ 'density_name'  ]
   col_type = [ 'text'       ]
   row_list = [
      ['uniform'],
      ['gaussian'],
      ['laplace'],
      ['log_gaussian'],
      ['log_laplace']
   ]
   tbl_name = 'density'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   # ----------------------------------------------------------------------
   # include primary key in test
   check_name = [ tbl_name + '_id' ] + col_name
   check_list = list()
   for i in range( len(row_list) ) :
      check_list.append( [i] + row_list[i] )
   #
   row_list = dismod_at.get_row_list(connection, tbl_name, check_name)
   assert row_list == check_list
   # ----------------------------------------------------------------------
   connection.close()
   print('density_table: OK')
# END PYTHON
