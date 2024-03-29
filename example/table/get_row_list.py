# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin get_row_list.py}
# {xrst_comment_ch #}
#
# get_row_list: Example and Test
# ##############################
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end get_row_list.py}
# BEGIN PYTHON
def get_row_list() :
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
   col_name = [ 'covariate_name',  'reference' ]
   col_type = [ 'text',             'real'     ]
   row_list = [
              [ 'sex',              0.0        ],
              [ 'income',           2000.0     ]
   ]
   tbl_name = 'covariate'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   n_row = len(row_list)
   #
   # reverse the order of the columns
   col_name = [ 'reference',   'covariate_name' ]
   n_col    = len(col_name)
   row_list = dismod_at.get_row_list(connection, tbl_name, col_name)
   #
   assert len(row_list) == n_row
   for i in range(n_row) :
      assert len(row_list[i] ) == n_col
      assert isinstance( row_list[i][0], float)
      assert isinstance( row_list[i][1], str)
   assert row_list[0][0] == 0.0
   assert row_list[0][1] == 'sex'
   assert row_list[1][0] == 2000.0
   assert row_list[1][1] == 'income'
   #
   connection.close()
   print('get_row_list: OK')
# END PYTHON
