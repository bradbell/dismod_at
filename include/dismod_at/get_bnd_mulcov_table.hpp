// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_BND_MULCOV_TABLE_HPP
# define DISMOD_AT_GET_BND_MULCOV_TABLE_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	struct bnd_mulcov_struct {
		double max_mulcov;
	};
	extern CppAD::vector<bnd_mulcov_struct> get_bnd_mulcov_table(sqlite3* db);
}

# endif
