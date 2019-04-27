// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
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
