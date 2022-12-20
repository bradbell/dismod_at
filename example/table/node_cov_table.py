# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin node_cov_table.py}
# {xrst_comment_ch #}
#
# node_cov_table: Example and Test
# ################################
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end node_cov_table.py}
# BEGIN PYTHON
def node_cov_table() :
   import dismod_at
   import copy
   #
   file_name      = 'example.db'
   new            = True
   connection     = dismod_at.create_connection(file_name, new)
   cursor         = connection.cursor()
   #
   # create the rate table
   col_name = [
      'covariate_id', 'node_id', 'age',  'time', 'cov_value'
   ]
   col_type = [
      'integer',     'integer',  'real', 'real', 'real'
   ]
   row_list = [
      [2,             3,          10.0,   1980.0, 0.1 ],
      [1,             2,          20.0,   1990.0, 0.2 ],
   ]
   tbl_name = 'node_cov'
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
   print('node_cov_table: OK')
# END PYTHON
