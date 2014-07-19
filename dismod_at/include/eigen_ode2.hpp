// $Id$
/* --------------------------------------------------------------------------
dismod_ode: MCMC Estimation of Disease Rates as Functions of Age
          Copyright (C) 2013 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_EIGEN_ODE2_HPP
# define DISMOD_AT_EIGEN_ODE2_HPP

# include <cppad/vector.hpp>

namespace dismod_at {
	extern void eigen_ode2(
		double tf                        , 
		const CppAD::vector<double>& a   , 
		const CppAD::vector<double>& yi  ,
		      CppAD::vector<double>& yf
	);
}
# endif
