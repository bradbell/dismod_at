// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_NSLIST_TABLE_HPP
# define DISMOD_AT_GET_NSLIST_TABLE_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	extern CppAD::vector<std::string> get_nslist_table(sqlite3*  db);
}

# endif
