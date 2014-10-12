// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_CHECK_RATE_SMOOTH_HPP
# define DISMOD_AT_CHECK_RATE_SMOOTH_HPP

# include <string>
# include <sqlite3.h>
# include <dismod_at/include/get_rate_table.hpp>
# include <dismod_at/include/get_smooth_table.hpp>

namespace dismod_at {
	void check_pini_n_age(
		const CppAD::vector<rate_struct>&   rate_table  ,
		const CppAD::vector<smooth_struct>& smooth_table
	);
}

# endif
