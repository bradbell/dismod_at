// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_smooth_table$$
$spell
   mulstd
   dage
   dtime
   struct
   sqlite
   enum
   cpp
   std
$$

$section C++: Get the Smoothing Table Information$$

$head Syntax$$
$icode%smooth_table% = get_smooth_table(%db%)%$$

$head Purpose$$
To read the $cref smooth_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
   sqlite3* %db%
%$$
and is an open connection to the database.

$head smooth_table$$
The return value $icode smooth_table$$ has prototype
$codei%
   CppAD::vector<smooth_struct>  %smooth_table%
%$$
For each $cref/smooth_id/smooth_table/smooth_id/$$,
$codei%
   %smooth_table%[%smooth_id%]
%$$
is the information for the corresponding
$cref/smooth_id/smooth_table/smooth_id/$$.

$head smooth_struct$$
This is a structure with the following fields
$table
Type $cnext Field $cnext Description
$rnext
$code std::string$$ $pre  $$ $cnext $code smooth_name$$ $cnext
   The $cref/smooth_name/smooth_table/smooth_name/$$ for this smoothing.
$rnext
$code int$$ $cnext $code n_age$$ $cnext
   The $cref/n_age/smooth_table/n_age/$$ for this smoothing.
$rnext
$code int$$ $cnext $code n_time$$ $cnext
   The $cref/n_time/smooth_table/n_time/$$ for this smoothing.
$rnext
$code int$$ $cnext $code mulstd_value_prior_id$$ $cnext
   The $cref/mulstd_value_prior_id/smooth_table/mulstd_value_prior_id/$$
   for this smoothing.
$rnext
$code int$$ $cnext $code mulstd_dage_prior_id$$ $cnext
   The $cref/mulstd_dage_prior_id/smooth_table/mulstd_dage_prior_id/$$
   for this smoothing.
$rnext
$code int$$ $cnext $code mulstd_dtime_prior_id$$ $cnext
   The $cref/mulstd_dtime_prior_id/smooth_table/mulstd_dtime_prior_id/$$
   for this smoothing.
$tend
$comment
   example/devel/table/get_smooth_grid_xam.cpp is included by smooth_grid.omh
%$$

$head Example$$
The file $cref get_smooth_grid_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_smooth_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<smooth_struct> get_smooth_table(sqlite3* db)
{  using std::string;

   string table_name  = "smooth";
   size_t n_smooth = check_table_id(db, table_name);
   if( n_smooth == 0 )
   {  string msg  = "smooth table is empty";
      error_exit(msg, table_name);
   }

   std::string column_name =  "smooth_name";
   CppAD::vector<string>       smooth_name;
   get_table_column(db, table_name, column_name, smooth_name);
   assert( smooth_name.size() == n_smooth );

   column_name        =  "n_age";
   CppAD::vector<int>     n_age;
   get_table_column(db, table_name, column_name, n_age);
   assert( n_age.size() == n_smooth );

   column_name        =  "n_time";
   CppAD::vector<int>     n_time;
   get_table_column(db, table_name, column_name, n_time);
   assert( n_time.size() == n_smooth );

   column_name        =  "mulstd_value_prior_id";
   CppAD::vector<int>     mulstd_value_prior_id;
   get_table_column(db, table_name, column_name, mulstd_value_prior_id);
   assert( mulstd_value_prior_id.size() == n_smooth );

   column_name        =  "mulstd_dage_prior_id";
   CppAD::vector<int>     mulstd_dage_prior_id;
   get_table_column(db, table_name, column_name, mulstd_dage_prior_id);
   assert( mulstd_dage_prior_id.size() == n_smooth );

   column_name        =  "mulstd_dtime_prior_id";
   CppAD::vector<int>     mulstd_dtime_prior_id;
   get_table_column(db, table_name, column_name, mulstd_dtime_prior_id);
   assert( mulstd_dtime_prior_id.size() == n_smooth );

   CppAD::vector<smooth_struct> smooth_table(n_smooth);
   for(size_t i = 0; i < n_smooth; i++)
   {  smooth_table[i].smooth_name             = smooth_name[i];
      smooth_table[i].n_age                   = n_age[i];
      smooth_table[i].n_time                  = n_time[i];
      smooth_table[i].mulstd_value_prior_id   = mulstd_value_prior_id[i];
      smooth_table[i].mulstd_dage_prior_id    = mulstd_dage_prior_id[i];
      smooth_table[i].mulstd_dtime_prior_id   = mulstd_dtime_prior_id[i];
   }
   return smooth_table;
}

} // END DISMOD_AT_NAMESPACE
