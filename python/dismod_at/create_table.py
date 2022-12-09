# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin create_table}
# {xrst_spell
#     tbl
# }
# {xrst_comment_ch #}
#
# Create a Database Table
# #######################
#
# Syntax
# ******
#
# | ``dismod_at.create_table`` (
# | |tab| *connection* , *tbl_name* , *col_name* , *col_type* , *row_list*
# | )
#
# connection
# **********
# is a :ref:`create_connection@connection` for this database.
#
# tbl_name
# ********
# is a ``str`` that specifies the name of the table.
#
# col_name
# ********
# is a ``list`` of ``str``
# where the elements are the column names in the table that is created.
# The column name for the primary key, *tbl_name* _ ``id`` must
# not be included in the list.
#
# unique
# ======
# If the column name *tbl_name* _ ``name`` is in the list,
# the corresponding column will have the ``unique`` constraint.
#
# col_type
# ********
# is a ``list`` of ``str`` where the elements are the column types
# in the same order as *col_name* .
# The valid types are
# ``integer`` , ``real`` , ``text`` .
#
# row_list
# ********
# is a possibly empty ``list`` of rows contain data that is written
# to the table.
# Each row is itself a list containing the data for one row of the
# table in the same order as *col_name* .
# Note that the primary key column is not included in *row_list* .
# Also note that the value in each column gets converted to unicode
# before being written to the database.
# Note that the special value
#
#     *row_list* [ *i* ][ *j* ] == ``None``
#
# gets converted to ``null`` in the table.
#
# tbl_name_id
# ***********
# A column with name *tbl_name* _ ``id`` and type
# ``integer primary key`` is included as the first column in the table.
# Its values start with zero (for the first row) and
# increment by one for each row.
#
# Side Effects
# ************
# This routine does a
#
#     *connection* . ``commit`` ()
#
# to make sure the table exists before returning.
# {xrst_toc_hidden
#    example/table/create_table.py
# }
# Example
# *******
# The file :ref:`create_table.py-name` is an example use of
# ``create_table`` .
#
# {xrst_end create_table}
# ---------------------------------------------------------------------------
def create_table(connection, tbl_name, col_name, col_type, row_list) :
   import dismod_at
   import copy
   primary_key = tbl_name + '_id'
   name_column = tbl_name + '_name'
   #
   cmd       = 'create table ' + tbl_name + '('
   n_col     = len( col_name )
   cmd      += '\n\t' + tbl_name + '_id integer primary key'
   for j in range(n_col) :
      assert col_name != primary_key
      cmd   += ',\n\t' + col_name[j] + ' ' + col_type[j]
      if col_name[j] == name_column :
         cmd += ' unique'
   cmd += '\n\t);'
   #
   cursor  = connection.cursor()
   cursor.execute(cmd)
   #
   quote_text = True
   for i in range( len(row_list) ) :
      row_cpy     = copy.copy(row_list[i])
      row_cpy.insert(0, i)
      value_tuple = dismod_at.unicode_tuple(row_cpy, quote_text)
      cmd = 'insert into ' + tbl_name + ' values ' + value_tuple
      cursor.execute(cmd)
   connection.commit()
