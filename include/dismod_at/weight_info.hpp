// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_WEIGHT_INFO_HPP
# define DISMOD_AT_WEIGHT_INFO_HPP

# include <cppad/utility/vector.hpp>
# include "get_weight_table.hpp"
# include "get_weight_grid.hpp"

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

class weight_info {
private:
   // grid of age values for this weighting
   CppAD::vector<size_t> age_id_;
   // grid of time values for this weighting
   CppAD::vector<size_t> time_id_;
   // vector of weights for each age, time pair
   CppAD::vector<double> weight_;
public:
   // assignment operator
   void operator=(const weight_info& w_info);
   // default constructor
   weight_info(void);
   // testing constructor
   weight_info(
      const CppAD::vector<double>& age_table    ,
      const CppAD::vector<double>& time_table   ,
      const CppAD::vector<size_t>& age_id       ,
      const CppAD::vector<size_t>& time_id      ,
      const CppAD::vector<double>& weight
   );
   // normal  constructor
   weight_info(
      const CppAD::vector<double>&             age_table    ,
      const CppAD::vector<double>&             time_table   ,
      size_t                                   weight_id    ,
      const CppAD::vector<weight_struct>&      weight_table ,
      const CppAD::vector<weight_grid_struct>& weight_grid_table
   );
   //
   size_t age_size(void) const;
   size_t time_size(void) const;
   //
   size_t age_id(size_t i) const;
   size_t time_id(size_t j) const;
   //
   double weight(size_t i, size_t j) const;
};

} // END_DISMOD_AT_NAMESPACE

# endif
