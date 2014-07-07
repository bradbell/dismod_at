// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_RATE_PRIOR_HPP
# define DISMOD_AT_GET_RATE_PRIOR_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/vector.hpp>

namespace dismod_at {
	struct rate_prior_struct {
		int    rate_id;
		int    is_parent;
		int    smooth_id;
	};
	extern CppAD::vector<rate_prior_struct> get_rate_prior(sqlite3*  db);
}

# endif
