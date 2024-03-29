// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_covariate_table_xam.cpp dev}

C++ get_covariate_table: Example and Test
#########################################

{xrst_literal
   // BEGIN C++
   // END C++
}

{xrst_end get_covariate_table_xam.cpp}
*/
// BEGIN C++
# include <dismod_at/get_covariate_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool get_covariate_table_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;

   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);

   // sql commands
   const char* sql_cmd[] = {
   "create table covariate("
      " covariate_id   integer primary key,"
      " covariate_name text unique,"
      " reference      real,"
      " max_difference real)",
   "insert into covariate values(0, 'sex',     0.0,     0.6 )",
   "insert into covariate values(1, 'income',  2000.0,  null)"
   };
   size_t n_command = sizeof(sql_cmd) / sizeof(sql_cmd[0]);
   for(size_t i = 0; i < n_command; i++)
      dismod_at::exec_sql_cmd(db, sql_cmd[i]);


   // get the covariate table
   vector<dismod_at::covariate_struct>
      covariate_table = dismod_at::get_covariate_table(db);
   ok  &= covariate_table.size() == 2;
   //
   ok  &= covariate_table[0].covariate_name  == "sex";
   ok  &= covariate_table[0].reference       == 0.0;
   ok  &= covariate_table[0].max_difference  == 0.6;
   //
   ok  &= covariate_table[1].covariate_name  == "income";
   ok  &= covariate_table[1].reference       == 2000.0;
   ok  &= covariate_table[1].max_difference  == std::atof("+inf");
   //
   // close database and return
   sqlite3_close(db);
   return ok;
}
// END C++
