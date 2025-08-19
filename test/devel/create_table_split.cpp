// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-25 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
Check that create table commands with split insertions execute properly.
*/
# include <cppad/utility/to_string.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/get_age_table.hpp>
# include <dismod_at/create_table.hpp>

namespace { // BEGIN EMPTY_NAMESPACE

// put bool on its own line so that test_one.sh works correctly
bool
test_create_table_split(void)
{  //
   // ok
   bool   ok = true;
   //
   using  std::string;
   using  CppAD::vector;
   using  CppAD::to_string;
   //
   // db
   string   file_name = "example.db";
   bool     new_file  = true;
   sqlite3* db        = dismod_at::open_connection(file_name, new_file);
   //
   // db
   // wreate the age table, note max_insert is less than n_row.
   string table_name         = "age";
   size_t n_col              = 1;
   size_t n_row              = 5;
   vector<string> col_name   = {"age"};
   vector<string> col_type   = {"real"};
   vector<bool>   col_unique = {false};
   size_t max_insert         = 3;
   //
   vector<string> row_value(n_col * n_row);
   for(size_t k = 0; k < n_row; ++k)
      row_value[n_col * k] = to_string(k);
   dismod_at::create_table(
      db, table_name, col_name, col_type, col_unique, row_value, max_insert
   );
   //
   // age_table
   vector<double> age_table = dismod_at::get_age_table(db);
   // std::cout << "age table: " << age_table << std::endl;
   //
   // ok
   for(size_t k = 0; k < n_row; k++)
   {
      double row_val = static_cast<double>(k);
      ok &= age_table[ k ] == row_val;
   }
   //
   return ok;
}

} // END EMPTY_NAMESPACE


bool create_table_split(void)
{  bool ok = true;
   ok &= test_create_table_split();
   return ok;
}
