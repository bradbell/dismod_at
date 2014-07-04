// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_LIKE_TABLE_HPP
# define DISMOD_AT_GET_LIKE_TABLE_HPP

# include <sqlite3.h>
# include <cppad/vector.hpp>

namespace dismod_at {
	struct like_struct {
		std::string name;
		int         density_id;
		double      lower;
		double      upper;
		double      mean;
		double      std;
		double      eta;
	};
	extern CppAD::vector<like_struct> get_like_table(sqlite3*  db);
}

# endif
