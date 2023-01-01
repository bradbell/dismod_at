# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin nslist_pair_table.py$$ $newlinech #$$
# $spell
#  nslist
# $$
#
# $nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section nslist_pair_table: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# BEGIN PYTHON
def nslist_pair_table() :
   import dismod_at
   import copy
   import collections
   #
   file_name      = 'example.db'
   new            = True
   connection     = dismod_at.create_connection(file_name, new)
   cursor         = connection.cursor()
   #
   # create nslist_pair table column names and types
   col_name2type = collections.OrderedDict( [
      ('nslist_id',       'integer' ),
      ('node_id',                 'integer' ),
      ('smooth_id',                'integer' )
   ] )
   col_name = list(col_name2type.keys())
   col_type = list(col_name2type.values())
   # two lists with different smoothing for each node
   row_list = [
      # nslist_id, node_id, smooth_id
      [                 0,       0,         0 ],
      [                 0,       1,         1 ],
      [                 0,       2,         2 ],
      [                 1,       0,         3 ],
      [                 1,       1,         4 ],
      [                 1,       2,         5 ]
   ]
   tbl_name = 'nslist_pair'
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
   print('nslist_pair_table: OK')
# END PYTHON
