# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin get_table_dict}
# {xrst_spell
#     sql
#     tbl
# }
# {xrst_comment_ch #}
#
# Get All Data From a Table
# #########################
#
# Syntax
# ******
# *table_dict* = ``dismod_at.get_table_dict`` ( *connection* , *tbl_name* )
#
# connection
# **********
# is a :ref:`create_connection@connection` for this database.
#
# tbl_name
# ********
# is a ``str`` that specifies the name of the table.
# We use the notation *n_row* for the number of rows in
# the table.
#
# table_dict
# **********
# This is a list, with length *n_row* , where each element
# of the list is a dictionary.
# For each dictionaries, the set of keys is the column names in the table
# (excluding the primary key *tbl_name* _ ``id`` ).
# The value *table_dict* [ *i* ][ *col_name* ] corresponds to
# and the column with name *col_name* and primary key
# *tbl_name* _ ``id`` = *i* .
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
# {xrst_toc_hidden
#    example/table/get_table_dict.py
# }
# Example
# *******
# The file :ref:`get_table_dict.py-name` is an example use of
# ``get_table_dict`` .
#
# {xrst_end get_table_dict}
# ---------------------------------------------------------------------------
def get_table_dict(connection, tbl_name) :
   import dismod_at
   #
   (col_name, col_type) = dismod_at.get_name_type(connection, tbl_name)
   assert col_name[0] == tbl_name + '_id'
   del col_name[0]
   #
   row_list   = dismod_at.get_row_list(connection, tbl_name, col_name)
   table_dict = list()
   for row in row_list :
      table_dict.append( dict( zip(col_name, row) ) )
   return table_dict
