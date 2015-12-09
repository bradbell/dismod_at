// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
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
