// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_CHECK_RATE_EFF_COV_HPP
# define DISMOD_AT_CHECK_RATE_EFF_COV_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include <dismod_at/get_covariate_table.hpp>
# include <dismod_at/get_node_table.hpp>
# include <dismod_at/get_rate_eff_cov_table.hpp>
# include <dismod_at/get_option_table.hpp>

namespace dismod_at {
   void check_rate_eff_cov(
      const CppAD::vector<double>&                data_cov_value     ,
      const CppAD::vector<double>&                avgint_cov_value   ,
      const CppAD::vector<covariate_struct>&      covariate_table    ,
      const CppAD::vector<node_struct>&           node_table         ,
      const CppAD::vector<rate_eff_cov_struct>&   rate_eff_cov_table ,
      const CppAD::vector<option_struct>&         option_table
   );
}

# endif
