// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_DATA_SIM_TABLE_HPP
# define DISMOD_AT_GET_DATA_SIM_TABLE_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
   struct data_sim_struct {
      int         simulate_index;
      int         data_subset_id;
      double      data_sim_value;
   };
   extern CppAD::vector<data_sim_struct> get_data_sim_table(sqlite3*  db);
}

# endif
