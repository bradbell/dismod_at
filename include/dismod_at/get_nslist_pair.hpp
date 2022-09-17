// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_NSLIST_PAIR_HPP
# define DISMOD_AT_GET_NSLIST_PAIR_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
   struct nslist_pair_struct {
      int         nslist_id;
      int         node_id;
      int         smooth_id;
   };
   CppAD::vector<nslist_pair_struct> get_nslist_pair(sqlite3*  db);
}

# endif
