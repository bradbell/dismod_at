# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin age_table.py$$ $newlinech #$$
#
# $section age_table: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# BEGIN PYTHON
def age_table() :
   import dismod_at
   import copy
   #
   file_name      = 'example.db'
   new            = True
   connection     = dismod_at.create_connection(file_name, new)
   cursor         = connection.cursor()
   #
   # create the age table
   col_name = [ 'age'  ]
   col_type = [ 'real' ]
   row_list = [ [0.0], [20.0], [40.0], [60.0], [80.0], [100.0] ]
   tbl_name = 'age'
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
   print('age_table: OK')
# END PYTHON
