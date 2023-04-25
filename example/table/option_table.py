# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin option_table.py}
# {xrst_comment_ch #}
#
# option_table: Example and Test
# ##############################
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end option_table.py}
# BEGIN PYTHON
def option_table() :
   import dismod_at
   import copy
   #
   file_name      = 'example.db'
   connection     = dismod_at.create_connection(
      file_name, new = True, readonly = False
   )
   cursor         = connection.cursor()
   #
   # create the option table
   col_name = [ 'option_name', 'option_value'  ]
   col_type = [ 'text unique'  , 'text' ]
   row_list = [
      ['parent_node_id',        '0'                ] ,
      ['ode_step_size',         '20.0'             ] ,
      ['rate_case',             'iota_pos_rho_pos' ] ,
      ['tolerance_fixed',       '1e-8'             ] ,
      ['max_num_iter_fixed',    '100'              ] ,
      ['print_level_fixed',     '0'                ] ,
      ['derivative_test_fixed', 'second-order'     ] ,
   ]
   tbl_name = 'option'
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
   print('option_table: OK')
# END PYTHON
