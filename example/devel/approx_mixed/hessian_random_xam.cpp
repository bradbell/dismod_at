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
$begin hessian_random_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ hessian_random: Example and Test$$

$head Warning$$
This example contains Laplace distributed terms in the
joint likelihood and hence does not satisfy the
$cref/smoothness assumption
	/approx_mixed_theory
	/f(theta, u)
	/Smoothness Assumption
/$$.

$code
$verbatim%example/devel/approx_mixed/hessian_random_xam.cpp
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

			// compute this factor once
			Float sqrt_2 = CppAD::sqrt( Float(2.0) );

			for(size_t i = 0; i < y_.size(); i++)
			{	Float mu     = u[i];
				Float sigma  = theta[i];
				Float res    = (y_[i] - mu) / sigma;

				if( i % 2 == 0 )
				{	// This is a Gaussian term, so entire density is smooth
					// (do not need 2*pi inside of log)
					vec[0]  += (log(sigma) + res*res) / Float(2.0);
				}
				else
				{	// This term is Laplace distributed
					// (do not need sqrt(2) inside of log)
					vec[0] += log(sigma);

					// part of the density that need absolute value
					vec.push_back(sqrt_2 * res);
				}
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

bool hessian_random_xam(void)
{
	bool   ok = true;
	double eps = 100. * std::numeric_limits<double>::epsilon();
	double sqrt_2 = CppAD::sqrt(2.0);

	size_t n_data = 10;
	size_t n_fixed = n_data;
	size_t n_random = n_data;
	vector<double> data(n_data);
	vector< AD<double> > fixed_vec(n_fixed), random_vec(n_random);

	for(size_t i = 0; i < n_data; i++)
	{	data[i]      = double(i + 1);
		fixed_vec[i] = std::sqrt( double(i + 1) );
		random_vec[i] = 0.0;
	}

	// object that is derived from approx_mixed
	approx_derived approx_object(n_fixed, n_random, data);

	// compute Hessian with respect to random effects
	vector<size_t> row, col;
	vector< AD<double> > val;
	approx_object.hessian_random(fixed_vec, random_vec, row, col, val);

	// check the result
	size_t K = row.size();
	ok &= col.size() == K;
	ok &= val.size() == K;
	for(size_t k = 0; k < K; k++)
	{	size_t i = row[k];
		size_t j = col[k];
		double check;
		if( i == j )
		{	// must be a random effects case
			ok      &= (i >= n_fixed);
			// must be a Guassian case (Laplace cases are know to be zero)
			ok      &= i % 2 == 0;
			//
			double sigma  = Value( fixed_vec[i-n_fixed] );
			check  = 1.0 / (sigma * sigma);
		}
		else if( i == j + n_fixed )
		{	// cross partial w.r.t mu and sigma
			double sigma  = Value( fixed_vec[i-n_fixed] );
			double mu     = Value( random_vec[j] );
			double res    = (data[i-n_fixed] - mu) / sigma;
			if( i % 2 == 0 )
			{	// Gaussian likelihood case
				// Note that there is a factor of 1 / sigma in res
				check  = 2.0 * res / (sigma * sigma);
			}
			else
			{	// Laplace likelihood case
				check  = sqrt_2 * CppAD::sign(res) / (sigma * sigma);
			}
		}
		else
		{	ok &= false;
		}
		ok  &= fabs( Value( val[k] ) / check - 1.0) <= eps;
	}
	size_t n_random_cases = (n_data + 1) / 2;
	size_t n_cross_cases  = n_data;
	ok &= K == n_random_cases + n_cross_cases;
	return ok;
}
// END C++
