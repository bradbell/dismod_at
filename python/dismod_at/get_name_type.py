# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin get_name_type$$ $newlinech #$$
# $spell
#  dismod
#  str
#  tbl
# $$
#
# $section Get Column Names and Types in a Table$$
#
# $head Syntax$$
# $codei%(%col_name%, %col_type%)%$$
# $codei% = dismod_at.get_name_type(%connection%, %tbl_name%)
# %$$
#
# $head connection$$
# is a $cref/connection/create_connection/connection/$$ for this database.
#
# $head table_name$$
# is a $code str$$ that specifies the name of the table.
#
# $head col_name$$
# is a list of strings containing the column names in the same order as they
# appear in the table.
#
# $head col_type$$
# is a list of strings containing the column types in the same order as
# the columns appear in the table.
# The possible values for the column types are
# $code integer$$, $code real$$, $code text$$, or
# $code integer primary key$$.
# Note that the types in the database are converted to lower case before
# being returned in $icode col_type$$.
#
# $head Primary Key$$
# This routine assumes the primary key is an integer,  corresponds
# to the first column, and has name $icode%tbl_name%_id%$$.
#
# $children%example/table/get_name_type.py
# %$$
# $head Example$$
# The file $cref get_name_type.py$$ is an example use of
# $code get_name_type$$.
#
# $end
# ---------------------------------------------------------------------------
import dismod_at
def get_name_type(connection, tbl_name) :
   #
   # database
   database = dismod_at.connection_file(connection)
   #
   # cursor
   cursor    = connection.cursor()
   #
   # check if table exists
   cmd     = "select * from sqlite_master where type='table' AND name="
   cmd    += "'" + tbl_name + "';"
   info    = cursor.execute(cmd).fetchall()
   if len(info) == 0 :
      msg = f'get_name_type: table {tbl_name} does not exist in {database}'
      assert False, msg
   #
   # pragma table_info for this table
   cmd       = 'pragma table_info(' + tbl_name + ');'
   found_pk  = False
   col_name  = list()
   col_type  = list()
   #
   # current column id
   cid       = 0
   #
   # row
   for row in cursor.execute(cmd) :
      assert cid == row[0]
      #
      col_name.append(row[1])
      col_type.append( row[2].lower() )
      pk            = row[5]
      if cid == 0 :
         if pk != 1 :
            msg     = f'{tbl_name} table in {database}'
            msg    += '\nfirst column not the primary key'
            assert False, msg
         assert found_pk == False
         assert col_type[cid] == 'integer'
         assert col_name[cid] == (tbl_name + '_id')
         col_type[cid]  =  'integer primary key'
         found_ok       = True
      else :
         if pk != 0 :
            msg     = f'{tbl_name} table in {database}'
            msg    += '\n muiltiple columns in primary key'
            assert False, msg
      cid += 1
   return (col_name, col_type)
