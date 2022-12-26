// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_data_sim_table dev}

C++: Get the Data Simulation Table
##################################

Syntax
******
*data_sim_table* = ``get_data_sim_table`` ( *db* )

Purpose
*******
To read the :ref:`data_sim_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

data_sim_table
**************
The return value *data_sim_table* has prototype

   ``CppAD::vector<data_sim_struct>`` *data_sim_table*

For each :ref:`data_sim_table@data_sim_id` ,

   *data_sim_table* [ *data_sim_id* ]

is the information for the corresponding
:ref:`data_sim_table@data_sim_id` .

data_sim_struct
***************
This is a structure with the following fields

.. list-table::
   :widths: auto

   * - Type
     - Field
     - Description
   * - ``int``
     - ``simulate_index``
     - The :ref:`data_sim_table@simulate_index`
       for this simulated measurement.
   * - ``int``
     - ``data_subset_id``
     - The :ref:`data_subset_table@data_subset_id`
       for this simulated measurement.
   * - ``double``
     - ``data_sim_value``
     - The :ref:`data_table@meas_value`
       for this simulated measurement.

{xrst_toc_hidden
   example/devel/table/get_data_sim_table_xam.cpp
}
Example
*******
The file :ref:`get_data_sim_table_xam.cpp-name` contains an example
and test of this function.

{xrst_end get_data_sim_table}
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_data_sim_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<data_sim_struct> get_data_sim_table(sqlite3* db)
{  using std::string;

   string table_name  = "data_sim";
   size_t n_data_sim  = check_table_id(db, table_name);

   std::string column_name =  "simulate_index";
   CppAD::vector<int>          simulate_index;
   get_table_column(db, table_name, column_name, simulate_index);
   assert( simulate_index.size() == n_data_sim );

   column_name             =  "data_subset_id";
   CppAD::vector<int>          data_subset_id;
   get_table_column(db, table_name, column_name, data_subset_id);
   assert( data_subset_id.size() == n_data_sim );

   column_name             =  "data_sim_value";
   CppAD::vector<double>       data_sim_value;
   get_table_column(db, table_name, column_name, data_sim_value);
   assert( data_sim_value.size() == n_data_sim );

   CppAD::vector<data_sim_struct> data_sim_table(n_data_sim);
   for(size_t i = 0; i < n_data_sim; i++)
   {  data_sim_table[i].simulate_index   = simulate_index[i];
      data_sim_table[i].data_subset_id   = data_subset_id[i];
      data_sim_table[i].data_sim_value   = data_sim_value[i];
   }
   return data_sim_table;
}

} // END DISMOD_AT_NAMESPACE
