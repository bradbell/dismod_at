// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Density Estimation as Functions of Age and Time
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
	enum density_enum {
		gaussian_enum,
		laplace_enum,
		log_gaussian_enum,
		log_laplace_enum,
		number_density_enum
	};
	extern CppAD::vector<density_enum> get_density_table(sqlite3*  db);
}

# endif
