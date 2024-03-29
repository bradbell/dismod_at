# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin data_table.py}
# {xrst_comment_ch #}
#
# data_table: Example and Test
# ############################
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end data_table.py}
# BEGIN PYTHON
def data_table() :
   import dismod_at
   import copy
   import collections
   #
   file_name      = 'example.db'
   connection     = dismod_at.create_connection(
      file_name, new = True, readonly = False
   )
   #
   col_name2type = collections.OrderedDict([
      # required columns
      ('data_name',    'text'   ),
      ('integrand_id', 'integer'),
      ('density_id',   'integer'),
      ('node_id',      'integer'),
      ('weight_id',    'integer'),
      ('hold_out',     'integer'),
      ('meas_value',   'real'   ),
      ('meas_std',     'real'   ),
      ('age_lower',    'real'   ),
      ('age_upper',    'real'   ),
      ('time_lower',   'real'   ),
      ('time_upper',   'real'   ),
      # covariates
      ('x_sex',        'real'   ),
      ('x_income',     'real'   ),
      # comments
      ('c_data_source','text'   )
   ] )
   col_name = list(col_name2type.keys())
   col_type = list(col_name2type.values())
   row_list = [ [
      'one',                  # data_name
      1,                      # integrand_id
      0,                      # density_id
      3,                      # node_id
      4,                      # weight_id
      0,                      # hold_out
      1e-4,                   # meas_value
      1e-5,                   # meas_std
      10.0,                   # age_lower
      90.0,                   # age_upper
      2000.,                  # time_lower
      2005.,                  # time_upper
      0.5,                    # x_sex
      1000.,                  # x_income
      'www.healthdata.org'    # c_data_source
   ] ]

   # create the data table
   tbl_name = 'data'
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
   print('data_table: OK')
# END PYTHON
