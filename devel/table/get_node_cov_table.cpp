// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_node_cov_table dev}
{xrst_spell
   hpp
}

C++: Get the Node Covariate Table Information
#############################################

Syntax
******
| ``# include <dismod_at/get_node_cov_table.hpp>``
| *node_cov_table* = ``get_node_cov_table`` ( *db* )


Prototype
*********
{xrst_literal
   // BEGIN_PROTOTYPE
   // END_PROTOTYPE
}

db
**
This argument is an open connection to the database

node_cov_struct
***************
{xrst_literal
   include/dismod_at/get_node_cov_table.hpp
   // BEGIN_NODE_COV_STRUCT
   // END_NODE_COV_STRUCT
}

covariate_id
============
The :ref:`covariate_table@covariate_id` corresponding to this row.

node_id
=======
The :ref:`node_table@node_id` corresponding to this row corresponds to.

weight_id
=========
The :ref:`weight_table@weight_id` corresponding to this row corresponds to.


node_cov_table
**************
If the node_cov table does not exist, the empty vector is returned.
Otherwise,
the *i*-th element of the return value,  *node_cov_table* [ *i* ],
contains the information in the row of node_cov table  with
:ref:`node_cov_table@node_cov_id` equal to *i*.

{xrst_end get_node_cov_table}
*/

# include <dismod_at/get_node_cov_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/does_table_exist.hpp>


namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
CppAD::vector<node_cov_struct> get_node_cov_table( sqlite3* db )
// END_PROTOTYPE
{  //
   // table_name
   std::string table_name  = "node_cov";
   //
   // node_cov_table
   CppAD::vector<node_cov_struct> node_cov_table(0);
   if( ! does_table_exist(db, table_name) )
      return node_cov_table;
   //
   // n_node_cov
   size_t n_node_cov = check_table_id(db, table_name);
   //
   // covaraite_id
   std::string column_name = "covariate_id";
   CppAD::vector<int>    covariate_id;
   get_table_column(db, table_name, column_name, covariate_id);
   assert( n_node_cov == covariate_id.size() );
   //
   // node_id
   column_name = "node_id";
   CppAD::vector<int>    node_id;
   get_table_column(db, table_name, column_name, node_id);
   assert( n_node_cov == node_id.size() );
   //
   // weight_id
   column_name = "weight_id";
   CppAD::vector<int>    weight_id;
   get_table_column(db, table_name, column_name, weight_id);
   assert( n_node_cov == weight_id.size() );
   //
   //
   // node_cov_table
   node_cov_table.resize(n_node_cov);
   for(size_t i = 0; i < n_node_cov; ++i)
   {
      node_cov_table[i].covariate_id = covariate_id[i];
      node_cov_table[i].node_id      = node_id[i];
      node_cov_table[i].weight_id    = weight_id[i];
   }

   // The primary key values covariate_id and node_id get checked by
   // the calling routine.
   //
   return node_cov_table;
}

} // END_DISMOD_AT_NAMESPACE
