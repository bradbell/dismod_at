# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin covariate_table.py}
# {xrst_comment_ch #}
#
# covariate_table: Example and Test
# #################################
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end covariate_table.py}
# BEGIN PYTHON
def covariate_table() :
   import dismod_at
   import copy
   #
   file_name      = 'example.db'
   connection     = dismod_at.create_connection(
      file_name, new = True, readonly = False
   )
   cursor         = connection.cursor()
   #
   # create the covariate table
   col_name = [ 'covariate_name',  'reference', 'max_difference' ]
   col_type = [ 'text',             'real',     'real'           ]
   row_list = [
              [ 'sex',              0.0,        0.6              ],
              [ 'income',           2000.0,     None             ]
   ]
   tbl_name = 'covariate'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   #
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
   print('covariate_table: OK')
# END PYTHON
