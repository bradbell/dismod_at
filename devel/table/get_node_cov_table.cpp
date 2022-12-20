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
| # include <dismod_at/get_node_cov_table.hpp>
| *node_cov_table* = get_node_cov_table( *db* , *n_covariate* , *n_node* )


Prototype
*********
{xrst_literal
   // BEGIN_PROTOTYPE
   // END_PROTOTYPE
}

db
**
This argument is an open connection to the database

n_covariate
***********
is the number of covariates in the :ref:`covariate_table-name` .

n_node
******
is the number of nodes in the :ref:`node_table-name` .

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

age
===
The age corresponding to this row.

time
====
The time corresponding to this row.

cov_value
=========
The covariate value corresponding to this row.


{xrst_end get_node_cov_table}
*/

# include <dismod_at/get_node_cov_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>


namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
CppAD::vector<node_cov_struct> get_node_cov_table(
   sqlite3* db          ,
   int      n_covariate ,
   int      n_node      )
// END_PROTOTYPE
{
   // n_node_cov
   std::string table_name  = "node_cov";
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
   // age
   column_name           =  "age";
   CppAD::vector<double>     age;
   get_table_column(db, table_name, column_name, age);
   assert( n_node_cov == age.size() );
   //
   // time
   column_name           =  "time";
   CppAD::vector<double>     time;
   get_table_column(db, table_name, column_name, time);
   assert( n_node_cov == time.size() );
   //
   // cov_value
   column_name           =  "cov_value";
   CppAD::vector<double>     cov_value;
   get_table_column(db, table_name, column_name, cov_value);
   assert( n_node_cov == cov_value.size() );
   //
   // node_cov_table
   CppAD::vector<node_cov_struct> node_cov_table(n_node_cov);
   for(size_t i = 0; i < n_node_cov; ++i)
   {
      node_cov_table[i].covariate_id = covariate_id[i];
      node_cov_table[i].node_id      = node_id[i];
      node_cov_table[i].age          = age[i];
      node_cov_table[i].time         = time[i];
      node_cov_table[i].cov_value    = cov_value[i];
   }

   // The primary key values covariate_id and node_id get checked by
   // the calling routine.
   //
   return node_cov_table;
}

} // END_DISMOD_AT_NAMESPACE
