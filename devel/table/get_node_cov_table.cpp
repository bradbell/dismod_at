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
| *node_cov_table* = ``get_node_cov_table`` ( *db* , *n_covariate, *n_node* )


Prototype
*********
{xrst_literal
   // BEGIN_PROTOTYPE
   // END_PROTOTYPE
}

db
**
This argument is an open connection to the database

n_covariate, n_node
*******************
This is the number of covariates (nodes)
in the covariate table (node table).
This is used to check that for each *covariate_id* in this table,
there is a row with every *node_id* between zero and *n_node* - 1.

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

# include <set>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/get_node_cov_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/does_table_exist.hpp>
# include <dismod_at/error_exit.hpp>


namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
CppAD::vector<node_cov_struct> get_node_cov_table(
   sqlite3* db, size_t n_covariate, size_t n_node
)
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
   // covariate_id
   std::string column_name = "covariate_id";
   CppAD::vector<int>    covariate_id_vec;
   get_table_column(db, table_name, column_name, covariate_id_vec);
   assert( n_node_cov == covariate_id_vec.size() );
   //
   // node_id
   column_name = "node_id";
   CppAD::vector<int>    node_id_vec;
   get_table_column(db, table_name, column_name, node_id_vec);
   assert( n_node_cov == node_id_vec.size() );
   //
   // split_value
   column_name = "split_value";
   CppAD::vector<double> split_value_vec;
   get_table_column(db, table_name, column_name, split_value_vec);
   assert( n_node_cov == split_value_vec.size() );
   //
   // weight_id
   column_name = "weight_id";
   CppAD::vector<int>    weight_id_vec;
   get_table_column(db, table_name, column_name, weight_id_vec);
   assert( n_node_cov == weight_id_vec.size() );
   //
   //
   // node_cov_table
   node_cov_table.resize(n_node_cov);
   for(size_t i = 0; i < n_node_cov; ++i)
   {
      node_cov_table[i].covariate_id = covariate_id_vec[i];
      node_cov_table[i].node_id      = node_id_vec[i];
      node_cov_table[i].split_value  = split_value_vec[i];
      node_cov_table[i].weight_id    = weight_id_vec[i];
   }
   // The primary key values covariate_id and node_id get checked by
   // the calling routine.
   //
   // node_covariate_id
   std::set<int> node_covariate_id;
   for(size_t i = 0; i < n_node_cov; ++i)
      node_covariate_id.insert( covariate_id_vec[i] );

   // msg
   std::string msg;

   // covariate_id
   std::set<int>::const_iterator itr;
   for(itr = node_covariate_id.begin(); itr != node_covariate_id.end(); ++itr)
   {  int covariate_id = *itr;

      // found_node_id
      CppAD::vector<bool> found_node_id(n_node);
      for(size_t i = 0; i < n_node; ++i)
         found_node_id[i] = false;
      //
      // found_node_id
      for(size_t i = 0; i < n_node_cov; ++i)
      {  if( covariate_id_vec[i] == covariate_id )
         {  int node_id = node_id_vec[i];
            if( node_id < 0 || n_node <= size_t(node_id) )
            {  msg  = "node_id " + CppAD::to_string( node_id );
               msg += " is not a valid node table node_id";
               size_t node_cov_id = i;
               error_exit(msg, table_name, node_cov_id);
            }
            if( found_node_id[node_id] )
            {  msg  = "node_id " + CppAD::to_string( node_id );
               msg += " appears more than one with covariae_id";
               msg += CppAD::to_string( covariate_id );
               size_t node_cov_id = i;
               error_exit(msg, table_name, node_cov_id);
            }
            found_node_id[node_id] = true;
         }
      }
      for(size_t node_id = 0; node_id < n_node; ++node_id)
      {  if( ! found_node_id[node_id] )
         {  msg  = "covariate_id " + CppAD::to_string(covariate_id);
            msg += " appread in this table but not with node_id ";
            msg += CppAD::to_string(node_id);
            error_exit(msg, table_name);
         }
      }
   }
   //
   return node_cov_table;
}

} // END_DISMOD_AT_NAMESPACE
