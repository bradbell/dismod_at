// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin does_table_exist dev}
{xrst_spell
   hpp
}

Check if A Table Exists
#######################

Syntax
******

| ``# include <dismod_at/does_table_exist.hpp>``
| *exist* = ``does_table_exist`` ( *db* , *table_name* )

Prototype
*********
{xrst_literal
   // BEGIN_PROTOTYPE
   // END_PROTOTYPE
}

db
**
The argument *db* is an open connection to the database.

table_name
**********
is the name of the table we are checking for in the database.

exist
*****
the return value *exist* is true (false) if the table
exist (does not exist) in this database.

{xrst_end does_table_exist}
*/
# include <dismod_at/does_table_exist.hpp>
# include <dismod_at/exec_sql_cmd.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
bool does_table_exist(sqlite3* db, const std::string table_name)
// END_PROTOTYPE
{
   // sql_cmd
   std::string sql_cmd;
   sql_cmd  = "SELECT name FROM sqlite_master WHERE name=";
   sql_cmd += "'" + table_name + "'";
   //
   // sql_result
   char sep = ',';
   std::string sql_result = exec_sql_cmd(db, sql_cmd, sep);
   //
   // exist
   bool exist = 0 < sql_result.size();
   return exist;
}

} // END_DISMOD_AT_NAMESPACE
