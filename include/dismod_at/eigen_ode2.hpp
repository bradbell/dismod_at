// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_EIGEN_ODE2_HPP
# define DISMOD_AT_EIGEN_ODE2_HPP

# include <cppad/vector.hpp>

namespace dismod_at {
	template <class Float>
	extern void eigen_ode2(
		const Float&                 tf  ,
		const CppAD::vector<Float>&  a   ,
		const CppAD::vector<Float>&  yi  ,
		      CppAD::vector<Float>&  yf
	);
}
# endif
