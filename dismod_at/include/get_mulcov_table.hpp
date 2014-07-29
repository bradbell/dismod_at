// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_MULCOV_TABLE_HPP
# define DISMOD_AT_GET_MULCOV_TABLE_HPP

# include <sqlite3.h>
# include <cppad/vector.hpp>

namespace dismod_at {
	struct mulcov_struct {
		std::string mulcov_type;
		int         rate_id;
		int         integrand_id;
		int         covariate_id;
		int         smooth_id;
	};
	extern CppAD::vector<mulcov_struct> get_mulcov_table(sqlite3*  db);
}

# endif
