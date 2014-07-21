// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_SOLVE_ODE_HPP
# define DISMOD_AT_SOLVE_ODE_HPP

# include <cppad/cppad.hpp>

namespace dismod_at {

	template <class Float>
	extern void solve_ode(
		size_t                       n_age     ,
		size_t                       n_time    ,
		const Float&                 step_size ,
		const CppAD::vector<Float>&  iota      ,
		const CppAD::vector<Float>&  rho       ,
		const CppAD::vector<Float>&  chi       ,
		const CppAD::vector<Float>&  omega     ,
		const CppAD::vector<Float>&  p_zero    ,
		      CppAD::vector<Float>&  S_out     ,
		      CppAD::vector<Float>&  C_out
	);
}

# endif
