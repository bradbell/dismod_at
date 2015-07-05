// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_ARGUMENT_TABLE_HPP
# define DISMOD_AT_GET_ARGUMENT_TABLE_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/vector.hpp>

namespace dismod_at {
	struct argument_struct {
		std::string argument_name;
		std::string argument_value;
	};
	extern CppAD::vector<argument_struct> get_argument_table(sqlite3*  db);
}

# endif
