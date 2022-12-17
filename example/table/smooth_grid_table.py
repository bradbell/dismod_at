# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin smooth_grid_table.py$$ $newlinech #$$
#
# $nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section smooth_grid_table: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# BEGIN PYTHON
def smooth_grid_table() :
   import dismod_at
   import copy
   import collections
   #
   file_name      = 'example.db'
   new            = True
   connection     = dismod_at.create_connection(file_name, new)
   cursor         = connection.cursor()
   #
   # create smooth table
   ptype    = 'integer primary key'
   col_name2type = collections.OrderedDict( [
      ('smooth_name',             'text'    ),
      ('n_age',                   'integer' ),
      ('n_time',                  'integer' ),
      ('mulstd_value_prior_id',   'integer' ),
      ('mulstd_dage_prior_id',    'integer' ),
      ('mulstd_dtime_prior_id',   'integer' )
   ] )
   col_name = list(col_name2type.keys())
   col_type = list(col_name2type.values())
   row_list = [
              [ 'constant',    1,          1,        1, 1, 1],
              [ 'age_only',    3,          1,        1, 1, 1],
              [ 'time_only',   1,          2,        1, 1, 1],
              [ 'bilinear',    3,          2,        1, 1, 1]
   ]
   tbl_name = 'smooth'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   #
   # smooth_grid table column names
   col_name2type = collections.OrderedDict( [
      ('smooth_id',       'integer' ),
      ('age_id',          'integer' ),
      ('time_id',         'integer' ),
      ('value_prior_id',  'integer' ),
      ('dage_prior_id',   'integer' ),
      ('dtime_prior_id',  'integer' ),
      ('const_value',     'real'    )
   ] )
   col_name = list(col_name2type.keys())
   col_type = list(col_name2type.values())
   #
   # smooth_grid table values
   row_list = list()
   default  = [
      3,          # smooth_id          (smooth_id == 3 is bilinear)
      None,       # age_id             (age_id  index is 1 in default)
      None,       # time_id            (time_id index is 2 in default)
      1,          # value_prior_id
      2,          # dage_prior_id
      3,          # dtime_prior_id
      None        # const_value
   ]
   age_time_list = list()
   for age_id in [0, 1, 2] :            # n_age is 3
      for time_id in [0, 1] :          # n_time is 2
         default[1] = age_id
         default[2] = time_id
         row        = copy.copy(default)
         if age_id == 2 :
            row[4] = None # dage_prior_id null for this case
         if time_id == 1 :
            row[5] = None # dtime_prior_id null for this case
         row_list.append( row )
         age_time_list.append( (age_id, time_id) )
   #
   # write the table
   tbl_name = 'smooth_grid'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   #
   # check values in the table
   columns = ','.join(col_name)
   columns = 'smooth_grid_id,' + columns
   cmd     = 'SELECT ' + columns + ' FROM smooth_grid'
   cmd    += ' JOIN smooth USING(smooth_id) '
   cmd    += ' WHERE smooth_name = "bilinear"'
   count        = 0
   cursor       = connection.cursor()
   for row in cursor.execute(cmd) :
      assert len(row) == 8
      assert row[0] == count
      assert row[1] == 3
      assert row[2] == age_time_list[count][0]
      assert row[3] == age_time_list[count][1]
      assert row[4] == 1
      if row[2] == 2 :
         assert row[5] == None
      else :
         assert row[5] == 2
      if row[3] == 1 :
         assert row[6] == None
      else :
         assert row[6] == 3
      assert row[7] == None
      count += 1
   assert count == len(row_list)
   #
   connection.close()
   print('smooth_grid_table: OK')
# END PYTHON
