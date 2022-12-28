// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_COV2WEIGHT_MAP_HPP
# define DISMOD_AT_COV2WEIGHT_MAP_HPP

# include <cppad/utility/vector.hpp>
# include "get_covariate_table.hpp"
# include "get_node_cov_table.hpp"

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class cov2weight_map {
   // -----------------------------------------------------------------------
   // BEGIN_CTOR_OUTPUTS
private:
   const size_t           n_covariate_;
   const size_t           n_node_;
   const size_t           n_weight_;
   //
   size_t                 split_covariate_id_;
   double                 split_covariate_reference_;
   CppAD::vector<double>  split_value_vec_;
   CppAD::vector<size_t>  weight_id_vec_;
   // END_CTOR_OUTPUTS
public:
   //
   // constructor
   cov2weight_map(
      size_t                                      n_node              ,
      size_t                                      n_weight            ,
      const std::string&                          splitting_covariate ,
      const CppAD::vector<covariate_struct>&      covariate_table     ,
      const CppAD::vector<node_cov_struct>&       node_cov_table
   );
   //
   // weight_id
   size_t weight_id(
      size_t                       covariate_id ,
      size_t                       node_id       ,
      const CppAD::vector<double>& x
   ) const;
};

} // END_DISMOD_AT_NAMESPACE

# endif
