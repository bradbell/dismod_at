// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_nslist_pair dev}

C++: Get the List of Smoothing Table
####################################

Syntax
******
*nslist_pair* = ``get_nslist_pair_table`` ( *db* )

Purpose
*******
To read the :ref:`nslist_pair_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

nslist_pair
***********
The return value *nslist_pair* has prototype

   ``CppAD::vector<nslist_pair_struct>`` *nslist_pair*

For each :ref:`nslist_pair_table@nslist_pair_id` ,

   *nslist_pair_table* [ *nslist_pair_id* ]

is the information for one entry in one smoothing list.

nslist_pair_struct
******************
This is a structure with the following fields

.. list-table::

   * - ``int``
     - ``density_id``
     - :ref:`nslist_pair_table@nslist_id`
       identifies this list of smoothings.
   * - ``int``
     - ``node_id``
     - :ref:`node_table@node_id` is the node that this smoothing
       (in this list) applies to.
   * - ``int``
     - ``smooth_id``
     - :ref:`smooth_table@smooth_id` is the smoothing
       that is applied to this node (for this list).

{xrst_toc_hidden
   example/devel/table/get_nslist_pair_xam.cpp
}
Example
*******
The file :ref:`get_nslist_pair_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_nslist_pair}
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
