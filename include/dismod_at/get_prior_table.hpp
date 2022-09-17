// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_PRIOR_TABLE_HPP
# define DISMOD_AT_GET_PRIOR_TABLE_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include "get_density_table.hpp"

namespace dismod_at {
	struct prior_struct {
		std::string prior_name;
		int         density_id;
		double      lower;
		double      upper;
		double      mean;
		double      std;
		double      eta;
		double      nu;
	};
	extern CppAD::vector<prior_struct> get_prior_table(
		sqlite3*                           db            ,
		const CppAD::vector<density_enum>& density_table
	);
}

# endif
