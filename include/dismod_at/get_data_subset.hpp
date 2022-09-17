// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_DATA_SUBSET_HPP
# define DISMOD_AT_GET_DATA_SUBSET_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
   struct data_subset_struct {
      int    data_id;
      int    hold_out;
      int    density_id;
      double eta;
      double nu;
   };
   extern CppAD::vector<data_subset_struct> get_data_subset(sqlite3* db);
}

# endif
