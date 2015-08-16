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
$begin ran_like_hes_xam.cpp$$
$spell
	hes
	interp
	xam
$$

$section C++ ran_like_hes: Example and Test$$


$head Private$$
This example is not part of the
$cref/approx_mixed public API/approx_mixed_public/$$.

$code
$verbatim%example/devel/approx_mixed/private/ran_like_hes_xam.cpp
	%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/approx_mixed.hpp>
# include <dismod_at/approx_pack.hpp>

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
		// implementation of ran_like
		template <class Float>
		vector<Float> implement_ran_like(
			const vector<Float>& theta  ,
			const vector<Float>& u      )
		{	vector<Float> vec(1);

			// initialize part of log-density that is always smooth
			vec[0] = Float(0.0);

			for(size_t i = 0; i < y_.size(); i++)
			{	Float mu     = u[i];
				Float sigma  = theta[i];
				Float res    = (y_[i] - mu) / sigma;

				// (do not need 2*pi inside of log)
				vec[0]  += (log(sigma) + res*res) / Float(2.0);
			}
			return vec;
		}
	public:
		//
		virtual vector<a2_double> ran_like(
			const vector<a2_double>& fixed_vec  ,
			const vector<a2_double>& random_vec )
		{	return implement_ran_like(fixed_vec, random_vec); }
		//
		virtual vector<a1_double> fix_like(
			const vector<a1_double>& fixed_vec  )
		{	a1d_vector vec(1);
			vec[0] = 0.0;
			return vec;
		}
		//
		virtual vector<a1_double> constraint(
			const vector<a1_double>& fixed_vec  )
		{	return vector<a1_double>(0); } // empty vector
		//
		virtual void fatal_error(const std::string& error_message)
		{	std::cerr << "Error: " << error_message << std::endl;
			std::exit(1);
		}
		//
		virtual void warning(const std::string& warning_message)
		{	std::cerr << "Warning: " << warning_message << std::endl;
		}
	};
}

bool ran_like_hes_xam(void)
{
	bool   ok = true;
	double eps = 100. * std::numeric_limits<double>::epsilon();
	typedef AD<double> a1_double;

	size_t n_data   = 10;
	size_t n_fixed  = n_data;
	size_t n_random = n_data;
	vector<double> data(n_data);
	vector<double> theta(n_fixed), u(n_random);
	vector<a1_double> fixed_vec(n_fixed), random_vec(n_random);

	for(size_t i = 0; i < n_data; i++)
	{	data[i]      = double( (i + 1) * (i + 1) );
		fixed_vec[i] = theta[i] = std::sqrt( double(i + 1) );
		random_vec[i] = u[i] = 0.0;
	}

	// object that is derived from approx_mixed
	approx_derived approx_object(n_fixed, n_random, data);
	approx_object.initialize(theta, u);

	// number of non-zeros in Hessian w.r.t random effects
	ok &= approx_object.hes_ran_row_.size() == n_random;
	ok &= approx_object.hes_ran_col_.size() == n_random;

	// compute Hessian with respect to random effects
	vector<a1_double> both_vec(n_fixed + n_random), val_out(n_random);
	approx_object.pack(fixed_vec, random_vec, both_vec);
	approx_object.ran_like_hes(both_vec, val_out);

	// check Hessian
	for(size_t k = 0; k < n_random; k++)
	{	ok     &= approx_object.hes_ran_row_[k] >= n_fixed;
		ok     &= approx_object.hes_ran_col_[k] >= n_fixed;
		size_t i = approx_object.hes_ran_row_[k] - n_fixed;
		size_t j = approx_object.hes_ran_col_[k] - n_fixed;
		ok      &= (i == j);
		//
		a1_double sigma  = fixed_vec[i];
		a1_double check  = a1_double(1.0) / (sigma * sigma);
		ok              &= abs( val_out[k] / check - 1.0) <= eps;
	}

	return ok;
}
// END C++
