// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
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
		const std::string&                        rate_info     ,
		const CppAD::vector<rate_struct>&         rate_table    ,
		const CppAD::vector<prior_struct>&        prior_table   ,
		const CppAD::vector<smooth_grid_struct>&  smooth_grid
	);
}

# endif
