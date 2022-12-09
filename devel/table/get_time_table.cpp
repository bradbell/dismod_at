// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_time_table dev}

C++: Get the Time Table Information
###################################

Syntax
******
*time_table* = ``get_time_table`` ( *db* )

Purpose
*******
To read the :ref:`time_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

time_table
**********
The return value *time_table* has prototype

   ``CppAD::vector<double>`` *time_table*

For each :ref:`time_table@time_id` ,

   *time_table* [ *time_id* ]

is the time for the corresponding
:ref:`time_table@time` .
{xrst_toc_hidden
   example/devel/table/get_time_table_xam.cpp
}
Example
*******
The file :ref:`get_time_table_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_time_table}
-----------------------------------------------------------------------------
*/
# include <dismod_at/get_time_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<double> get_time_table(sqlite3* db)
{  using std::string;

   string table_name  = "time";
   string column_name = "time";
   CppAD::vector<double>  time;
   get_table_column(db, table_name, column_name, time);

# ifndef NDEBUG
   size_t n_time      = check_table_id(db, table_name);
   assert( time.size() == n_time );
# endif

   return time;
}

} // END DISMOD_AT_NAMESPACE
