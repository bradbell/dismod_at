// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_SUBGROUP_TABLE_HPP
# define DISMOD_AT_GET_SUBGROUP_TABLE_HPP

# include <sqlite3.h>
# include <string>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	struct subgroup_struct {
		std::string  subgroup_name;
		int          group_id;
		std::string  group_name;
	};
	extern CppAD::vector<subgroup_struct>
	get_subgroup_table(sqlite3* db);
}



# endif
