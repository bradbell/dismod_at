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
$begin solve_ode_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ solve_ode: Example and Test$$
$index example, C++ solve_ode$$
$index solve_ode, C++ example$$

$code
$verbatim%example/devel/model/solve_ode_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/dismod_at.hpp>

namespace {
	using CppAD::vector;
	typedef CppAD::AD<double> Float;
	Float  step_size = 1.0;
	size_t n_age     = 4;
	size_t n_time    = 3;
	size_t n_grid    = n_age * n_time;
	vector<Float> iota(n_grid), rho(n_grid), chi(n_grid), omega(n_grid);

	Float interpolate(
		Float a, Float t, size_t i, size_t j, const vector<Float>& v
	)
	{	Float ai    = i * step_size;
		Float aip   = ai + step_size;
		Float vipj  = v[(i+1) * n_time + j];
		Float sum;
		if( t < 0.0 )
		{	sum     = (aip - a) * v[  i * n_time + j];
			sum    += (a  - ai) * v[(i+1)*n_time + j];
			sum    /= step_size;
			return sum;
		}
		Float tj    = j * step_size;
		Float tjp   = tj + step_size;
		sum         = (aip - a)*(tjp - t) * v[  i * n_time + j];
		sum        += (a  - ai)*(tjp - t) * v[(i+1)*n_time + j];
		sum        += (aip - a)*(t  - tj) * v[  i * n_time + j+1];
		sum        += (a  - ai)*(t  - tj) * v[(i+1)*n_time + j+1];
		sum        /= step_size * step_size;
		return sum;
	}

	class Fun {
	private:
		Float c_;
	public:
		void set_c(Float c)
		{	c_ = c; }
		void Ode(const Float& a, const vector<Float>& y, vector<Float>& yp)
		{	Float t = a + c_;
			size_t i = n_age - 2;
			while( a < i * step_size && i > 0)
				i--;
			size_t j = n_time - 2;
			while( t < j * step_size && j > 0 )
				j--;
			Float iota_a  = interpolate(a, t, i, j, iota);
			Float rho_a   = interpolate(a, t, i, j, rho);
			Float chi_a   = interpolate(a, t, i, j, chi);
			Float omega_a = interpolate(a, t, i, j, omega);
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
	size_t i, j, k;
	using  CppAD::abs;	
	Float eps = CppAD::numeric_limits<Float>::epsilon();
	
	vector<Float> p_zero(n_time);
	vector<Float> S_out, C_out;
	for(i = 0; i < n_age; i++)
	{	double ri = double(i) / double(n_age - 1);
		for(j = 0; j < n_time; j++)
		{	double rj = double(j) / double(n_time - 1);
			k        = i * n_time +  j;
			iota[k]  = 0.10 * ri * ri;
			rho[k]   = 0.09 * rj * rj;
			chi[k]   = 0.08 * ri; 
			omega[k] = 0.07 * rj; 
		}
	}
	for(j = 0; j < n_time; j++)
		p_zero[j] = 0.5;

	dismod_at::solve_ode(
		n_age, n_time, step_size, iota, rho, chi, omega, p_zero, S_out, C_out
	);

	// check integral along diagonal starting at age zero, time zero
	assert( n_age > n_time );
	Fun F;
	F.set_c(0.0);
	size_t M = 100;
	Float ai = 0.0;
	Float af = (n_time-1) * step_size;
	vector<Float> yi(2);
	yi[0] = 1.0 - p_zero[0];
	yi[1] = p_zero[0];
	vector<Float> yf = CppAD::Runge45(F, M, ai, af, yi);
	i = n_time - 1;
	j = n_time - 1;
	ok &= abs( 1.0 - S_out[ i * n_time + j] / yf[0] ) < 1e-3;
	ok &= abs( 1.0 - C_out[ i * n_time + j] / yf[1] ) < 1e-3;
	// std::cout << 1.0 - S_out[ i * n_time + j] / yf[0]  << std::endl;
	// std::cout << 1.0 - C_out[ i * n_time + j] / yf[1]  << std::endl;

	// check integral for a negative cohort
	F.set_c(-step_size);
	af = af + step_size;
	yf = CppAD::Runge45(F, M, ai, af, yi);
	i = n_time;
	j = n_time - 1;
	ok &= abs( 1.0 - S_out[ i * n_time + j] / yf[0] ) < 1e-3;
	ok &= abs( 1.0 - C_out[ i * n_time + j] / yf[1] ) < 1e-3;
	// std::cout << 1.0 - S_out[ i * n_time + j] / yf[0]  << std::endl;
	// std::cout << 1.0 - C_out[ i * n_time + j] / yf[1]  << std::endl;
	

	return ok;
}
// END C++
