// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_WEIGHT_TABLE_HPP
# define DISMOD_AT_GET_WEIGHT_TABLE_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	struct weight_struct {
		std::string weight_name;
		int         n_age;
		int         n_time;
	};
	extern CppAD::vector<weight_struct> get_weight_table(sqlite3*  db);
}

# endif
