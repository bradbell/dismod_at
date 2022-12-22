// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin map_node_cov dev}

Get Mapping From (covariate_id, node_id) to weight_id
#####################################################

Syntax
******
{xrst_literal
   // BEGIN_PROTOTYPE
   // END_PROTOTYPE
}

node_cov_table
**************
This is the contents of the node_cov table as returned by
:ref:`get_node_cov_table-name` .

n_covariate, n_node
*******************
is the number of covariates (nodes) in the covariate table (node table).


node_cov_map
************
We use *node_cov_map* to denote the return value.
For *covariate_id* equal 0 through *n_covariate_id* - 1,

#. if *covariate_id* is not in node_cov table,
   *node_cov_map* [ *covariate_id* ].size() == 0

#. if *covariate_id* is in node_cov table,
   *node_cov_map* [ *covariate_id* ].size() == *n_node*

#. if *covariate_id* is in node_cov table, and
   *node_id* is between 0 and *n_node* - 1, and

      *weight_id* = *node_cov_map* [ *covariate_id* ][*node_id*]

   then there is a row in the node_cov table with the specified
   *covariate_id*, *node_id* and *weight_id* .

{xrst_end map_node_cov}
*/
# include <dismod_at/map_node_cov.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
CppAD::vector< CppAD::vector<size_t> >    map_node_cov(
   const CppAD::vector<node_cov_struct>&  node_cov_table  ,
   size_t                                 n_covariate     ,
   size_t                                 n_node          )
// END_PROTOTYPE
{  // n_node_cov
   size_t n_node_cov = node_cov_table.size();
   //
   // node_cov_map
   CppAD::vector< CppAD::vector<size_t> > node_cov_map(n_covariate);
   //
   // node_cov_id
   for(size_t node_cov_id = 0; node_cov_id < n_node_cov; ++node_cov_id)
   {  //
      // covariate_id, node_id, weight_id
      size_t covariate_id = size_t( node_cov_table[node_cov_id].covariate_id );
      size_t node_id      = size_t( node_cov_table[node_cov_id].node_id );
      size_t weight_id    = size_t( node_cov_table[node_cov_id].weight_id );
      //
      // node_cov_map[covariate_id]
      if( node_cov_map[covariate_id].size() == 0 )
         node_cov_map[covariate_id].resize(n_node);
      node_cov_map[covariate_id][node_id] = weight_id;
   }
   return node_cov_map;
}

} // END DISMOD_AT_NAMESPACE
