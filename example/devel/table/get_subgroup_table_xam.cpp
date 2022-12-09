// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_subgroup_table_xam.cpp}

C++ get_subgroup_table: Example and Test
########################################

{xrst_literal
   // BEGIN C++
   // END C++
}

{xrst_end get_subgroup_table_xam.cpp}
*/
// BEGIN C++
# include <dismod_at/get_subgroup_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_subgroup_table_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;

   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);

   // sql commands
   const char* sql_cmd[] = {
      "create table subgroup("
         "subgroup_id       integer primary key,"
         "subgroup_name     text unique,"
         "group_id          integer,"
         "group_name        text)",
      "insert into subgroup values(0, 'canada',  0, 'north_america')",
      "insert into subgroup values(1, 'mexico',  0, 'north_america')",
      "insert into subgroup values(2, 'brazil',  1, 'south_america')",
      "insert into subgroup values(3, 'chile',   1, 'south_america')",
   };
   size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
   for(size_t i = 0; i < n_command; i++)
      dismod_at::exec_sql_cmd(db, sql_cmd[i]);


   // get the subgroup table
   vector<dismod_at::subgroup_struct>
      subgroup_table = dismod_at::get_subgroup_table(db);
   //
   ok  &= subgroup_table.size() == 4;
   //
   ok  &= subgroup_table[0].subgroup_name == "canada";
   ok  &= subgroup_table[1].subgroup_name == "mexico";
   ok  &= subgroup_table[2].subgroup_name == "brazil";
   ok  &= subgroup_table[3].subgroup_name == "chile";
   //
   ok  &= subgroup_table[0].group_id == 0;
   ok  &= subgroup_table[1].group_id == 0;
   ok  &= subgroup_table[2].group_id == 1;
   ok  &= subgroup_table[3].group_id == 1;
   //
   ok  &= subgroup_table[0].group_name == "north_america";
   ok  &= subgroup_table[1].group_name == "north_america";
   ok  &= subgroup_table[2].group_name == "south_america";
   ok  &= subgroup_table[3].group_name == "south_america";
   //
   // close database and return
   sqlite3_close(db);
   return ok;
}
// END C++
