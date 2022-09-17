// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_AGE_AVG_GRID_HPP
# define DISMOD_AT_AGE_AVG_GRID_HPP

# include <string>
# include <cppad/utility/vector.hpp>

namespace dismod_at {

CppAD::vector<double> age_avg_grid(
	double                         ode_step_size  ,
	const std::string              age_avg_split  ,
	const CppAD::vector<double>&   age_table
);

}

# endif
