// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-23 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin is_column_in_table dev}

Is Column in Table
##################

Syntax
******
{xrst_literal
   // BEGIN_PROTOTYPE
   // END_PROTOTYPE
}


db
**
This is an open connection to the database.

table_name
**********
This is the name of the table that we are checking for a certain column.
This table must be in the database.

column_name
***********
This is the name of the column we are looking for.

flag
****
The return value *flag* is true (false) if the column is (is not)
in the specified table.

{xrst_end is_column_in_table}
*/
# include <dismod_at/is_column_in_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
// flag =
bool is_column_in_table(
   sqlite3*           db          ,
   const std::string& table_name  ,
   const std::string& column_name )
// END_PROTOTYPE
{
   char sep = ',';
   std::string sql_cmd = "PRAGMA table_info(" + table_name + ")";
   std::string sql_ret = exec_sql_cmd(db, sql_cmd, sep);
   std::string column_type = "";
   size_t next_line       = 0;
   while( next_line < sql_ret.size() && column_type == "" )
   {  // start of next column name in this table
      size_t start = sql_ret.find(sep, next_line) + 1;
      size_t stop  = sql_ret.find(sep, start);
      std::string column = sql_ret.substr(start, stop - start);
      if( column == column_name )
         return true;
      //
      next_line = sql_ret.find('\n', next_line) + 1;
   }
   return false;
}

} // END_DISMOD_AT_NAMESPACE
