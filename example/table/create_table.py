# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin create_table.py}
# {xrst_comment_ch #}
#
# create_table and Unicode: Example and Test
# ##########################################
#
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end create_table.py}
# BEGIN PYTHON
def create_table() :
   import dismod_at
   #
   file_name      = 'example.db'
   new            = True
   connection     = dismod_at.create_connection(file_name, new)
   cursor         = connection.cursor()
   #
   inverted_exclamation = chr( 10 * 16 + 1 ) # 00a1
   cent_sign            = chr( 10 * 16 + 2 ) # 00a2
   pound_sign           = chr( 10 * 16 + 3 ) # 00a3
   #
   # create table
   col_name = [ 'temp_name'          ]
   col_type = [ 'text'               ]
   row_list = [
              [ inverted_exclamation ],
              [ cent_sign            ],
              [ pound_sign           ]
   ]
   tbl_name = 'temp'
   dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
   #
   # check values in table
   row_list = list()
   cmd      = 'SELECT temp_id, temp_name FROM temp'
   for row in cursor.execute(cmd) :
      row_list.append(row)
   #
   for i in range( len(row_list) ) :
      assert row_list[i][0] == i
   #
   assert row_list[0][1] == inverted_exclamation
   assert row_list[1][1] == cent_sign
   assert row_list[2][1] == pound_sign
   #
   connection.close()
   print('create_table: OK')
# END PYTHON
