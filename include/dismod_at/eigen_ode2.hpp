// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_EIGEN_ODE2_HPP
# define DISMOD_AT_EIGEN_ODE2_HPP

# include <cppad/utility/vector.hpp>

namespace dismod_at {
	template <class Float>
	extern CppAD::vector<Float> eigen_ode2(
		size_t                       case_number ,
		const CppAD::vector<Float>&  b           ,
		const CppAD::vector<Float>&  yi          ,
		const Float&                 tf
	);
}
# endif
