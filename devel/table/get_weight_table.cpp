// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_weight_table dev}

C++: Get the Weight Table Information
#####################################

Syntax
******
*weight_table* = ``get_weight_table`` ( *db* )

Purpose
*******
To read the :ref:`weight_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

weight_table
************
The return value *weight_table* has prototype

   ``CppAD::vector<weight_struct>`` *weight_table*

For each :ref:`weight_table@weight_id` ,

   *weight_table* [ *weight_id* ]

is the information for the corresponding
:ref:`weight_table@weight_id` .

weight_struct
*************
This is a structure with the following fields

.. list-table::
   :widths: auto

   * - Type
     - Field
     - Description
   * - ``std::string``
     - ``weight_name``
     - The :ref:`weight_table@weight_name` for this weighting.
   * - ``int``
     - ``n_age``
     - The :ref:`weight_table@n_age` for this weighting.
   * - ``int``
     - ``n_time``
     - The :ref:`weight_table@n_time` for this weighting.

{xrst_comment
   example/devel/table/get_weight_grid_xam.cpp is included by weight_grid.omh
%}
Example
*******
The file :ref:`get_weight_grid_xam.cpp-name` contains an example that uses
this function.

{xrst_end get_weight_table}
-----------------------------------------------------------------------------
*/
# include <dismod_at/get_weight_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<weight_struct> get_weight_table(sqlite3* db)
{  using std::string;

   string table_name  = "weight";
   size_t n_weight    = check_table_id(db, table_name);

   string column_name =  "weight_name";
   CppAD::vector<string>  weight_name;
   get_table_column(db, table_name, column_name, weight_name);
   assert( weight_name.size() == n_weight );

   column_name        =  "n_age";
   CppAD::vector<int>     n_age;
   get_table_column(db, table_name, column_name, n_age);
   assert( n_age.size() == n_weight );

   column_name        =  "n_time";
   CppAD::vector<int>     n_time;
   get_table_column(db, table_name, column_name, n_time);
   assert( n_time.size() == n_weight );

   CppAD::vector<weight_struct> weight_table(n_weight);
   for(size_t i = 0; i < n_weight; i++)
   {  weight_table[i].weight_name   = weight_name[i];
      weight_table[i].n_age         = n_age[i];
      weight_table[i].n_time        = n_time[i];
   }
   return weight_table;
}

} // END DISMOD_AT_NAMESPACE
