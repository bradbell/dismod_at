// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_AVGINT_TABLE_HPP
# define DISMOD_AT_GET_AVGINT_TABLE_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include "get_covariate_table.hpp"

namespace dismod_at {
   struct avgint_struct {
      int         integrand_id;
      int         node_id;
      int         subgroup_id;
      int         weight_id;
      double      age_lower;
      double      age_upper;
      double      time_lower;
      double      time_upper;
   };
   extern void get_avgint_table(
      sqlite3* db                                     ,
      size_t                           n_covariate    ,
      double                           age_min        ,
      double                           age_max        ,
      double                           time_min       ,
      double                           time_max       ,
      CppAD::vector<avgint_struct>&    avgint_table   ,
      CppAD::vector<double>&           avgint_cov_value
   );
}

# endif
