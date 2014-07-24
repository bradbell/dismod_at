// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin avg_integrand_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ avg_integrand: Example and Test (Under Construction)$$
$index example, C++ avg_integrand$$
$index avg_integrand, C++ example$$

$code
$verbatim%example/devel/model/avg_integrand_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/dismod_at.hpp>

namespace {
	using CppAD::vector;
	typedef CppAD::AD<double> Float;
	size_t n_age_ode     = 4;
	size_t n_time_ode    = 3;
	size_t n_ode         = n_age_ode * n_time_ode;
	vector<Float> iota(n_ode), rho(n_ode), chi(n_ode), omega(n_ode);
	vector<Float> S(n_ode), C(n_ode);

}

bool avg_integrand_xam(void)
{	bool   ok = true;
	size_t i, j, k;
	using  CppAD::abs;	
	Float eps = CppAD::numeric_limits<Float>::epsilon();
	
	return ok;
}
// END C++
