// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_prior_sim_table_xam.cpp$$
$spell
   sim
   xam
$$

$section C++ get_prior_sim_table: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/get_prior_sim_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_prior_sim_table_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;

   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);

   // sql commands
   const char* sql_cmd[] = {
   "create table prior_sim("
      " prior_sim_id    integer primary key,"
      " simulate_index  integer,"
      " var_id          integer,"
      " prior_sim_value real,"
      " prior_sim_dage  real,"
      " prior_sim_dtime real"
   ")",
   "insert into prior_sim values(0, 0, 0, 1e-1, 1e-2, 1e-3)",
   "insert into prior_sim values(1, 0, 1, 2e-1, 2e-2, 2e-3)",
   "insert into prior_sim values(2, 0, 2, 3e-1, 3e-2, 3e-3)",
   "insert into prior_sim values(3, 1, 0, 4e-1, 4e-2, 4e-3)",
   "insert into prior_sim values(4, 1, 1, 5e-1, 5e-2, 5e-3)",
   "insert into prior_sim values(5, 1, 2, 6e-1, 6e-2, 6e-3)"
   };
   size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
   for(size_t i = 0; i < n_command; i++)
      dismod_at::exec_sql_cmd(db, sql_cmd[i]);

   // get the prior_sim table
   vector<dismod_at::prior_sim_struct> prior_sim_table =
      dismod_at::get_prior_sim_table(db);
   ok  &= prior_sim_table.size() == 6;
   //
   ok  &= prior_sim_table[0].simulate_index == 0;
   ok  &= prior_sim_table[1].simulate_index == 0;
   ok  &= prior_sim_table[2].simulate_index == 0;
   ok  &= prior_sim_table[3].simulate_index == 1;
   ok  &= prior_sim_table[4].simulate_index == 1;
   ok  &= prior_sim_table[5].simulate_index == 1;
   //
   ok  &= prior_sim_table[0].var_id         == 0;
   ok  &= prior_sim_table[1].var_id         == 1;
   ok  &= prior_sim_table[2].var_id         == 2;
   ok  &= prior_sim_table[3].var_id         == 0;
   ok  &= prior_sim_table[4].var_id         == 1;
   ok  &= prior_sim_table[5].var_id         == 2;
   //
   ok  &= prior_sim_table[0].prior_sim_value == 1e-1;
   ok  &= prior_sim_table[1].prior_sim_value == 2e-1;
   ok  &= prior_sim_table[2].prior_sim_value == 3e-1;
   ok  &= prior_sim_table[3].prior_sim_value == 4e-1;
   ok  &= prior_sim_table[4].prior_sim_value == 5e-1;
   ok  &= prior_sim_table[5].prior_sim_value == 6e-1;
   //
   ok  &= prior_sim_table[0].prior_sim_dage  == 1e-2;
   ok  &= prior_sim_table[1].prior_sim_dage  == 2e-2;
   ok  &= prior_sim_table[2].prior_sim_dage  == 3e-2;
   ok  &= prior_sim_table[3].prior_sim_dage  == 4e-2;
   ok  &= prior_sim_table[4].prior_sim_dage  == 5e-2;
   ok  &= prior_sim_table[5].prior_sim_dage  == 6e-2;
   //
   ok  &= prior_sim_table[0].prior_sim_dtime == 1e-3;
   ok  &= prior_sim_table[1].prior_sim_dtime == 2e-3;
   ok  &= prior_sim_table[2].prior_sim_dtime == 3e-3;
   ok  &= prior_sim_table[3].prior_sim_dtime == 4e-3;
   ok  &= prior_sim_table[4].prior_sim_dtime == 5e-3;
   ok  &= prior_sim_table[5].prior_sim_dtime == 6e-3;
   //
   // close database and return
   sqlite3_close(db);
   return ok;
}
// END C++
