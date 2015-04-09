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
$begin approx_derived_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ approx_derived: Example and Test$$

$code
$verbatim%example/devel/approx_mixed/approx_derived_xam.cpp
	%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/approx_mixed.hpp>

# define IMPLEMENT_FIXED_DENSITY(Float)        \
	virtual vector< Float > fixed_density(     \
		const vector< Float >& fixed_vec  )    \
		{	return implement_fixed_density(fixed_vec); }

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
		// implementation of joint_density
		template <class Float>
		vector<Float> implement_joint_density(
			const vector<Float>& theta  ,
			const vector<Float>& u      )
		{	vector<Float> vec(1);

			// compute this factor once
			Float sqrt_2pi = Float( CppAD::sqrt( 8.0 * CppAD::atan(1.0) ) );

			// initialize summation
			vec[0] = Float(0.0);

			// for each data and random effect
			for(size_t i = 0; i < y_.size(); i++)
			{	Float mu     = u[i];
				Float sigma  = theta[i];
				Float res    = (y_[i] - mu) / sigma;

				// This is a Gaussian term, so entire density is smooth
				vec[0]  += log(sqrt_2pi * sigma) + res * res / Float(2.0);
			}
			return vec;
		}
		// implementation of fixed_density
		template <class Float>
		vector<Float> implement_fixed_density(
			const vector<Float>& theta  )
		{	vector<Float> vec(1);

			// initialize part of log-density that is smooth
			vec[0] = Float(0.0);

			// compute these factors once
			Float one    = Float(1.0);
			Float sqrt_2 = CppAD::sqrt( Float(2.0) );

			for(size_t i = 0; i < y_.size(); i++)
			{	Float sigma  = theta[i];

				// This is a Laplace term, so using Laso for this variable
				vec[0] += CppAD::log( sqrt_2 * sigma);

				// part of the density that needs absolute value
				vec.push_back(sqrt_2 * (sigma - one) );
			}
			return vec;
		}
		virtual vector< a3_double > joint_density(
			const vector< a3_double >& fixed_vec  ,
			const vector< a3_double >& random_vec )
		{	return implement_joint_density(fixed_vec, random_vec); }
		//
		IMPLEMENT_FIXED_DENSITY( double )
		IMPLEMENT_FIXED_DENSITY( AD<double> )
	};
}

bool approx_derived_xam(void)
{
	bool   ok  = true;
	double pi  = 4.0 * std::atan(1.0);
	double eps = 100. * std::numeric_limits<double>::epsilon();

	size_t n_data = 10;
	vector<double> data(n_data), fixed_vec(n_data), random_vec(n_data);

	for(size_t i = 0; i < n_data; i++)
	{	data[i]       = double(i + 1);
		fixed_vec[i]  = 1.0;
		random_vec[i] = 0.0;
	}

	// object that is derived from approx_mixed
	approx_derived approx_object(n_data, n_data, data);
	approx_object.initialize(fixed_vec, random_vec);

	// Evaluate the joint density
	vector<double> vec(1);
	vec = approx_object.implement_joint_density(fixed_vec, random_vec);

	// check the joint density
	double sum = 0.0;
	for(size_t i = 0; i < n_data; i++)
	{	double mu     = random_vec[i];
		double sigma  = fixed_vec[i];
		double res    = (data[i] - mu) / sigma;
		sum          += (std::log(2 * pi * sigma * sigma) + res * res) / 2.0;
	}
	ok &= std::fabs( vec[0] / sum - 1.0 ) < eps;

	// Evaluate the joint density
	vec.resize(1 + n_data);
	vec = approx_object.fixed_density(fixed_vec);

	// check the fixed density
	sum = 0.0;
	for(size_t i = 0; i < n_data; i++)
	{	double sigma  = fixed_vec[i];
		sum          += std::log( std::sqrt(2.0) * sigma );
		//
		double check  = std::sqrt(2.0) * (sigma - 1.0);
		ok  &= fabs( vec[1+i] / check - 1.0) < eps;
	}
	ok &= std::fabs( vec[0] / sum - 1.0 ) < eps;

	return ok;
}
// END C++
