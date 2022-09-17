// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_BILINEAR_INTERP_HPP
# define DISMOD_AT_BILINEAR_INTERP_HPP

# include <cppad/utility/vector.hpp>

namespace dismod_at {
		double bilinear_interp(
			double                         x          ,
			double                         y          ,
			const CppAD::vector<double>&   x_grid     ,
			const CppAD::vector<double>&   y_grid     ,
			const CppAD::vector<double>&   z_grid     ,
			size_t&                        i          ,
			size_t&                        j
		);
}

# endif
