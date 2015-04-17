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
$begin joint_grad_ran_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ joint_grad_ran: Example and Test$$

$head Private$$
This example is not part of the
$cref/approx_mixed public API/approx_mixed_public/$$.

$code
$verbatim%example/devel/approx_mixed/joint_grad_ran_xam.cpp
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
		//
		virtual vector<a5_double> joint_density(
			const vector<a5_double>& fixed_vec  ,
			const vector<a5_double>& random_vec )
		{	return implement_joint_density(fixed_vec, random_vec); }
		//
		virtual vector<a1_double> fixed_density(
			const vector<a1_double>& fixed_vec  )
		{	a1d_vector vec(1);
			vec[0] = 0.0;
			return vec;
		}
	};
}

bool joint_grad_ran_xam(void)
{
	bool   ok = true;
	double eps = 100. * std::numeric_limits<double>::epsilon();
	double sqrt_2 = std::sqrt( 2.0 );
	typedef AD< AD< AD<double> > > a3_double;

	size_t n_data   = 10;
	size_t n_fixed  = n_data;
	size_t n_random = n_data;
	vector<double> data(n_data);
	vector<double> theta(n_fixed), u(n_random);
	vector<a3_double> fixed_vec(n_fixed), random_vec(n_random);

	for(size_t i = 0; i < n_data; i++)
	{	data[i]      = double(i + 1);
		fixed_vec[i] = theta[i] =std::sqrt( double(i + 1) );
		random_vec[i] = u[i] = 0.0;
	}

	// object that is derived from approx_mixed
	approx_derived approx_object(n_fixed, n_random, data);
	approx_object.initialize(theta, u);

	// compute gradient with respect to random effects
	vector<a3_double> grad =
		approx_object.joint_grad_ran(fixed_vec, random_vec);

	// The Laplace terms are known to have zero Hessian w.r.t random effects
	for(size_t i = 0; i < n_random; i++)
	{	a3_double sigma  = fixed_vec[i];
		a3_double mu     = random_vec[i];
		a3_double res    = (a3_double(data[i]) - mu) / sigma;
		a3_double check;
		if( i % 2 == 0 )
			check  = - res / sigma;
		else
			check  = - sqrt_2 * CppAD::sign(res) / sigma;
		//
		ok           &= abs( grad[i] / check - 1.0) <= eps;
	}

	return ok;
}
// END C++
