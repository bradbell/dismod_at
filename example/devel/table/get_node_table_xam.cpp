// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_node_table_xam.cpp$$
$spell
   xam
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section C++ get_node_table: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/get_node_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/null_int.hpp>

bool get_node_table_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;

   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);

   // sql commands
   const char* sql_cmd[] = {
      "create table node"
      "(node_id integer primary key, node_name text unique, parent int)",
      "insert into node values(0, 'world',          null)",
      "insert into node values(1, 'north_america',  0)",
      "insert into node values(2, 'united_states',  1)",
      "insert into node values(3, 'canada',         1)"
   };
   size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
   for(size_t i = 0; i < n_command; i++)
      dismod_at::exec_sql_cmd(db, sql_cmd[i]);


   // get the node table
   vector<dismod_at::node_struct> node_table = dismod_at::get_node_table(db);
   ok  &= node_table.size() == 4;
   //
   ok  &= node_table[0].node_name == "world";
   ok  &= node_table[1].node_name == "north_america";
   ok  &= node_table[2].node_name == "united_states";
   ok  &= node_table[3].node_name == "canada";
   //
   ok  &= node_table[0].parent == DISMOD_AT_NULL_INT;
   ok  &= node_table[1].parent == 0;
   ok  &= node_table[2].parent == 1;
   ok  &= node_table[3].parent == 1;
   //
   // close database and return
   sqlite3_close(db);
   return ok;
}
// END C++
