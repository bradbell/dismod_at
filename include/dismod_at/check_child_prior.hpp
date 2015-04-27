// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_CHECK_CHILD_PRIOR_HPP
# define DISMOD_AT_CHECK_CHILD_PRIOR_HPP

# include <string>
# include <sqlite3.h>
# include "get_rate_table.hpp"
# include "get_smooth_grid.hpp"
# include "get_prior_table.hpp"

namespace dismod_at {
	void check_child_prior(
		const CppAD::vector<rate_struct>&        rate_table  ,
		const CppAD::vector<smooth_grid_struct>& smooth_grid ,
		const CppAD::vector<prior_struct>&       prior_table
	);
}

# endif
