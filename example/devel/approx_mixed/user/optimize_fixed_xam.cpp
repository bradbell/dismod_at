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
$begin optimize_fixed_xam.cpp$$
$spell
	hes
	eval
	interp
	xam
$$

$section C++ optimize_fixed_xam: Example and Test$$

$head Model$$
$latex \[
	\B{p}( y_i | \theta , u ) \sim \B{N} ( u_i + \theta_0 , \theta_1^2 )
\] $$
$latex \[
	\B{p}( u_i | \theta ) \sim \B{N} ( 0 , 1 )
\] $$
$latex \[
	\B{p}( \theta ) \sim \B{N} ( 4 , 1 )
\] $$
It follows that the Laplace approximation is exact and
$latex \[
	\B{p}( y_i | \theta ) \sim \B{N} \left( \theta_0 , 1 + \theta_1^2 \right)
\] $$
The corresponding objective for the fixed effects is equivalent to:
$latex \[
F( \theta ) = \frac{1}{2} \left[
	( \theta_0 - 4 )^2 + ( \theta_1 - 4 )^2 +
		N \log \left( 1 + \theta_1^2 \right) +
		( 1 + \theta_1^2)^{-1} \sum_{i=0}^{N-1} ( y_i - \theta_0 )^2
\right]
\] $$
The constraints on the fixed effect are
$latex \[
	- \infty \leq \theta_0 \leq + \infty
	\R{\; and \;}
	0.1 \leq \theta_1 \leq 100
\] $$

$code
$verbatim%example/devel/approx_mixed/user/optimize_fixed_xam.cpp
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
	// ----------------------------------------------------------------------
	public:
		// constructor
		approx_derived(
			size_t n_fixed                    ,
			size_t n_random                   ,
			const vector<double>& y           ) :
			dismod_at::approx_mixed(n_fixed, n_random) ,
			n_fixed_(n_fixed)                          ,
			y_(y)
		{	assert( n_fixed == 2); }
	// ----------------------------------------------------------------------
	private:
		// implementation of joint_like
		template <class Float>
		vector<Float> implement_joint_like(
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
		// implementation of fix_like
		template <class Float>
		vector<Float> implement_fix_like(
			const vector<Float>& fixed_vec  )
		{	vector<Float> vec(1);

			// initialize part of log-density that is smooth
			vec[0] = Float(0.0);

			// compute these factors once
			Float sqrt_2pi = Float( CppAD::sqrt( 8.0 * CppAD::atan(1.0) ) );

			for(size_t j = 0; j < n_fixed_; j++)
			{	Float mu     = Float(4.0);
				Float sigma  = Float(1.0);
				Float res    = (fixed_vec[j] - mu);

				// This is a Gaussian term, so entire density is smooth
				vec[0]  += log(sqrt_2pi * sigma) + res * res / Float(2.0);
			}
			return vec;
		}
	// ----------------------------------------------------------------------
	public:
		// User defined virtual functions
		virtual vector<a5_double> joint_like(
			const vector<a5_double>& fixed_vec  ,
			const vector<a5_double>& random_vec )
		{	return implement_joint_like(fixed_vec, random_vec); }
		//
		virtual vector<a1_double> fix_like(
			const vector<a1_double>& fixed_vec  )
		{	return implement_fix_like(fixed_vec); }
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
		// ------------------------------------------------------------------
	};
}

bool optimize_fixed_xam(void)
{
	bool   ok = true;
	double inf = std::numeric_limits<double>::infinity();
	double tol = 1e-8;

	size_t n_data   = 10;
	size_t n_fixed  = 2;
	size_t n_random = n_data;
	vector<double>
		fixed_lower(n_fixed), fixed_in(n_fixed), fixed_upper(n_fixed);
	fixed_lower[0] = - inf; fixed_in[0] = 2.0; fixed_upper[0] = inf;
	fixed_lower[1] = .01;   fixed_in[1] = 0.5; fixed_upper[1] = inf;
	//
	// explicit constriants (in addition to l1 terms)
	vector<double> constraint_lower(0), constraint_upper(0);
	//
	vector<double> data(n_data), random_in(n_random);
	for(size_t i = 0; i < n_data; i++)
	{	data[i]       = double(i + 1);
		random_in[i] = i / double(n_data);
	}

	// object that is derived from approx_mixed
	approx_derived approx_object(n_fixed, n_random, data);
	approx_object.initialize(fixed_in, random_in);

	// optimize the fixed effects
	std::string options =
		"Integer print_level               0\n"
		"String  sb                        yes\n"
		"String  derivative_test           second-order\n"
		"String  derivative_test_print_all yes\n"
		"Numeric tol                       1e-8\n"
	;
	vector<double> fixed_out = approx_object.optimize_fixed(
		options,
		fixed_lower,
		fixed_upper,
		constraint_lower,
		constraint_upper,
		fixed_in,
		random_in
	);

	// results of optimization
	double theta_0 = fixed_out[0];
	double theta_1 = fixed_out[1];

	// compute partials of F
	double sum   = 0.0;
	double sumsq = 0.0;
	for(size_t i = 0; i < n_data; i++)
	{	sum   += theta_0 - data[i];
		sumsq += (theta_0 - data[i]) * (theta_0 - data[i]);
	}
	double den = 1.0 + theta_1 * theta_1;
	double F_0 = theta_0 - 4.0 + sum / den;
	double F_1 = theta_1 - 4.0;
	F_1       += double(n_data) * theta_1 / den;
	F_1       -= sumsq * theta_1  / (den * den);

	// Note that no constraints are active, (not even the l1 terms)
	// so the partials should be zero.
	ok &= CppAD::abs( F_0 ) <= tol;
	ok &= CppAD::abs( F_1 ) <= tol;

	return ok;
}
// END C++
