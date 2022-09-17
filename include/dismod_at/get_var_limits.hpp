// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_VAR_LIMITS_HPP
# define DISMOD_AT_GET_VAR_LIMITS_HPP

# include <cppad/utility/vector.hpp>
# include "pack_info.hpp"
# include "pack_prior.hpp"
# include "get_prior_table.hpp"

namespace dismod_at {
   void get_var_limits(
      CppAD::vector<double>&               lower_limit  ,
      CppAD::vector<double>&               upper_limit  ,
       const pack_prior&                    var2prior    ,
      const CppAD::vector<prior_struct>&   prior_table
   );
}

# endif
