// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_SMOOTH_LIST_HPP
# define DISMOD_AT_GET_SMOOTH_LIST_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	struct smooth_list_struct {
		int         smooth_list_index;
		int         node_id;
		int         smooth_id;
	};
	CppAD::vector<smooth_list_struct> get_smooth_list(sqlite3*  db);
}

# endif
