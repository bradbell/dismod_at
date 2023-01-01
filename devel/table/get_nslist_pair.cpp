// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin get_nslist_pair$$
$spell
   nslist
   sqlite
   struct
   cpp
   std
   smoothings
$$

$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section C++: Get the List of Smoothing Table$$

$head Syntax$$
$icode%nslist_pair% = get_nslist_pair_table(%db%)%$$

$head Purpose$$
To read the $cref nslist_pair_table$$ and return it as a C++ data structure.

$head db$$
The argument $icode db$$ has prototype
$codei%
   sqlite3* %db%
%$$
and is an open connection to the database.

$head nslist_pair$$
The return value $icode nslist_pair$$ has prototype
$codei%
   CppAD::vector<nslist_pair_struct> %nslist_pair%
%$$
For each $cref/nslist_pair_id/nslist_pair_table/nslist_pair_id/$$,
$codei%
   %nslist_pair_table%[%nslist_pair_id%]
%$$
is the information for one entry in one smoothing list.

$head nslist_pair_struct$$
This is a structure with the following fields
$table
$code int$$ $cnext $code density_id$$  $cnext
   $cref/nslist_id/nslist_pair_table/nslist_id/$$
   identifies this list of smoothings.
$rnext
$code int$$ $cnext $code node_id$$ $cnext
   $cref/node_id/node_table/node_id/$$ is the node that this smoothing
   (in this list) applies to.
$rnext
$code int$$ $cnext $code smooth_id$$ $cnext
   $cref/smooth_id/smooth_table/smooth_id/$$ is the smoothing
   that is applied to this node (for this list).
$tend

$children%example/devel/table/get_nslist_pair_xam.cpp
%$$
$head Example$$
The file $cref get_nslist_pair_xam.cpp$$ contains an example that uses
this function.

$end
-----------------------------------------------------------------------------
*/
# include <cmath>
# include <dismod_at/get_nslist_pair.hpp>
# include <dismod_at/get_table_column.hpp>
# include<dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<nslist_pair_struct> get_nslist_pair(sqlite3* db)
{  using std::string;

   // user for error messaging
   string msg, table_name, column_name;

   table_name         = "nslist_pair";
   size_t n_nslist_pair  = check_table_id(db, table_name);

   column_name        =  "nslist_id";
   CppAD::vector<int>     nslist_id;
   get_table_column(db, table_name, column_name, nslist_id);
   assert( nslist_id.size() == n_nslist_pair );

   column_name        =  "node_id";
   CppAD::vector<int>     node_id;
   get_table_column(db, table_name, column_name, node_id);
   assert( node_id.size() == n_nslist_pair );

   column_name        =  "smooth_id";
   CppAD::vector<int>     smooth_id;
   get_table_column(db, table_name, column_name, smooth_id);
   assert( smooth_id.size() == n_nslist_pair );

   CppAD::vector<nslist_pair_struct> nslist_pair(n_nslist_pair);
   for(size_t i = 0; i < n_nslist_pair; i++)
   {  nslist_pair[i].nslist_id          = nslist_id[i];
      nslist_pair[i].node_id            = node_id[i];
      nslist_pair[i].smooth_id          = smooth_id[i];
   }
   return nslist_pair;
}

} // END DISMOD_AT_NAMESPACE
