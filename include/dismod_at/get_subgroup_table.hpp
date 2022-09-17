// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
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
