# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin prior_table.py}
# {xrst_comment_ch #}
#
# prior_table: Example and Test
# #############################
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end prior_table.py}
# BEGIN PYTHON
def prior_table() :
   import dismod_at
   import copy
   import collections
   #
   file_name      = 'example.db'
   new            = True
   connection     = dismod_at.create_connection(file_name, new)
   cursor         = connection.cursor()
   #
   # create the prior table
   ptype    = 'integer primary key'
   col_name2type = collections.OrderedDict( [
      ('prior_name',  'text'    ),
      ('density_id',  'integer' ),
      ('lower',       'real'    ),
      ('upper',       'real'    ),
      ('mean',        'real'    ),
      ('std',         'real'    ),
      ('eta',         'real'    )
   ] )
   col_name = list(col_name2type.keys())
   col_type = list(col_name2type.values())
   uniform_density_id = 0;
   row_list = [ [
      'none',              # prior_name
      uniform_density_id,  # density_id
      None,                # lower
      None,                # upper
      0,                   # mean
      None,                # std
      None                 # eta
   ],[
      'rate',              # prior_name
      uniform_density_id,  # density_id
      0.0,                 # lower
      1.0,                 # upper
      0.1,                 # mean
      None,                # std
      None                 # eta
   ] ]
   tbl_name = 'prior'
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
   print('prior_table: OK')
# END PYTHON
