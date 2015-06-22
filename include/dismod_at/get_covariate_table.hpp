// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_COVARIATE_TABLE_HPP
# define DISMOD_AT_GET_COVARIATE_TABLE_HPP

# include <sqlite3.h>
# include <cppad/vector.hpp>

namespace dismod_at {
	struct covariate_struct {
		std::string covariate_name;
		double      reference;
		double      max_difference;
	};
	extern CppAD::vector<covariate_struct> get_covariate_table(sqlite3*  db);
}

# endif
