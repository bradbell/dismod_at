// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin put_table_row_xam.cpp$$
$spell
   xam
$$

$section C++ put_table_row: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/put_table_row.hpp>
# include <dismod_at/get_covariate_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>

bool put_table_row_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;

   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);

   // create the covariate table
   const char* sql_cmd =
   "create table covariate("
      "covariate_id       integer primary key, "
      "covariate_name     text, "
      "reference          real, "
      "max_difference     real"
   ")";
   dismod_at::exec_sql_cmd(db, sql_cmd);

   // setup for put_table_row
   std::string table_name = "covariate";
   CppAD::vector<std::string> col_name_vec(3), row_val_vec(3);

   // column names as a vector
   col_name_vec[0] = "covariate_name";
   col_name_vec[1] = "reference";
   col_name_vec[2] = "max_difference";

   // insert first row in the covariate table
   row_val_vec[0]   = "income";
   row_val_vec[1]   = "1000.00";
   row_val_vec[2]   = "null";
   size_t covariate_id = dismod_at::put_table_row(
      db, table_name, col_name_vec, row_val_vec
   );
   ok &= covariate_id == 0;

   // insert second row in the covariate table
   row_val_vec[0]  = "weight";
   row_val_vec[1]  = "100.00";
   row_val_vec[2]  = "200.00";
   covariate_id = dismod_at::put_table_row(
      db, table_name, col_name_vec, row_val_vec
   );
   ok &= covariate_id == 1;

   // get the covariate table
   vector<dismod_at::covariate_struct>
      covariate_table = dismod_at::get_covariate_table(db);
   ok  &= covariate_table.size() == 2;
   //
   ok  &= covariate_table[0].covariate_name == "income";
   ok  &= covariate_table[0].reference      == 1000.;
   // for max_difference, null is intrepreted as plus infinity
   ok  &= covariate_table[0].max_difference == std::atof("+inf");
   //
   ok  &= covariate_table[1].covariate_name == "weight";
   ok  &= covariate_table[1].reference      == 100.;
   ok  &= covariate_table[1].max_difference == 200;
   //
   // close database and return
   sqlite3_close(db);
   return ok;
}
// END C++
