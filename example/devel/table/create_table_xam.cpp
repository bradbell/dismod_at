// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin create_table_xam.cpp$$
$spell
   xam
$$

$section C++ create_table: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/create_table.hpp>
# include <dismod_at/exec_sql_cmd.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/get_covariate_table.hpp>

bool create_table_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;

   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);

   // ----------------------------------------------------------------------
   // create a covariate table
   size_t n_col = 3;
   size_t n_row = 2;
   std::string table_name = "covariate";
   vector<string> col_name(n_col), col_type(n_col), row_value(n_col * n_row);
   vector<bool>   col_unique(n_col);
   //
   col_name[0]     = "covariate_name";
   col_type[0]     = "text";
   col_unique[0]   = true;
   //
   col_name[1]     = "reference";
   col_type[1]     = "real";
   col_unique[1]   = false;
   //
   col_name[2]     = "max_difference";
   col_type[2]     = "real";
   col_unique[2]   = false;
   //
   size_t i = 0;
   row_value[ n_col * i + 0 ] = "income";
   row_value[ n_col * i + 1 ] = "1000.00";
   row_value[ n_col * i + 2 ] = "";
   //
   i = 1;
   row_value[ n_col * i + 0 ] = "weight";
   row_value[ n_col * i + 1 ] = "100";
   row_value[ n_col * i + 2 ] = "200";
   //
   dismod_at::create_table(
      db, table_name, col_name, col_type, col_unique, row_value
   );
   // ----------------------------------------------------------------------

   // get the covariate table
   vector<dismod_at::covariate_struct>
      covariate_table = dismod_at::get_covariate_table(db);
   ok  &= covariate_table.size() == n_row;
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
