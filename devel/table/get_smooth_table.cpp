// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_smooth_table dev}

C++: Get the Smoothing Table Information
########################################

Syntax
******
*smooth_table* = ``get_smooth_table`` ( *db* )

Purpose
*******
To read the :ref:`smooth_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

smooth_table
************
The return value *smooth_table* has prototype

   ``CppAD::vector<smooth_struct>`` *smooth_table*

For each :ref:`smooth_table@smooth_id` ,

   *smooth_table* [ *smooth_id* ]

is the information for the corresponding
:ref:`smooth_table@smooth_id` .

smooth_struct
*************
This is a structure with the following fields

.. list-table::
   :widths: auto

   * - Type
     - Field
     - Description
   * - ``std::string``
     - ``smooth_name``
     - The :ref:`smooth_table@smooth_name` for this smoothing.
   * - ``int``
     - ``n_age``
     - The :ref:`smooth_table@n_age` for this smoothing.
   * - ``int``
     - ``n_time``
     - The :ref:`smooth_table@n_time` for this smoothing.
   * - ``int``
     - ``mulstd_value_prior_id``
     - The :ref:`smooth_table@mulstd_value_prior_id`
       for this smoothing.
   * - ``int``
     - ``mulstd_dage_prior_id``
     - The :ref:`smooth_table@mulstd_dage_prior_id`
       for this smoothing.
   * - ``int``
     - ``mulstd_dtime_prior_id``
     - The :ref:`smooth_table@mulstd_dtime_prior_id`
       for this smoothing.

{xrst_comment
   example/devel/table/get_smooth_grid_xam.cpp is included by smooth_grid.omh
%}

Example
*******
The file :ref:`get_smooth_grid_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_smooth_table}
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
