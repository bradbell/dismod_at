// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_data_subset$$
$spell
   sqlite
   struct
$$

$section C++: Get the Data Subset Table$$

$head Syntax$$
$icode%data_subset_table% = get_data_subset(%db%)%$$

$head Prototype$$
$srcthisfile%0%// BEGIN_PROTOTYPE%// END_PROTOTYPE%1%$$

$head Purpose$$
To read the $cref data_subset_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
   sqlite3* %db%
%$$
and is an open connection to the database.

$head data_subset_table$$
For each $cref/data_subset_id/data_subset_table/data_subset_id/$$,
$codei%
   %data_subset_table%[%data_subset_id%]
%$$
is the information for the corresponding
$icode data_subset_id$$.

$head data_subset_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code int$$ $cnext $code data_id$$ $cnext
   The $cref/data_id/data_subset_table/data_id/$$
   for this measurement.
$rnext
$code int$$ $cnext $code hold_out$$ $cnext
   The $cref/hold_out/data_subset_table/hold_out/$$
   for this measurement.
$rnext
$code int$$ $cnext $code density_id$$ $cnext
   The $cref/density_id/data_subset_table/density_id/$$
   for this measurement.
$rnext
$code int$$ $cnext $code eta$$ $cnext
   The $cref/eta/data_subset_table/eta/$$
   for this measurement.
$rnext
$code int$$ $cnext $code nu$$ $cnext
   The $cref/nu/data_subset_table/nu/$$
   for this measurement.
$tend

$children%example/devel/table/get_data_subset_xam.cpp
%$$
$head Example$$
The file $cref get_data_subset_xam.cpp$$ contains an example
and test of this function.

$end
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_data_subset.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
CppAD::vector<data_subset_struct> get_data_subset(sqlite3* db)
// END_PROTOTYPE
{  using std::string;

   string table_name     = "data_subset";
   size_t n_data_subset  = check_table_id(db, table_name);

   std::string column_name =  "data_id";
   CppAD::vector<int>          data_id;
   get_table_column(db, table_name, column_name, data_id);
   assert( data_id.size() == n_data_subset );

   column_name             =  "hold_out";
   CppAD::vector<int>          hold_out;
   get_table_column(db, table_name, column_name, hold_out);
   assert( hold_out.size() == n_data_subset );

   column_name             =  "density_id";
   CppAD::vector<int>          density_id;
   get_table_column(db, table_name, column_name, density_id);
   assert( hold_out.size() == n_data_subset );

   column_name             =  "eta";
   CppAD::vector<double>       eta;
   get_table_column(db, table_name, column_name, eta);
   assert( hold_out.size() == n_data_subset );

   column_name             =  "nu";
   CppAD::vector<double>       nu;
   get_table_column(db, table_name, column_name, nu);
   assert( hold_out.size() == n_data_subset );

   CppAD::vector<data_subset_struct> data_subset_table(n_data_subset);
   for(size_t i = 0; i < n_data_subset; i++)
   {  data_subset_table[i].data_id    = data_id[i];
      data_subset_table[i].hold_out   = hold_out[i];
      data_subset_table[i].density_id  = density_id[i];
      data_subset_table[i].eta         = eta[i];
      data_subset_table[i].nu          = nu[i];
   }
   return data_subset_table;
}

} // END DISMOD_AT_NAMESPACE
