// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_covariate_table$$
$spell
   sqlite
   struct
   cpp
   std
   covariate
$$

$section C++: Get the Covariate Table Information$$

$head Syntax$$
$icode%covariate_table% = get_covariate_table(%db%)%$$

$head Purpose$$
To read the $cref covariate_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
   sqlite3* %db%
%$$
and is an open connection to the database.

$head covariate_struct$$
This is a structure with the following fields
$table
Type  $cnext Field $cnext Description
$rnext
$code std::string$$ $cnext $code covariate_name$$ $cnext
   The $cref/covariate_name/covariate_table/covariate_name/$$
   for this covariate
$rnext
$code double$$ $cnext $code reference$$  $cnext
   The $cref/reference/covariate_table/reference/$$
   value for this covariate
$rnext
$code double$$ $cnext $code max_difference$$  $cnext
   The $cref/max_difference/covariate_table/max_difference/$$
   value for this covariate
$tend

$head covariate_table$$
The return value $icode covariate_table$$ has prototype
$codei%
   CppAD::vector<covariate_struct>  %covariate_table%
%$$
For each $cref/covariate_id/covariate_table/covariate_id/$$,
$codei%
   %covariate_table%[%covariate_id%]
%$$
is the information for the corresponding covariate.

$children%example/devel/table/get_covariate_table_xam.cpp
%$$
$head Example$$
The file $cref get_covariate_table_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/



# include <cmath>
# include <limits>
# include <dismod_at/get_covariate_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<covariate_struct> get_covariate_table(sqlite3* db)
{  using std::string;
   double plus_infinity = std::numeric_limits<double>::infinity();

   string table_name         = "covariate";
   size_t n_covariate = check_table_id(db, table_name);

   string column_name        =  "covariate_name";
   CppAD::vector<string>  covariate_name;
   get_table_column(db, table_name, column_name, covariate_name);
   assert( n_covariate == covariate_name.size() );

   column_name           =  "reference";
   CppAD::vector<double>     reference;
   get_table_column(db, table_name, column_name, reference);
   assert( n_covariate == reference.size() );

   column_name           =  "max_difference";
   CppAD::vector<double>     max_difference;
   get_table_column(db, table_name, column_name, max_difference);
   assert( n_covariate == max_difference.size() );

   CppAD::vector<covariate_struct> covariate_table(n_covariate);
   for(size_t i = 0; i < n_covariate; i++)
   {  covariate_table[i].covariate_name  = covariate_name[i];
      covariate_table[i].reference       = reference[i];
      covariate_table[i].max_difference  = max_difference[i];
      if( std::isnan( max_difference[i] ) )
         covariate_table[i].max_difference  = plus_infinity;
      if( std::isnan(reference[i]) )
      {  std::string msg  = "reference value for a covariate is null";
         size_t covariate_id = i;
         error_exit(msg, table_name, covariate_id);
      }
   }
   return covariate_table;
}

} // END DISMOD_AT_NAMESPACE
