# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin get_table_dict.py}
# {xrst_comment_ch #}
#
# get_table_dict: Example and Test
# ################################
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end get_table_dict.py}
# BEGIN PYTHON
def get_table_dict() :
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
   table_dict = dismod_at.get_table_dict(connection, tbl_name)
   assert len(table_dict) == n_row
   for i in range(n_row) :
      assert len( table_dict[i] )            == 2
      assert table_dict[i]['covariate_name'] == row_list[i][0]
      assert table_dict[i]['reference']      == row_list[i][1]
   #
   connection.close()
   print('get_table_dict: OK')
# END PYTHON
