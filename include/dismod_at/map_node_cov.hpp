// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_MAP_NODE_COV_HPP
# define DISMOD_AT_MAP_NODE_COV_HPP

# include <cppad/utility/vector.hpp>
# include <dismod_at/get_node_cov_table.hpp>

namespace dismod_at {

   extern CppAD::vector< CppAD::vector<size_t> > map_node_cov(
      const CppAD::vector<node_cov_struct>&  node_cov_table  ,
      size_t                                 n_covariate     ,
      size_t                                 n_node
   );

}

# endif
