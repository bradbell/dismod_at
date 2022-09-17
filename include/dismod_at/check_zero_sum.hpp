// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_CHECK_ZERO_SUM_HPP
# define DISMOD_AT_CHECK_ZERO_SUM_HPP

# include <string>
# include <sqlite3.h>
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/get_option_table.hpp>

namespace dismod_at {
   void check_zero_sum(
      sqlite3*                                  db            ,
      const CppAD::vector<rate_struct>&         rate_table    ,
      const CppAD::vector<option_struct>&       option_table
   );
}

# endif
