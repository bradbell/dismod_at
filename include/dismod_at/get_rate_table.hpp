// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_RATE_TABLE_HPP
# define DISMOD_AT_GET_RATE_TABLE_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
   enum rate_enum {
      pini_enum,
      iota_enum,
      rho_enum,
      chi_enum,
      omega_enum,
      number_rate_enum
   };
   struct rate_struct {
      rate_enum  rate;
      int        parent_smooth_id;
      int        child_smooth_id;
      int        child_nslist_id;
   };
   extern CppAD::vector<rate_struct> get_rate_table(sqlite3*  db);
   extern std::string get_rate_name(size_t rate_id);
}

# endif
