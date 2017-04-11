// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
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
