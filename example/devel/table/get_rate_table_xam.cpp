// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_rate_table_xam.cpp$$
$spell
   xam
   nslist
$$

$section C++ get_rate_table: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/null_int.hpp>

bool get_rate_table_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;

   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);

   // sql commands
   const char* sql_cmd[] = {
      "create table rate("
         "rate_id           integer primary key,"
         "rate_name         text unique,"
         "parent_smooth_id  integer,"
         "child_smooth_id   integer,"
         "child_nslist_id   integer)",
      "insert into rate values(0, 'pini',  0, 1, null)",
      "insert into rate values(1, 'iota',  0, 1, null)",
      "insert into rate values(2, 'rho',   0, 1, null)",
      "insert into rate values(3, 'chi',   0, 1, null)",
      "insert into rate values(4, 'omega', 0, 1, null)"
   };
   size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
   for(size_t i = 0; i < n_command; i++)
      dismod_at::exec_sql_cmd(db, sql_cmd[i]);


   // get the rate table
   vector<dismod_at::rate_struct> rate_table = dismod_at::get_rate_table(db);
   size_t n_rate = dismod_at::number_rate_enum;
   ok  &= rate_table.size() == n_rate;
   for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
   {  ok &= rate_table[rate_id].parent_smooth_id == 0;
      ok &= rate_table[rate_id].child_smooth_id  == 1;
      ok &= rate_table[rate_id].child_nslist_id  == DISMOD_AT_NULL_INT;

      // check that one can use rate_enum values in place of rate_id
      dismod_at::rate_enum rate = rate_table[rate_id].rate;
      ok &= rate == dismod_at::rate_enum( rate_id );
   }

   // close database and return
   sqlite3_close(db);
   return ok;
}
// END C++
