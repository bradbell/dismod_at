// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_nslist_table_xam.cpp$$
$spell
   nslist
   xam
$$

$section C++ get_nslist_table: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/get_nslist_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_nslist_table_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;

   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);

   // sql commands
   const char* sql_cmd[] = {
      "create table nslist(nslist_id integer primary key, nslist_name text)",
      "insert into nslist values(0, 'first_list')"    ,
      "insert into nslist values(1, 'second_list')"
   };
   size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
   for(size_t i = 0; i < n_command; i++)
      dismod_at::exec_sql_cmd(db, sql_cmd[i]);


   // get the nslist table
   vector<string> nslist_table = dismod_at::get_nslist_table(db);
   ok  &= nslist_table.size() == 2;
   ok  &= nslist_table[0] == "first_list";
   ok  &= nslist_table[1] == "second_list";

   // close database and return
   sqlite3_close(db);
   return ok;
}
// END C++
