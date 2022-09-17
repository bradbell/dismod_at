// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_CHECK_CHILD_NSLIST_HPP
# define DISMOD_AT_CHECK_CHILD_NSLIST_HPP

# include <string>
# include <sqlite3.h>
# include "get_option_table.hpp"
# include "get_rate_table.hpp"
# include "get_node_table.hpp"
# include "get_nslist_table.hpp"
# include "get_nslist_pair.hpp"

namespace dismod_at {
	void check_child_nslist(
		sqlite3*                                 db             ,
		const CppAD::vector<option_struct>&      option_table   ,
		const CppAD::vector<rate_struct>&        rate_table     ,
		const CppAD::vector<node_struct>&        node_table     ,
		const CppAD::vector<std::string>&        nslist_table   ,
		const CppAD::vector<nslist_pair_struct>& nslist_pair
	);
}

# endif
