// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_WEIGHT_GRID_HPP
# define DISMOD_AT_GET_WEIGHT_GRID_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
   struct weight_grid_struct {
      int    weight_id;
      int    age_id;
      int    time_id;
      double weight;
   };
   extern CppAD::vector<weight_grid_struct> get_weight_grid(sqlite3*  db);
}

# endif
