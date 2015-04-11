// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin laplace_beta_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ laplace_beta: Example and Test$$

$head Model$$
$latex \[
	\B{p}( y_i | theta , u ) \sim \B{N} ( u_i , \theta_i^2 )
\] $$
$latex \[
	\B{p}( u_i | theta ) \sim \B{N} ( 0 , 1 )
\] $$
It follows that the Laplace approximation is exact and
$latex \[
	\B{p}( y_i | theta ) \sim \B{N} ( 0, 1 + \theta_i^2 )
\] $$

$code
$verbatim%example/devel/approx_mixed/laplace_beta_xam.cpp
	%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/approx_mixed.hpp>

namespace {
	using CppAD::vector;
	using CppAD::log;
	using CppAD::AD;

	class approx_derived : public dismod_at::approx_mixed {
	private:
		const vector<double>& y_;
	public:
		// constructor
		approx_derived(
			size_t n_fixed                    ,
			size_t n_random                   ,
			const vector<double>& y           )
			:
			dismod_at::approx_mixed(n_fixed, n_random) ,
			y_(y)
		{ }
	private:
		// implementation of joint_density
		template <class Float>
		vector<Float> implement_joint_density(
			const vector<Float>& theta  ,
			const vector<Float>& u      )
		{	vector<Float> vec(1);

			// initialize part of log-density that is always smooth
			vec[0] = Float(0.0);

			// pi
			Float sqrt_2pi = Float( CppAD::sqrt(8.0 * CppAD::atan(1.0) ) );

			for(size_t i = 0; i < y_.size(); i++)
			{	Float mu     = u[i];
				Float sigma  = theta[i];
				Float res    = (y_[i] - mu) / sigma;

				// p(y_i | u, theta)
				vec[0] += log(sqrt_2pi * sigma) + res*res / Float(2.0);

				// p(u_i | theta)
				vec[0] += log(sqrt_2pi) + u[i] * u[i] / Float(2.0);
			}
			return vec;
		}
	public:
		//
		virtual vector<a4_double> joint_density(
			const vector<a4_double>& fixed_vec  ,
			const vector<a4_double>& random_vec )
		{	return implement_joint_density(fixed_vec, random_vec); }
		//
		virtual vector<double> fixed_density(
			const vector<double>& fixed_vec  )
		{	assert(false);
			return vector<double>();
		}
		//
		virtual vector<a1_double> fixed_density(
			const vector<a1_double>& fixed_vec  )
		{	assert(false);
			return vector<a1_double>();
		}
	};
}

bool laplace_beta_xam(void)
{
	bool   ok = true;
	double eps = 100. * std::numeric_limits<double>::epsilon();
	typedef dismod_at::approx_mixed::a1_double a1_double;

	size_t n_data   = 10;
	size_t n_fixed  = n_data;
	size_t n_random = n_data;
	vector<double> data(n_data), fixed_vec(n_fixed), random_vec(n_random);
	vector<a1_double>
		a1_beta(n_fixed), a1_theta(n_fixed), a1_uhat(n_random);

	for(size_t i = 0; i < n_data; i++)
	{	data[i]       = double(i + 1);
		fixed_vec[i]  = std::sqrt( double(i + 1) );
		random_vec[i] = i / double(n_data);
		//
		a1_beta[i]    = a1_double( fixed_vec[i] );
		a1_theta[i]   = a1_beta[i];
	}

	// object that is derived from approx_mixed
	approx_derived approx_object(n_fixed, n_random, data);
	approx_object.initialize(fixed_vec, random_vec);

	// optimize the random effects
	vector<double> uhat = approx_object.optimize_random(fixed_vec, random_vec);
	for(size_t i = 0; i < n_data; i++)
		a1_uhat[i] = a1_double( uhat[i] );

	// compute partial of joint part of Laplace approximation w.r.t beta
	vector<a1_double> a1_H_beta = approx_object.laplace_beta(
		a1_beta, a1_theta, a1_uhat
	);

	// For this case the Laplace approximation is exactly equal the integral
	// p(y | theta ) = integral of p(y | theta , u) p(u | theta) du
	// Furthermore p(y | theta ) is simple to calculate directly
	double zero   = 0.0;
	for(size_t i = 0; i < n_random; i++)
	{	double sigma  = fixed_vec[i];
		double delta  = CppAD::sqrt( sigma * sigma + 1.0 );
		double res    = (data[i] - zero) / delta;
		// - log p( y_i | theta ) = log(sqrt_2pi * delta) + res*res / 2.0;
		// so compute partials w.r.t. sigma = theta_i
		double d_sigma  = 1.0;
		double d_delta  = d_sigma * sigma / delta;
		double d_res    = - d_delta * (data[i] - zero) / ( delta * delta) ;
		double d_log    = d_delta / delta;
		double d_square = d_res * res;
		double check    = d_log + d_square;
		ok &= abs( a1_H_beta[i] / a1_double(check) - a1_double(1.0) ) < eps;
	}

	return ok;
}
// END C++
