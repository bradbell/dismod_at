// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_CHECK_CHILD_PRIOR_HPP
# define DISMOD_AT_CHECK_CHILD_PRIOR_HPP

# include <string>
# include <sqlite3.h>
# include "get_rate_table.hpp"
# include "get_smooth_grid.hpp"
# include "get_nslist_pair.hpp"
# include "get_prior_table.hpp"

namespace dismod_at {
	void check_child_prior(
		sqlite3*                                 db          ,
		const CppAD::vector<rate_struct>&        rate_table  ,
		const CppAD::vector<smooth_grid_struct>& smooth_grid ,
		const CppAD::vector<nslist_pair_struct>& nslist_pair ,
		const CppAD::vector<prior_struct>&       prior_table
	);
}

# endif
