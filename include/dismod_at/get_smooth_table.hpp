// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_SMOOTH_TABLE_HPP
# define DISMOD_AT_GET_SMOOTH_TABLE_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	struct smooth_struct {
		std::string smooth_name;
		int         n_age;
		int         n_time;
		int         mulstd_value_prior_id;
		int         mulstd_dage_prior_id;
		int         mulstd_dtime_prior_id;
	};
	extern CppAD::vector<smooth_struct> get_smooth_table(sqlite3*  db);
}

# endif
