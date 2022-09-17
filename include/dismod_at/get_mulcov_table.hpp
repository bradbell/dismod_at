// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_MULCOV_TABLE_HPP
# define DISMOD_AT_GET_MULCOV_TABLE_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include "mulcov_type.hpp"
# include "get_subgroup_table.hpp"

namespace dismod_at {
	struct mulcov_struct {
		mulcov_type_enum mulcov_type;
		int              rate_id;
		int              integrand_id;
		int              covariate_id;
		int              group_id;
		int              group_smooth_id;
		int              subgroup_smooth_id;
	};
	extern CppAD::vector<mulcov_struct> get_mulcov_table(
		sqlite3*                              db              ,
		const CppAD::vector<subgroup_struct>& subgroup_table
	);
}

# endif
