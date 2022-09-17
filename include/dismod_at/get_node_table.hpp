// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_NODE_TABLE_HPP
# define DISMOD_AT_GET_NODE_TABLE_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	struct node_struct {
		std::string node_name;
		int         parent;
	};
	extern CppAD::vector<node_struct> get_node_table(sqlite3*  db);
}

# endif
