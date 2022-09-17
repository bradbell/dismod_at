// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_CHECK_RATE_LIMIT_HPP
# define DISMOD_AT_CHECK_RATE_LIMIT_HPP

# include <string>
# include <sqlite3.h>
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/get_prior_table.hpp>
# include <dismod_at/get_smooth_grid.hpp>

namespace dismod_at {
	extern void check_rate_limit(
		sqlite3*                                  db            ,
		const std::string&                        rate_case     ,
		const CppAD::vector<rate_struct>&         rate_table    ,
		const CppAD::vector<prior_struct>&        prior_table   ,
		const CppAD::vector<smooth_grid_struct>&  smooth_grid
	);
}

# endif
