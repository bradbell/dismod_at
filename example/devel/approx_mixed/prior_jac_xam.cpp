// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin prior_jac_xam.cpp$$
$spell
	jac
	interp
	xam
$$

$section C++ prior_jac: Example and Test$$

$head Private$$
This example is not part of the
$cref/approx_mixed public API/approx_mixed_public/$$.

$code
$verbatim%example/devel/approx_mixed/prior_jac_xam.cpp
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
		size_t                n_fixed_;
		const vector<double>& y_;
	public:
		// constructor
		approx_derived(
			size_t n_fixed                    ,
			size_t n_random                   ,
			const vector<double>& y           ) :
			dismod_at::approx_mixed(n_fixed, n_random) ,
			n_fixed_(n_fixed) ,
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

			// compute this factor once
			Float sqrt_2pi = Float( CppAD::sqrt( 8.0 * CppAD::atan(1.0) ) );

			// initialize summation
			vec[0] = Float(0.0);

			// for each data and random effect
			for(size_t i = 0; i < y_.size(); i++)
			{	Float mu     = theta[0] + u[i];
				Float sigma  = theta[1];
				Float res    = (y_[i] - mu) / sigma;

				// This is a Gaussian term, so entire density is smooth
				vec[0]  += log(sqrt_2pi * sigma) + res * res / Float(2.0);
			}
			return vec;
		}
		// implementation of prior_density
		template <class Float>
		vector<Float> implement_prior_density(
			const vector<Float>& fixed_vec  )
		{	vector<Float> vec(1);

			// initialize part of log-density that is smooth
			vec[0] = Float(0.0);

			// compute these factors once
			Float mu     = Float(1.0);
			Float sqrt_2 = CppAD::sqrt( Float(2.0) );

			for(size_t j = 0; j < n_fixed_; j++)
			{
				// This is a Laplace term
				vec[0] += CppAD::log( sqrt_2 );

				// part of the density that needs absolute value
				vec.push_back(sqrt_2 * (fixed_vec[j] - mu) );
			}
			return vec;
		}
	public:
		virtual vector<a5_double> joint_density(
			const vector<a5_double>& fixed_vec  ,
			const vector<a5_double>& random_vec )
		{	return implement_joint_density(fixed_vec, random_vec); }
		//
		virtual vector<a1_double> prior_density(
			const vector<a1_double>& fixed_vec  )
		{	return implement_prior_density(fixed_vec); }
		//
		virtual void fatal_error(const std::string& error_message)
		{	std::cerr << error_message << std::endl;
			std::exit(1);
		}
	};
}

bool prior_jac_xam(void)
{
	bool   ok = true;
	double eps = 100. * std::numeric_limits<double>::epsilon();
	double sqrt_2 = CppAD::sqrt(2.0);

	size_t n_data   = 10;
	size_t n_fixed  = 2;
	size_t n_random = n_data;
	vector<double> data(n_data), fixed_vec(n_fixed), random_vec(n_random);

	fixed_vec[0] = 2.0;
	fixed_vec[1] = 0.5;
	for(size_t i = 0; i < n_data; i++)
	{	data[i]       = double(i + 1);
		random_vec[i] = i / double(n_data);
	}

	// object that is derived from approx_mixed
	approx_derived approx_object(n_fixed, n_random, data);
	approx_object.initialize(fixed_vec, random_vec);

	// compute prior jacobian
	CppAD::vector<size_t> row, col;
	CppAD::vector<double> val;
	approx_object.prior_jac(fixed_vec, row, col, val);

	// initialize which rows have been found so far
	CppAD::vector<bool> found(3);
	for(size_t i = 0; i < 3; i++)
		found[i] = false;

	// check derivatives
	for(size_t k = 0; k < row.size(); k++)
	{	size_t i = row[k];
		size_t j = col[k];
		double check = sqrt_2;
		ok      &= i == j+1;
		ok      &= ! found[i];
		ok      &= ( val[k] / check - 1.0) <= eps;
		found[i] = true;
	}
	ok &= found[0] == false;
	ok &= found[1];
	ok &= found[2];

	return ok;
}
// END C++
