// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_CHECK_ZERO_SUM_HPP
# define DISMOD_AT_CHECK_ZERO_SUM_HPP

# include <string>
# include <sqlite3.h>
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/get_option_table.hpp>

namespace dismod_at {
	void check_zero_sum(
		sqlite3*                                  db            ,
		const CppAD::vector<rate_struct>&         rate_table    ,
		const CppAD::vector<option_struct>&       option_table
	);
}

# endif
