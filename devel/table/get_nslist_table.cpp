// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_nslist_table dev}

C++: Get the nslist Table Information
#####################################

Syntax
******
*nslist_table* = ``get_nslist_table`` ( *db* )

Purpose
*******
To read the :ref:`nslist_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

nslist_table
************
The return value *nslist_table* has prototype

   ``CppAD::vector<std::string>`` *nslist_table*

For each :ref:`nslist_table@nslist_id` ,

   *nslist_table* [ *nslist_id* ]

is the name for the corresponding
:ref:`nslist_pair_table@nslist_id` .
{xrst_toc_hidden
   example/devel/table/get_nslist_table_xam.cpp
}
Example
*******
The file :ref:`get_nslist_table_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_nslist_table}
-----------------------------------------------------------------------------
*/



# include <dismod_at/get_nslist_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<std::string> get_nslist_table(sqlite3* db)
{  using std::string;

   string table_name  = "nslist";
# ifndef NDEBUG
   size_t n_nslist       = check_table_id(db, table_name);
# else
   check_table_id(db, table_name);
# endif

   string column_name = "nslist_name";
   CppAD::vector<std::string>  nslist;
   get_table_column(db, table_name, column_name, nslist);
   assert( nslist.size() == n_nslist );

   return nslist;
}

} // END DISMOD_AT_NAMESPACE
