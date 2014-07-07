// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_MULTIPLIER_TABLE_HPP
# define DISMOD_AT_GET_MULTIPLIER_TABLE_HPP

# include <sqlite3.h>
# include <cppad/vector.hpp>

namespace dismod_at {
	struct multiplier_struct {
		std::string multiplier_type;
		int         rate_id;
		int         integrand_id;
		int         covariate_id;
		int         smooth_id;
	};
	extern CppAD::vector<multiplier_struct> get_multiplier_table(sqlite3*  db);
}

# endif
