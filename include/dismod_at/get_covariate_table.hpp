// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_COVARIATE_TABLE_HPP
# define DISMOD_AT_GET_COVARIATE_TABLE_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	struct covariate_struct {
		std::string covariate_name;
		double      reference;
		double      max_difference;
	};
	extern CppAD::vector<covariate_struct> get_covariate_table(sqlite3*  db);
}

# endif
