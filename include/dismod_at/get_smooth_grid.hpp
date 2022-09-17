// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_SMOOTH_GRID_HPP
# define DISMOD_AT_GET_SMOOTH_GRID_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include <dismod_at/get_density_table.hpp>
# include <dismod_at/get_prior_table.hpp>

namespace dismod_at {
   struct smooth_grid_struct {
      int    smooth_id;
      int    age_id;
      int    time_id;
      int    value_prior_id;
      int    dage_prior_id;
      int    dtime_prior_id;
      double const_value;
   };
   extern CppAD::vector<smooth_grid_struct> get_smooth_grid(
      sqlite3*                           db             ,
      const CppAD::vector<density_enum>& density_table  ,
      const CppAD::vector<prior_struct>& prior_table
   );
}

# endif
