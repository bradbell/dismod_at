// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_data_sim_table_xam.cpp}

C++ get_data_sim_table: Example and Test
########################################

{xrst_literal
   // BEGIN C++
   // END C++
}

{xrst_end get_data_sim_table_xam.cpp}
*/
// BEGIN C++
# include <dismod_at/get_data_sim_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_data_sim_table_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;

   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);

   // sql commands
   const char* sql_cmd[] = {
   "create table data_sim("
      " data_sim_id    integer primary key,"
      " simulate_index integer,"
      " data_subset_id integer,"
      " data_sim_value real"
   ")",
   "insert into data_sim values(0, 0, 0, 0e-3)",
   "insert into data_sim values(1, 0, 1, 1e-3)",
   "insert into data_sim values(2, 0, 2, 2e-3)",
   "insert into data_sim values(3, 1, 0, 0e-3)",
   "insert into data_sim values(4, 1, 1, 1e-3)",
   "insert into data_sim values(5, 1, 2, 2e-3)"
   };
   size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
   for(size_t i = 0; i < n_command; i++)
      dismod_at::exec_sql_cmd(db, sql_cmd[i]);


   // get the data_sim table
   vector<dismod_at::data_sim_struct> data_sim_table =
      dismod_at::get_data_sim_table(db);
   ok  &= data_sim_table.size() == 6;
   //
   ok  &= data_sim_table[0].simulate_index == 0;
   ok  &= data_sim_table[1].simulate_index == 0;
   ok  &= data_sim_table[2].simulate_index == 0;
   ok  &= data_sim_table[3].simulate_index == 1;
   ok  &= data_sim_table[4].simulate_index == 1;
   ok  &= data_sim_table[5].simulate_index == 1;
   //
   ok  &= data_sim_table[0].data_subset_id == 0;
   ok  &= data_sim_table[1].data_subset_id == 1;
   ok  &= data_sim_table[2].data_subset_id == 2;
   ok  &= data_sim_table[3].data_subset_id == 0;
   ok  &= data_sim_table[4].data_subset_id == 1;
   ok  &= data_sim_table[5].data_subset_id == 2;
   //
   ok  &= data_sim_table[0].data_sim_value == 0e-3;
   ok  &= data_sim_table[1].data_sim_value == 1e-3;
   ok  &= data_sim_table[2].data_sim_value == 2e-3;
   ok  &= data_sim_table[3].data_sim_value == 0e-3;
   ok  &= data_sim_table[4].data_sim_value == 1e-3;
   ok  &= data_sim_table[5].data_sim_value == 2e-3;
   //
   // close database and return
   sqlite3_close(db);
   return ok;
}
// END C++
