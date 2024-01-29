# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin get_row_list}
# {xrst_spell
#     col
#     sql
#     tbl
# }
# {xrst_comment_ch #}
#
# Get Data From a Table
# #####################
#
# Syntax
# ******
# *row_list* = ``dismod_at.get_row_list`` (
# *connection* , *tbl_name* , *col_name* )
#
# connection
# **********
# is a :ref:`create_connection@connection` for this database.
#
# table_name
# **********
# is a ``str`` that specifies the name of the table.
# We use the notation *n_row* for the number of rows in
# the table.
#
# col_name
# ********
# is a list of strings containing the names of the columns that we are
# retrieving data from.
# We use the notation *n_col* for the number of columns in
# *col_name* .
# You can determine the name of all the columns in the table using
# :ref:`get_name_type-name` .
#
# row_list
# ********
# This is a list, with length *n_row* ,
# where each element of the list is a list.
# In addition, for each *i* , *row_list* [ *i* ] is a list
# with length *n_col* .
# The value *row_list* [ *i* ][ *j* ] corresponds to
# primary key *tbl_name* _ ``id`` = *i* ,
# and column *col_name* [ *j* ] .
# The python type corresponding to the values in the table are
# as follows:
#
# .. csv-table::
#     :widths: auto
#
#     Sql Table,Python
#     ``integer``,``int``
#     ``real``,``float``
#     ``text``,``str``
#     ``null``,``None``
#
# You can determine the type for all the columns in the table using
# :ref:`get_name_type-name` .
# Note that the type ``integer primary key`` corresponds to
# ``integer`` above.
# {xrst_toc_hidden
#    example/table/get_row_list.py
# }
# Example
# *******
# The file :ref:`get_row_list.py-name` is an example use of
# ``get_row_list`` .
#
# {xrst_end get_row_list}
# ---------------------------------------------------------------------------
def get_row_list(connection, tbl_name, col_name) :
   import collections
   #
   cursor      = connection.cursor()
   n_col       = len(col_name)
   columns     = ','.join(col_name)
   primary_key = tbl_name + '_id'
   cmd         = 'SELECT ' + columns + ' FROM ' + tbl_name
   cmd        += ' ORDER BY ' + primary_key
   row_list  = list()
   for row in cursor.execute(cmd) :
      row_tmp = list()
      for j in range(n_col) :
         row_tmp.append( row[j] )
      row_list.append(row_tmp)
   return row_list
