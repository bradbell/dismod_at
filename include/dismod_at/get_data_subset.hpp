// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_DATA_SUBSET_HPP
# define DISMOD_AT_GET_DATA_SUBSET_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	struct data_subset_struct {
		int    data_id;
		int    hold_out;
	};
	extern CppAD::vector<data_subset_struct> get_data_subset(sqlite3* db);
}

# endif
