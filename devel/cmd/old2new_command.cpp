// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-23 Bradley M. Bell
// ----------------------------------------------------------------------------

# include <dismod_at/old2new_command.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/is_column_in_table.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
{xrst_begin old2new_command}

Convert an Old Database to New Format
#####################################

Syntax
******
``dismod_at`` *database* ``old2new``

data Table
**********
If the original data table does not contain a sample_size column,
this column is added with the value null for each row.
It is an error to run this command on a database that already has
a sample_size column in the data table.

Example
*******
{xrst_toc_list
   example/get_started/old2new_command.py
}

{xrst_end old2new_command}
*/

// ----------------------------------------------------------------------------
void old2new_command(sqlite3* db)
{  //
   // msg
   std::string msg;
   //
   // check if sample_name column is in data table
   std::string table_name  = "data";
   std::string column_name = "sample_size";
   if( dismod_at::is_column_in_table(db, table_name, column_name) )
   {  msg = "old2new commnad: sample_name is alread in data table";
      error_exit(msg, table_name);
   }
   //
   // add sample_name column to data table
   std::string sql_cmd = "ALTER TABLE data ADD COLUMN sample_size;";
   exec_sql_cmd(db, sql_cmd);
   //
   assert( dismod_at::is_column_in_table(db, table_name, column_name) );
   //
   return;
}

} // END_DISMOD_AT_NAMESPACE
