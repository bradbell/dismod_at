// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_BND_MULCOV_TABLE_HPP
# define DISMOD_AT_GET_BND_MULCOV_TABLE_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
   struct bnd_mulcov_struct {
      double max_cov_diff;
      double max_mulcov;
   };
   extern CppAD::vector<bnd_mulcov_struct> get_bnd_mulcov_table(sqlite3* db);
}

# endif
