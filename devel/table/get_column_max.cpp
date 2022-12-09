// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_column_max}
{xrst_spell
   cmd
}

C++: Get The Maximum Values in a Table Column
#############################################

Syntax
******
*max_str* = ``get_column_max`` ( *db* , *select_cmd* , *column_name* )

db
**
This argument has prototype

   ``sqlite3`` * *db*

and is the database we are getting information from.

select_cmd
**********
This argument has prototype

   ``const std::string&`` *select_cmd*

and is a SQL select command that results in a table,
that has a column named *column_name* .
For example, if you want to take the maximum over all the rows
in table *table_name* ,
you could use the following for *select_cmd* :

   ``select`` * ``from`` *table_name*

.

column_name
***********
This argument has prototype

   ``const std::string&`` *column_name*

and is the name of the column we are getting information from.

max_str
*******
The return value has prototype

   ``std::string`` *max_str*

and is the maximum value in the column.
If the table is empty,
or if all the values in the column are ``null`` ,
the return value of *max_str* is the empty string.
{xrst_toc_hidden
   example/devel/table/get_column_max_xam.cpp
}
Example
*******
The file :ref:`get_column_max_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_column_max}
------------------------------------------------------------------------------
*/
# include <iostream>
# include <cassert>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/configure.hpp>
# include <dismod_at/error_exit.hpp>

namespace {
   size_t count_callback;
   int callback(void *result, int argc, char **argv, char **azColName)
   {  count_callback++;
      assert(count_callback == 1);
      assert( argc == 1 );
      assert( result != DISMOD_AT_NULL_PTR );
      std::string* str_ptr = static_cast<std::string*>(result);
      if( argv[0] == DISMOD_AT_NULL_PTR )
         *str_ptr = "";
      else
         *str_ptr = argv[0];
      return 0;
   }
}
namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

std::string get_column_max(
   sqlite3*                    db                    ,
   const std::string&          select_cmd            ,
   const std::string&          column_name           )
{
   // sql command: select max(column_name) from ( select_cmd ) sub
   std::string cmd = "select max(";
   cmd            += column_name;
   cmd            += ") from (";
   cmd            += select_cmd;
   cmd            += ") sub";

   // execute sql command
   count_callback = 0;
   char* zErrMsg  = DISMOD_AT_NULL_PTR;
   std::string max_str;
   int rc = sqlite3_exec(db, cmd.c_str(), callback, &max_str, &zErrMsg);
   if( rc )
   {  assert(zErrMsg != DISMOD_AT_NULL_PTR );
      std::string message = "SQL error: ";
      message += sqlite3_errmsg(db);
      message += ". SQL command: " + cmd;
      sqlite3_free(zErrMsg);
      error_exit(message);
   }
   assert( count_callback == 1 );
   return max_str;
}

} // END DISMOD_AT_NAMESPACE
