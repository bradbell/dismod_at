// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_CHECK_PINI_N_AGE_HPP
# define DISMOD_AT_CHECK_PINI_N_AGE_HPP

# include <string>
# include <sqlite3.h>
# include "get_rate_table.hpp"
# include "get_smooth_table.hpp"

namespace dismod_at {
   void check_pini_n_age(
      sqlite3*                            db          ,
      const CppAD::vector<rate_struct>&   rate_table  ,
      const CppAD::vector<smooth_struct>& smooth_table
   );
}

# endif
