// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_RATE_TABLE_HPP
# define DISMOD_AT_GET_RATE_TABLE_HPP

# include <sqlite3.h>
# include <cppad/vector.hpp>

namespace dismod_at {
	enum rate_enum {
		pini_enum,
		iota_enum,
		rho_enum,
		chi_enum,
		omega_enum,
		number_rate_enum
	};
	struct rate_struct {
		rate_enum  rate;
		int        parent_smooth_id;
		int        child_smooth_id;
	};
	extern CppAD::vector<rate_struct> get_rate_table(sqlite3*  db);
}

# endif
