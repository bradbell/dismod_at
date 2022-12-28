// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_NODE_COV_TABLE_HPP
# define DISMOD_AT_GET_NODE_COV_TABLE_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {

   // BEGIN_NODE_COV_STRUCT
   struct node_cov_struct {
      int        covariate_id;
      int        node_id;
      double     split_value;
      int        weight_id;
   };
   // END_NODE_COV_STRUCT

   extern CppAD::vector<node_cov_struct> get_node_cov_table(
      sqlite3* db, size_t n_covariate, size_t n_node
   );
}

# endif
