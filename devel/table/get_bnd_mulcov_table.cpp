// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_bnd_mulcov_table dev}

C++: Get the Data Subset Table
##############################

Syntax
******
*bnd_mulcov_table_table* = ``get_bnd_mulcov_table`` ( *db* )

Prototype
*********
{xrst_literal
   // BEGIN_PROTOTYPE
   // END_PROTOTYPE
}

Purpose
*******
To read the :ref:`bnd_mulcov_table-name` and return it as a C++ data structure.

db
**
The argument *db* has prototype

   ``sqlite3`` * *db*

and is an open connection to the database.

bnd_mulcov_table
****************
For each :ref:`bnd_mulcov_table@bnd_mulcov_id` ,

   *bnd_mulcov_table* [ *bnd_mulcov_id* ]

is the information for the corresponding
*bnd_mulcov_id* .

bnd_mulcov_struct
*****************
This is a structure with the following fields

.. list-table::
   :widths: auto

   * - Type
     - Field
     - Description
   * - ``double``
     - ``max_cov_diff``
     - The :ref:`bnd_mulcov_table@max_cov_diff`
   * - ``double``
     - ``max_mulcov``
     - The :ref:`bnd_mulcov_table@max_mulcov`

{xrst_toc_hidden
   example/devel/table/get_bnd_mulcov_table_xam.cpp
}
Example
*******
The file :ref:`get_bnd_mulcov_table_xam.cpp-name` contains an example
and test of this function.

{xrst_end get_bnd_mulcov_table}
-----------------------------------------------------------------------------
*/

# include <dismod_at/get_bnd_mulcov_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
CppAD::vector<bnd_mulcov_struct> get_bnd_mulcov_table(sqlite3* db)
// END_PROTOTYPE
{  using std::string;

   string table_name     = "bnd_mulcov";
   size_t n_bnd_mulcov   = check_table_id(db, table_name);

   string column_name    =   "max_cov_diff";
   CppAD::vector<double>       max_cov_diff;
   get_table_column(db, table_name, column_name, max_cov_diff);
   assert( max_cov_diff.size() == n_bnd_mulcov );

   column_name           =   "max_mulcov";
   CppAD::vector<double>       max_mulcov;
   get_table_column(db, table_name, column_name, max_mulcov);
   assert( max_mulcov.size() == n_bnd_mulcov );

   CppAD::vector<bnd_mulcov_struct> bnd_mulcov_table(n_bnd_mulcov);
   for(size_t i = 0; i < n_bnd_mulcov; i++)
   {
      bnd_mulcov_table[i].max_cov_diff = max_cov_diff[i];
      bnd_mulcov_table[i].max_mulcov   = max_mulcov[i];
   }
   return bnd_mulcov_table;
}

} // END DISMOD_AT_NAMESPACE
