// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_DEPEND_HPP
# define DISMOD_AT_DEPEND_HPP

# include "data_model.hpp"
# include "prior_model.hpp"

namespace dismod_at {
   // data_depend
   CppAD::vector<bool> data_depend(
      const CppAD::vector<double>& pack_vec     ,
      data_model&                  data_object
   );
   // prior_depend
   CppAD::vector<bool> prior_depend(
      const CppAD::vector<double>& pack_vec     ,
      const prior_model&           prior_object
   );
}

# endif
