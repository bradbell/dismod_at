// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_CHILD_DATA_IN_FIT_HPP
# define DISMOD_AT_CHILD_DATA_IN_FIT_HPP

# include <string>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
   CppAD::vector<size_t> child_data_in_fit(
      const std::map<std::string, std::string>&    option_map            ,
      const CppAD::vector<data_subset_struct>&     data_subset_table     ,
      const CppAD::vector<integrand_struct>&       integrand_table       ,
      const CppAD::vector<data_struct>&            data_table            ,
      const child_info&                            child_info4data
   );
}

# endif
