# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin get_table_dict$$ $newlinech #$$
# $spell
#  dismod
#  tbl
#  str
#  Sql
# $$
#
# $nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section Get All Data From a Table$$
#
# $head Syntax$$
# $icode%table_dict% = dismod_at.get_table_dict(%connection%, %tbl_name%)%$$
#
# $head connection$$
# is a $cref/connection/create_connection/connection/$$ for this database.
#
# $head tbl_name$$
# is a $code str$$ that specifies the name of the table.
# We use the notation $icode n_row$$ for the number of rows in
# the table.
#
# $head table_dict$$
# This is a list, with length $icode n_row$$, where each element
# of the list is a dictionary.
# For each dictionaries, the set of keys is the column names in the table
# (excluding the primary key $icode%tbl_name%_id%$$).
# The value $icode%table_dict%[%i%][%col_name%]%$$ corresponds to
# and the column with name $icode col_name$$ and primary key
# $icode%tbl_name%_id = %i%$$.
# The python type corresponding to the values in the table are
# as follows:
# $table
# Sql Table $pre  $$ $cnext  Python           $rnext
# $code integer$$    $cnext  $code int$$      $rnext
# $code real$$       $cnext  $code float$$    $rnext
# $code text$$       $cnext  $code str$$      $rnext
# $code null$$       $cnext  $code None$$
# $tend
# You can determine the type for all the columns in the table using
# $cref get_name_type$$.
#
# $children%example/table/get_table_dict.py
# %$$
# $head Example$$
# The file $cref get_table_dict.py$$ is an example use of
# $code get_table_dict$$.
#
# $end
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
