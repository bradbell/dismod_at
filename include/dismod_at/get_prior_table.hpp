// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_PRIOR_TABLE_HPP
# define DISMOD_AT_GET_PRIOR_TABLE_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	struct prior_struct {
		std::string prior_name;
		int         density_id;
		double      lower;
		double      upper;
		double      mean;
		double      std;
		double      eta;
	};
	extern CppAD::vector<prior_struct> get_prior_table(sqlite3*  db);
}

# endif
