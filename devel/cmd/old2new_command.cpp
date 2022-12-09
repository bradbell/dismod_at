// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------

# include <dismod_at/old2new_command.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/log_message.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/create_table.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
-----------------------------------------------------------------------------
{xrst_begin old2new_command}

Convert an Old Database to New Format
#####################################

Syntax
******
``dismod_at`` *database* ``old2new``

Subgroup Table
**************
The :ref:`subgroup_table-name` was added to the database input tables on
:ref:`2019-11-22<whats_new_2019@mm-dd@11-22>` .
If the original database does not contain a subgroup table,
it is added with one row and the following values:

| |tab| ``subgroup_id`` ``subgroup_name`` ``group_id`` ``group_name``
| |tab| 0 *world* 0 *world*

data Table
**********
If the original data base does not contain a subgroup table,
the column :ref:`data_table@subgroup_id` is added to the
:ref:`data_table-name` with the value *subgroup_id*  = 0 for every row.

avgint Table
************
If the original data base does not contain a subgroup table,
the column :ref:`avgint_table@subgroup_id` is added to the
:ref:`avgint_table-name` with the value *subgroup_id*  = 0 for every row.

mulcov Table
************
If the original data base does not contain a subgroup table,
the mulcov table column named *smooth_id* is changed to be named
:ref:`mulcov_table@group_smooth_id` .
In addition, the columns
:ref:`mulcov_table@group_id` and
:ref:`mulcov_table@subgroup_smooth_id`
are added to the :ref:`mulcov_table-name` with values
*group_id*  = 0 , and *subgroup_smooth_id* = ``null``
for every row.

option Table
************
The ``zero_sum_random`` option name was changed to
:ref:`option_table@Zero Sum Constraints@zero_sum_child_rate` on
:ref:`2019-11-29<whats_new_2019@mm-dd@11-29>` .
If the option name ``zero_sum_random`` is in the option table, it is
changed to :ref:`option_table@Zero Sum Constraints@zero_sum_child_rate` .

{xrst_end old2new_command}
*/

// ----------------------------------------------------------------------------
void old2new_command(sqlite3* db)
{  using std::string;
   using CppAD::vector;
   //
   // check if subgroup table exists
   string sql_cmd = "SELECT name FROM sqlite_master WHERE name='subgroup'";
   char   sep = ',';
   string sql_ret = exec_sql_cmd(db, sql_cmd, sep);
   if( sql_ret.size() == 0 )
   {  string msg = "Adding subgroup informtion to following tables: ";
      msg += "subgroup, data, avgint, mulcov";
      string msg_type = "warning";
      log_message(db, &std::cout, msg_type, msg);
      //
      // --------------------------------------------------------------------
      // subgroup table
      string table_name = "subgroup";
      vector<string> col_name(3);
      vector<string> col_type(3);
      vector<bool>   col_unique(3);
      vector<string> row_value(3);
      //
      col_name[0]   = "subgroup_name";
      col_type[0]   = "text";
      col_unique[0] = true;
      row_value[0]  = "world";
      //
      col_name[1]   = "group_id";
      col_type[1]   = "integer";
      col_unique[1] = false;
      row_value[1]  = "0";
      //
      col_name[2]   = "group_name";
      col_type[2]   = "text";
      col_unique[2] = false;
      row_value[2]  = "world";
      //
      create_table(
         db, table_name, col_name, col_type, col_unique, row_value
      );
      // -------------------------------------------------------------------
      // mulcov table
      //
      // rename smooth_id -> group_smooth_id
      sql_cmd =
      "ALTER TABLE mulcov RENAME COLUMN smooth_id TO group_smooth_id";
      exec_sql_cmd(db, sql_cmd);
      //
      // mulcov_table: Add group_id column
      sql_cmd = "ALTER TABLE mulcov ADD COLUMN group_id integer";
      exec_sql_cmd(db, sql_cmd);
      //
      // mulcov_table: Add subgroup_smooth_id column
      sql_cmd = "ALTER TABLE mulcov ADD COLUMN subgroup_smooth_id integer";
      exec_sql_cmd(db, sql_cmd);
      //
      // mulcov_table: set group_id and subgroup_smooth_id to zero
      sql_cmd = "UPDATE mulcov SET group_id = 0, subgroup_smooth_id = null";
      exec_sql_cmd(db, sql_cmd);
      // -------------------------------------------------------------------
      // data table
      sql_cmd = "ALTER TABLE data ADD COLUMN subgroup_id integer";
      exec_sql_cmd(db, sql_cmd);
      sql_cmd = "UPDATE data SET subgroup_id = 0";
      exec_sql_cmd(db, sql_cmd);
      // -------------------------------------------------------------------
      // avgint table
      sql_cmd = "ALTER TABLE avgint ADD COLUMN subgroup_id integer";
      exec_sql_cmd(db, sql_cmd);
      sql_cmd = "UPDATE avgint SET subgroup_id = 0";
      exec_sql_cmd(db, sql_cmd);
      // -------------------------------------------------------------------
   }
   //
   // check if zero_sum_random is in option table
   sql_cmd = "SELECT * FROM option WHERE option_name='zero_sum_random'";
   sep     = ',';
   sql_ret = exec_sql_cmd(db, sql_cmd, sep);
   if( sql_ret.size() != 0 )
   {  string msg = "option table: ";
      msg += "Changing zero_sum_random to zero_sum_child_rate.";
      string msg_type = "warning";
      log_message(db, &std::cout, msg_type, msg);
      //
      sql_cmd  = "UPDATE option SET option_name = 'zero_sum_child_rate' ";
      sql_cmd += "WHERE option_name == 'zero_sum_random'";
      exec_sql_cmd(db, sql_cmd);
   }
   //
   return;
}

} // END_DISMOD_AT_NAMESPACE
