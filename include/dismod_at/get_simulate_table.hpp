// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_SIMULATE_TABLE_HPP
# define DISMOD_AT_GET_SIMULATE_TABLE_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	struct simulate_struct {
		int         simulate_index;
		int         data_subset_id;
		double      simulate_value;
		double      simulate_delta;
	};
	extern CppAD::vector<simulate_struct> get_simulate_table(sqlite3*  db);
}

# endif
