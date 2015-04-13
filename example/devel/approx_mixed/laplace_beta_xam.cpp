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
	\B{p}( y_i | theta , u ) \sim \B{N} ( u_i + \theta_0 , \theta_1^2 )
\] $$
$latex \[
	\B{p}( u_i | theta ) \sim \B{N} ( 0 , 1 )
\] $$
It follows that the Laplace approximation is exact and
$latex \[
	\B{p}( y_i | theta ) \sim \B{N} ( \theta_0 , 1 + \theta_1^2 )
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
		{	assert( n_fixed == 2);
		}
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
			{	Float mu     = u[i] + theta[0];
				Float sigma  = theta[1];
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
		virtual vector<a5_double> joint_density(
			const vector<a5_double>& fixed_vec  ,
			const vector<a5_double>& random_vec )
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
	typedef dismod_at::approx_mixed::a2_double a2_double;

	size_t n_data   = 10;
	size_t n_fixed  = 2;
	size_t n_random = n_data;
	vector<double> data(n_data), fixed_vec(n_fixed), random_vec(n_random);
	vector<a2_double>
		a2_beta(n_fixed), a2_theta(n_fixed), a2_uhat(n_random);

	fixed_vec[0] = 2.0;
	fixed_vec[1] = 1.0;
	for(size_t i = 0; i < n_data; i++)
	{	data[i]       = double(i + 1);
		random_vec[i] = i / double(n_data);
	}
	for(size_t j = 0; j < n_fixed; j++)
	{	a2_beta[j]    = a2_double( fixed_vec[j] );
		a2_theta[j]   = a2_beta[j];
	}

	// object that is derived from approx_mixed
	approx_derived approx_object(n_fixed, n_random, data);
	approx_object.initialize(fixed_vec, random_vec);

	// optimize the random effects
	vector<double> uhat = approx_object.optimize_random(fixed_vec, random_vec);
	for(size_t i = 0; i < n_data; i++)
		a2_uhat[i] = a2_double( uhat[i] );

	// compute partial of joint part of Laplace approximation w.r.t beta
	vector<a2_double> a2_H_beta = approx_object.laplace_beta(
		a2_beta, a2_theta, a2_uhat
	);

	// For this case the Laplace approximation is exactly equal the integral
	// p(y | theta ) = integral of p(y | theta , u) p(u | theta) du
	// Furthermore p(y | theta ) is simple to calculate directly
	double mu         = fixed_vec[0];
	double sigma      = fixed_vec[1];
	//
	double d_mu_0     = 1.0;
	double d_sigma_1  = 1.0;
	//
	double delta      = CppAD::sqrt( sigma * sigma + 1.0 );
	double d_delta_1  = d_sigma_1 * sigma / delta;
	//
	double d_sum_0    = 0.0;
	double d_sum_1    = 0.0;
	for(size_t i = 0; i < n_random; i++)
	{	double res    = (data[i] - mu) / delta;
		// - log p( y_i | theta ) = log(sqrt_2pi * delta) + res*res / 2.0;
		// so compute partials w.r.t. sigma = theta_i
		double d_res_0    = - d_mu_0 / delta;
		double d_res_1    = - d_delta_1 * (data[i] - mu) / ( delta * delta) ;
		//
		double d_log_1    = d_delta_1 / delta;
		double d_square_0 = d_res_0 * res;
		double d_square_1 = d_res_1 * res;
		//
		d_sum_0          += d_square_0;
		d_sum_1          += d_log_1 + d_square_1;
	}
	ok &= abs( a2_H_beta[0] / a2_double(d_sum_0) - a2_double(1.0) ) < eps;
	ok &= abs( a2_H_beta[1] / a2_double(d_sum_1) - a2_double(1.0) ) < eps;

	return ok;
}
// END C++
