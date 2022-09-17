// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_PACK_WARM_START_HPP
# define DISMOD_AT_PACK_WARM_START_HPP

# include <cppad/mixed/warm_start_struct.hpp>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
   CppAD::vector<double> pack_warm_start(
      const CppAD::mixed::warm_start_struct& warm_start
   );
   CppAD::mixed::warm_start_struct unpack_warm_start(
      const CppAD::vector<double>& vec
   );
}

# endif
