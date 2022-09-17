// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_PRIOR_SIM_TABLE_HPP
# define DISMOD_AT_GET_PRIOR_SIM_TABLE_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
   struct prior_sim_struct {
      int         simulate_index;
      int         var_id;
      double      prior_sim_value;
      double      prior_sim_dage;
      double      prior_sim_dtime;
   };
   extern CppAD::vector<prior_sim_struct> get_prior_sim_table(sqlite3*  db);
}

# endif
