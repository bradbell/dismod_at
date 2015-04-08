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
$begin joing_laplace_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ joint_laplace: Example and Test$$

$verbatim%example/devel/approx_mixed/joint_laplace.cpp
	%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/approx_mixed.hpp>

# define IMPLEMENT_JOINT_DENSITY(Float)        \
	virtual vector< Float > joint_density(     \
		const vector< Float >& fixed_vec  ,    \
		const vector< Float >& random_vec )    \
		{	return implement_joint_density(fixed_vec, random_vec); }

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
		IMPLEMENT_JOINT_DENSITY( double )
		IMPLEMENT_JOINT_DENSITY( AD<double> )
		IMPLEMENT_JOINT_DENSITY( AD< AD<double> > )
		IMPLEMENT_JOINT_DENSITY( AD< AD< AD<double> > > )
	};
}

bool joint_laplace_xam(void)
{
	bool   ok = true;
	double eps = 100. * std::numeric_limits<double>::epsilon();
	double sqrt_2pi = CppAD::sqrt(8.0 * CppAD::atan(1.0) );

	size_t n_data   = 10;
	size_t n_fixed  = n_data;
	size_t n_random = n_data;
	vector<double> data(n_data), fixed_vec(n_fixed), random_vec(n_random);
	vector< AD<double> >
		a1_beta(n_fixed), a1_theta(n_fixed), a1_uhat(n_random);

	for(size_t i = 0; i < n_data; i++)
	{	data[i]       = double(i + 1);
		fixed_vec[i]  = std::sqrt( double(i + 1) );
		a1_beta[i]    = fixed_vec[i];
		a1_theta[i]   = a1_beta[i];
		random_vec[i] = i / double(n_data);
	}

	// object that is derived from approx_mixed
	approx_derived approx_object(n_fixed, n_random, data);

	// optimize the random effects
	vector<double> uhat = approx_object.optimize_random(fixed_vec, random_vec);
	for(size_t i = 0; i < n_data; i++)
		a1_uhat[i] = uhat[i];

	// compute joint part of Laplace approximation
	AD<double> a1_laplace = approx_object.joint_laplace(
		a1_beta, a1_theta, a1_uhat
	);

	// For this case the Laplace approximation is exactly equal the integral
	// p(y | theta ) = integral of p(y | theta , u) p(u | theta) du
	// Furthermore p(y | theta ) is simple to calculate directly
	double zero   = 0.0;
	double check  = 0.0;
	for(size_t i = 0; i < n_random; i++)
	{	double sigma  = fixed_vec[i];
		double delta  = CppAD::sqrt( sigma * sigma + 1.0 );
		double res    = (data[i] - zero) / delta;
		check        += CppAD::log(sqrt_2pi * delta) + res*res / 2.0;
	}
	ok &= CppAD::abs( a1_laplace / check - 1.0 ) < eps;

	return ok;
}
// END C++
