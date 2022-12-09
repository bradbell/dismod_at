// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin check_table_id}

C++: Check That table_id Column Has Proper Values
#################################################

Syntax
******
*n_row* = ``check_table_id`` ( *db* , *table_name* )

Purpose
*******
Check that the column *table_name* _ ``id``
starts at zero and increments by one between rows.
It not, an error message is printed and the program exists.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

table_name
**********
This argument has prototype

   ``const std::string&`` *table_name*

It is the name of the table that we are checking.

n_row
*****
The return value has prototype

   ``size_t`` *n_row*

It is the number of rows in the table.

Speed
*****
Note that if one only wants to determine *n_row* ,
it should be faster to use :ref:`get_column_max-name` to find the maximum
value of *table_name* _ ``id`` (the primary key) and then add one;
see :ref:`database@Primary Key` .

{xrst_end check_table_id}
------------------------------------------------------------------------------
*/
# include <iostream>

# include <cppad/cppad.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at {
   size_t  check_table_id(sqlite3* db, const std::string& table_name )
   {  using std::string;
      string column_name = table_name + "_id";
      CppAD::vector<int>   table_id;
      get_table_column(db, table_name, column_name, table_id);

      size_t n_row = table_id.size();
      for(size_t i = 0; i < n_row; i++)
      {  if( table_id[i] != int(i) )
         {  string msg = table_name + "_id must start at zero";
            msg       += " and increment by one.";
            error_exit(msg, table_name, i);
         }
      }
      return n_row;
   }
}
