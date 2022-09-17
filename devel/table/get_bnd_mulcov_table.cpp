// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_bnd_mulcov_table$$
$spell
   sqlite
   struct
   bnd_mulcov
   cov_diff
$$

$section C++: Get the Data Subset Table$$

$head Syntax$$
$icode%bnd_mulcov_table_table% = get_bnd_mulcov_table(%db%)%$$

$head Prototype$$
$srcthisfile%0%// BEGIN_PROTOTYPE%// END_PROTOTYPE%1%$$

$head Purpose$$
To read the $cref bnd_mulcov_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
   sqlite3* %db%
%$$
and is an open connection to the database.

$head bnd_mulcov_table$$
For each $cref/bnd_mulcov_id/bnd_mulcov_table/bnd_mulcov_id/$$,
$codei%
   %bnd_mulcov_table%[%bnd_mulcov_id%]
%$$
is the information for the corresponding
$icode bnd_mulcov_id$$.

$head bnd_mulcov_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code double$$ $cnext $code max_cov_diff$$ $cnext
   The $cref/max_cov_diff/bnd_mulcov_table/max_cov_diff/$$
$rnext
$code double$$ $cnext $code max_mulcov$$ $cnext
   The $cref/max_mulcov/bnd_mulcov_table/max_mulcov/$$
$tend

$children%example/devel/table/get_bnd_mulcov_table_xam.cpp
%$$
$head Example$$
The file $cref get_bnd_mulcov_table_xam.cpp$$ contains an example
and test of this function.

$end
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_bnd_mulcov_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
CppAD::vector<bnd_mulcov_struct> get_bnd_mulcov_table(sqlite3* db)
// END_PROTOTYPE
{  using std::string;

   string table_name     = "bnd_mulcov";
   size_t n_bnd_mulcov   = check_table_id(db, table_name);

   string column_name    =   "max_cov_diff";
   CppAD::vector<double>       max_cov_diff;
   get_table_column(db, table_name, column_name, max_cov_diff);
   assert( max_cov_diff.size() == n_bnd_mulcov );

   column_name           =   "max_mulcov";
   CppAD::vector<double>       max_mulcov;
   get_table_column(db, table_name, column_name, max_mulcov);
   assert( max_mulcov.size() == n_bnd_mulcov );

   CppAD::vector<bnd_mulcov_struct> bnd_mulcov_table(n_bnd_mulcov);
   for(size_t i = 0; i < n_bnd_mulcov; i++)
   {
      bnd_mulcov_table[i].max_cov_diff = max_cov_diff[i];
      bnd_mulcov_table[i].max_mulcov   = max_mulcov[i];
   }
   return bnd_mulcov_table;
}

} // END DISMOD_AT_NAMESPACE
