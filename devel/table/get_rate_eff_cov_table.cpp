// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin get_rate_eff_cov_table dev}
{xrst_spell
   hpp
}

C++: Get the Node Covariate Table Information
#############################################

Syntax
******
| ``# include <dismod_at/get_rate_eff_cov_table.hpp>``
| *rate_eff_cov_table* = ``get_rate_eff_cov_table`` ( *db* , *n_covariate, *n_node* )


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

rate_eff_cov_struct
*******************
{xrst_literal
   include/dismod_at/get_rate_eff_cov_table.hpp
   // BEGIN_RATE_EFF_COV_STRUCT
   // END_RATE_EFF_COV_STRUCT
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


rate_eff_cov_table
******************
If the rate_eff_cov table does not exist, the empty vector is returned.
Otherwise,
the *i*-th element of the return value,  *rate_eff_cov_table* [ *i* ],
contains the information in the row of rate_eff_cov table  with
:ref:`rate_eff_cov_table@rate_eff_cov_id` equal to *i*.

{xrst_end get_rate_eff_cov_table}
*/

# include <set>
# include <cppad/utility/to_string.hpp>
# include <dismod_at/get_rate_eff_cov_table.hpp>
# include <dismod_at/get_table_column.hpp>
# include <dismod_at/check_table_id.hpp>
# include <dismod_at/does_table_exist.hpp>
# include <dismod_at/error_exit.hpp>


namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
CppAD::vector<rate_eff_cov_struct> get_rate_eff_cov_table(
   sqlite3* db, size_t n_covariate, size_t n_node
)
// END_PROTOTYPE
{  //
   // table_name
   std::string table_name  = "rate_eff_cov";
   //
   // rate_eff_cov_table
   CppAD::vector<rate_eff_cov_struct> rate_eff_cov_table(0);
   if( ! does_table_exist(db, table_name) )
      return rate_eff_cov_table;
   //
   // n_rate_eff_cov
   size_t n_rate_eff_cov = check_table_id(db, table_name);
   //
   // covariate_id
   std::string column_name = "covariate_id";
   CppAD::vector<int>    covariate_id_vec;
   get_table_column(db, table_name, column_name, covariate_id_vec);
   assert( n_rate_eff_cov == covariate_id_vec.size() );
   //
   // node_id
   column_name = "node_id";
   CppAD::vector<int>    node_id_vec;
   get_table_column(db, table_name, column_name, node_id_vec);
   assert( n_rate_eff_cov == node_id_vec.size() );
   //
   // split_value
   column_name = "split_value";
   CppAD::vector<double> split_value_vec;
   get_table_column(db, table_name, column_name, split_value_vec);
   assert( n_rate_eff_cov == split_value_vec.size() );
   //
   // weight_id
   column_name = "weight_id";
   CppAD::vector<int>    weight_id_vec;
   get_table_column(db, table_name, column_name, weight_id_vec);
   assert( n_rate_eff_cov == weight_id_vec.size() );
   //
   //
   // rate_eff_cov_table
   rate_eff_cov_table.resize(n_rate_eff_cov);
   for(size_t i = 0; i < n_rate_eff_cov; ++i)
   {
      rate_eff_cov_table[i].covariate_id = covariate_id_vec[i];
      rate_eff_cov_table[i].node_id      = node_id_vec[i];
      rate_eff_cov_table[i].split_value  = split_value_vec[i];
      rate_eff_cov_table[i].weight_id    = weight_id_vec[i];
   }
   // The primary key values covariate_id and node_id get checked by
   // the calling routine. Other constraints are checked by check_rate_eff_cov.
   //
   return rate_eff_cov_table;
}

} // END_DISMOD_AT_NAMESPACE
