/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_COHORT_ODE_HPP
# define DISMOD_AT_COHORT_ODE_HPP

# include <cppad/cppad.hpp>

namespace dismod_at {

	template <class Float>
	extern void cohort_ode(
		const std::string&           rate_case ,
		const CppAD::vector<double>& age       ,
		const Float&                 pini      ,
		const CppAD::vector<Float>&  iota      ,
		const CppAD::vector<Float>&  rho       ,
		const CppAD::vector<Float>&  chi       ,
		const CppAD::vector<Float>&  omega     ,
		      CppAD::vector<Float>&  s_out     ,
		      CppAD::vector<Float>&  c_out
	);
}

# endif
