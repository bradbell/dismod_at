// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_SMOOTH_TABLE_HPP
# define DISMOD_AT_GET_SMOOTH_TABLE_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
   struct smooth_struct {
      std::string smooth_name;
      int         n_age;
      int         n_time;
      int         mulstd_value_prior_id;
      int         mulstd_dage_prior_id;
      int         mulstd_dtime_prior_id;
   };
   extern CppAD::vector<smooth_struct> get_smooth_table(sqlite3*  db);
}

# endif
