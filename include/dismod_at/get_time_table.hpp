// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_TIME_TABLE_HPP
# define DISMOD_AT_GET_TIME_TABLE_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/vector.hpp>

namespace dismod_at {
	extern CppAD::vector<double> get_time_table(sqlite3*  db);
}

# endif
