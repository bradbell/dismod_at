# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin weight_grid_table.py}
# {xrst_comment_ch #}
#
# weight_grid_table: Example and Test
# ###################################
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end weight_grid_table.py}
# BEGIN PYTHON
def weight_grid_table() :
   import dismod_at
   import copy
   #
   file_name      = 'example.db'
   new            = True
   connection     = dismod_at.create_connection(file_name, new)
   cursor         = connection.cursor()
   #
   # create weight table
   col_name = [ 'weight_name', 'n_age',   'ntime'   ]
   col_type = [ 'text',        'integer', 'integer' ]
   row_list = [
              [ 'constant',    1,          1        ],
              [ 'age_linear',  2,          1        ],
              [ 'bilinear',    2,          2        ]
   ]
   tbl_name = 'weight'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   #
   # create weight_grid table
   col_name = [ 'weight_id', 'age_id',  'time_id',  'weight' ]
   col_type = [ 'integer',   'integer', 'integer',  'real'   ]
   row_list = [
              # constant
              [ 0,             1,        1,   1.0      ],
              # age_linear
              [ 1,             0,        1,   0.5      ],
              [ 1,             2,        1,   1.5      ],
              # bilinear
              [ 2,             0,        0,   0.5      ],
              [ 2,             2,        0,   1.0      ],
              [ 2,             0,        2,   1.0      ],
              [ 2,             2,        2,   1.5      ]
   ]
   tbl_name = 'weight_grid'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   #
   # check values in the bilinear weight table
   columns = ','.join(col_name)
   columns = 'weight_grid_id,' + columns
   cmd     = 'SELECT ' + columns + ' FROM weight_grid'
   cmd    += ' INNER JOIN weight USING (weight_id)'
   cmd    += ' WHERE weight_name = "bilinear"'
   #
   count        = 3
   cursor       = connection.cursor()
   for row in cursor.execute(cmd) :
      check = copy.copy( row_list[count] )
      check.insert(0, count)
      assert len(row) == len(check)
      for j in range( len(row) ) :
         assert row[j] == check[j]
      count += 1
   assert count == len( row_list )
   #
   connection.close()
   print('weight_grid_table: OK')
# END PYTHON
