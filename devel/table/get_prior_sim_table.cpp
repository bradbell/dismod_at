// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_prior_sim_table dev}

C++: Get the Prior Simulation Table
###################################

Syntax
******
*prior_sim_table* = ``get_prior_sim_table`` ( *db* )

Purpose
*******
To read the :ref:`prior_sim_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

prior_sim_table
***************
The return value *prior_sim_table* has prototype

   ``CppAD::vector<prior_sim_struct>`` *prior_sim_table*

For each :ref:`prior_sim_table@prior_sim_id` ,

   *prior_sim_table* [ *prior_sim_id* ]

is the information for the corresponding
:ref:`prior_sim_table@prior_sim_id` .

prior_sim_struct
****************
This is a structure with the following fields

.. list-table::
   :widths: auto

   * - Type
     - Field
     - Description
   * - ``int``
     - ``simulate_index``
     - The :ref:`prior_sim_table@simulate_index`
       for this prior simulation.
   * - ``int``
     - ``var_id``
     - The value :ref:`var_table@var_id`
       identifies the variable for this prior simulation.
   * - ``double``
     - ``prior_sim_value``
     - This is the variable value for this prior simulation.
   * - ``double``
     - ``prior_sim_dage``
     - This is the age difference value for this variables prior simulation.
   * - ``double``
     - ``prior_sim_dtime``
     - This is the time difference value for this variables prior simulation.

{xrst_toc_hidden
   example/devel/table/get_prior_sim_table_xam.cpp
}
Example
*******
The file :ref:`get_prior_sim_table_xam.cpp-name` contains an example
and test of this function.

{xrst_end get_prior_sim_table}
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_prior_sim_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

CppAD::vector<prior_sim_struct> get_prior_sim_table(sqlite3* db)
{  using std::string;

   string table_name  = "prior_sim";
   size_t n_prior_sim = check_table_id(db, table_name);

   std::string column_name =  "simulate_index";
   CppAD::vector<int>          simulate_index;
   get_table_column(db, table_name, column_name, simulate_index);
   assert( simulate_index.size() == n_prior_sim );

   column_name             =  "var_id";
   CppAD::vector<int>          var_id;
   get_table_column(db, table_name, column_name, var_id);
   assert( var_id.size() == n_prior_sim );

   column_name             =  "prior_sim_value";
   CppAD::vector<double>       prior_sim_value;
   get_table_column(db, table_name, column_name, prior_sim_value);
   assert( prior_sim_value.size() == n_prior_sim );

   column_name             =  "prior_sim_dage";
   CppAD::vector<double>       prior_sim_dage;
   get_table_column(db, table_name, column_name, prior_sim_dage);
   assert( prior_sim_dage.size() == n_prior_sim );

   column_name             =  "prior_sim_dtime";
   CppAD::vector<double>       prior_sim_dtime;
   get_table_column(db, table_name, column_name, prior_sim_dtime);
   assert( prior_sim_dtime.size() == n_prior_sim );

   CppAD::vector<prior_sim_struct> prior_sim_table(n_prior_sim);
   for(size_t i = 0; i < n_prior_sim; i++)
   {  prior_sim_table[i].simulate_index   = simulate_index[i];
      prior_sim_table[i].var_id           = var_id[i];
      prior_sim_table[i].prior_sim_value  = prior_sim_value[i];
      prior_sim_table[i].prior_sim_dage   = prior_sim_dage[i];
      prior_sim_table[i].prior_sim_dtime  = prior_sim_dtime[i];
   }
   return prior_sim_table;
}

} // END DISMOD_AT_NAMESPACE
