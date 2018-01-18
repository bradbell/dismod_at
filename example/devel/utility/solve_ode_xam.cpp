// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin solve_ode_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ solve_ode: Example and Test$$
$index example, C++ solve_ode$$
$index solve_ode, C++ example$$

$code
$srcfile%example/devel/utility/solve_ode_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/solve_ode.hpp>

namespace {
	using CppAD::vector;
	typedef CppAD::AD<double> Float;
	Float  step_size = 1.0;
	vector<Float> iota, rho, chi, omega;

	Float interpolate(const Float& a, size_t k, const vector<Float>& v)
	{	Float ak    = double(k) * step_size;
		Float akp   = ak + step_size;
		Float vk    = v[k];
		Float vkp   = v[k + 1];
		Float sum   = ( (akp - a) * vk + (a - ak) * vkp ) / (akp - ak);
		return sum;
	}

	class Fun {
	public:
		void Ode(const Float& a, const vector<Float>& y, vector<Float>& yp)
		{
			size_t k = iota.size() - 2;
			while( a < double(k) * step_size && k > 0)
				k--;
			Float iota_a  = interpolate(a, k, iota);
			Float rho_a   = interpolate(a, k, rho);
			Float chi_a   = interpolate(a, k, chi);
			Float omega_a = interpolate(a, k, omega);
			Float S_a     = y[0];
			Float C_a     = y[1];
			yp[0]         = - (iota_a + omega_a) * S_a + rho_a * C_a;
			yp[1]         = + iota_a * S_a - (rho_a + chi_a + omega_a) * C_a;
			return;
		}
	};
}

bool solve_ode_xam(void)
{	bool   ok = true;

	// check integral along diagonal starting at age zero, time zero
	size_t i_max  = 5;
	size_t j_max  = 5;
	Float  pini   = 0.5;
	vector<Float> S_out, C_out;
	iota.resize(i_max+1);
	rho.resize(i_max+1);
	chi.resize(i_max+1);
	omega.resize(i_max+1);
	for(size_t k = 0; k <= i_max; k++)
	{	double ri = double(k) / double(i_max);
		iota[k]  = 0.10 * ri * ri;
		rho[k]   = 0.09 * ri * ri;
		chi[k]   = 0.08 * ri * ri;
		omega[k] = 0.07 * ri * ri;
	}
	size_t eigen_ode2_case_number = 4;
	dismod_at::solve_ode(
		eigen_ode2_case_number,
		i_max, j_max, step_size, pini, iota, rho, chi, omega, S_out, C_out
	);
	Fun F;
	size_t M = 100;
	Float ai = 0.0;
	Float af = double(i_max) * step_size;
	vector<Float> yi(2);
	yi[0] = 1.0 - pini;
	yi[1] = pini;
	vector<Float> yf = CppAD::Runge45(F, M, ai, af, yi);
	//
	size_t k = i_max;
	ok &= fabs( 1.0 - S_out[k] / yf[0] ) < 1e-6;
	ok &= fabs( 1.0 - C_out[k] / yf[1] ) < 1e-6;
	//
	return ok;
}
// END C++
