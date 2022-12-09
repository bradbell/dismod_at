// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_sample_table dev}

C++: Get the Simulate Table
###########################

Syntax
******
*sample_table* = ``get_sample_table`` ( *db* )

Purpose
*******
To read the :ref:`sample_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

sample_table
************
The return value *sample_table* has prototype

   ``CppAD::vector<sample_struct>`` *sample_table*

For each :ref:`sample_table@sample_id` ,

   *sample_table* [ *sample_id* ]

is the information for the corresponding
:ref:`sample_table@sample_id` .

sample_struct
*************
This is a structure with the following fields

.. list-table::

   * - Type
     - Field
     - Description
   * - ``int``
     - ``sample_index``
     - The :ref:`sample_table@sample_index`
       for this sampled measurement.
   * - ``int``
     - ``var_id``
     - The :ref:`sample_table@var_id`
       for this sample
   * - ``double``
     - ``var_value``
     - The :ref:`sample_table@var_value`
       for this sample.

{xrst_toc_hidden
   example/devel/table/get_sample_table_xam.cpp
}
Example
*******
The file :ref:`get_sample_table_xam.cpp-name` contains an example
and test of this function.

{xrst_end get_sample_table}
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_sample_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<sample_struct> get_sample_table(sqlite3* db)
{  using std::string;

   string table_name  = "sample";
   size_t n_sample = check_table_id(db, table_name);

   std::string column_name =  "sample_index";
   CppAD::vector<int>          sample_index;
   get_table_column(db, table_name, column_name, sample_index);
   assert( sample_index.size() == n_sample );

   column_name             =  "var_id";
   CppAD::vector<int>          var_id;
   get_table_column(db, table_name, column_name, var_id);
   assert( var_id.size() == n_sample );

   column_name             =  "var_value";
   CppAD::vector<double>       var_value;
   get_table_column(db, table_name, column_name, var_value);
   assert( var_value.size() == n_sample );

   CppAD::vector<sample_struct> sample_table(n_sample);
   for(size_t i = 0; i < n_sample; i++)
   {  sample_table[i].sample_index   = sample_index[i];
      sample_table[i].var_id         = var_id[i];
      sample_table[i].var_value      = var_value[i];
   }
   return sample_table;
}

} // END DISMOD_AT_NAMESPACE
