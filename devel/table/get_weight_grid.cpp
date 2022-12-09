// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_weight_grid dev}

C++: Get the Weight Grid Information
####################################

Syntax
******
*weight_grid* = ``get_weight_grid`` ( *db* )

Purpose
*******
To read the :ref:`weight_grid_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

weight_grid_struct
******************
This is a structure with the following fields

.. list-table::

   * - Type
     - Field
     - Description
   * - ``int``
     - ``weight_id``
     - The :ref:`weight_grid_table@weight_id` for this weighting.
   * - ``int``
     - ``age_id``
     - The :ref:`weight_grid_table@age_id` for this weight value
   * - ``int``
     - ``time_id``
     - The :ref:`weight_grid_table@time_id` for this weight value
   * - ``double``
     - ``weight``
     - The :ref:`weight_grid_table@weight` value

weight_grid
***********
The return value *weight_grid* has prototype

   ``CppAD::vector<weight_grid_struct>`` *weight_grid*

For each :ref:`weight_grid_table@weight_grid_id` ,

   *weight_grid* [ *weight_grid_id* ]

is the information for a specific weighting and specific age and time.
{xrst_toc_hidden
   example/devel/table/get_weight_grid_xam.cpp
}
Example
*******
The file :ref:`get_weight_grid_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_weight_grid}
-----------------------------------------------------------------------------
*/

# include <cmath>


# include <dismod_at/get_weight_grid.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<weight_grid_struct> get_weight_grid(sqlite3* db)
{  using std::string;

   string table_name  = "weight_grid";
   size_t n_grid      = check_table_id(db, table_name);

   string column_name =  "weight_id";
   CppAD::vector<int>     weight_id;
   get_table_column(db, table_name, column_name, weight_id);
   assert( weight_id.size() == n_grid );

   column_name        =  "age_id";
   CppAD::vector<int>     age_id;
   get_table_column(db, table_name, column_name, age_id);
   assert( age_id.size() == n_grid );

   column_name        =  "time_id";
   CppAD::vector<int>     time_id;
   get_table_column(db, table_name, column_name, time_id);
   assert( time_id.size() == n_grid );

   column_name        =  "weight";
   CppAD::vector<double>  weight;
   get_table_column(db, table_name, column_name, weight);
   assert( weight.size() == n_grid );

   CppAD::vector<weight_grid_struct> weight_grid(n_grid);
   for(size_t i = 0; i < n_grid; i++)
   {  weight_grid[i].weight_id = weight_id[i];
      weight_grid[i].age_id    = age_id[i];
      weight_grid[i].time_id   = time_id[i];
      weight_grid[i].weight    = weight[i];
      if( weight[i] <= 0.0 )
      {  string message = "weight is less than or equal zero.";
         error_exit(message, table_name, i);
      }
   }
   return weight_grid;
}

} // END DISMOD_AT_NAMESPACE
