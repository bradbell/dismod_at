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
$begin no_random_xam.cpp$$
$spell
	hes
	eval
	interp
	xam
$$

$section C++ approx_mixed: User Example and Test with no Random Effects$$.

$head Model$$
$latex \[
	\B{p}( z_i | \theta ) \sim \B{N} ( \theta_i , 1 )
\] $$
$latex \[
	\B{p}( \theta_i ) \sim \B{N} ( 0 , 1 )
\] $$
The corresponding prior negative log-likelihood
$cref/g(theta)/approx_mixed_theory/Prior Negative Log-Likelihood, g(theta)/$$
is
$latex \[
g( \theta ) = \frac{1}{2} \sum_{i} \left[
	\log ( 2 \pi ) + \theta_i^2
	+
	\log ( 2 \pi ) + ( z_i - \theta_i )^2
\right]
\] $$
The optimal solution (with no constraints) is
$latex \[
	\hat{\theta}_i = z_i / 2
\] $$

$code
$verbatim%example/devel/approx_mixed/user/no_random_xam.cpp
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
		const vector<double>& z_;
	public:
		// constructor
		approx_derived(
			size_t n_fixed                    ,
			size_t n_random                   ,
			const vector<double>& z           ) :
			dismod_at::approx_mixed(n_fixed, n_random) ,
			n_fixed_(n_fixed)                          ,
			z_(z)
		{	assert(z.size() == n_fixed); }
	private:
		// implementation of prior_like as p(z|theta) * p(theta)
		template <class Float>
		vector<Float> implement_prior_like(
			const vector<Float>& fixed_vec  )
		{
			// initialize log-density
			vector<Float> vec(1);
			vec[0] = Float(0.0);

			// compute this factors once
			Float sqrt_2pi = Float( CppAD::sqrt( 8.0 * CppAD::atan(1.0) ) );

			for(size_t j = 0; j < n_fixed_; j++)
			{
				// Data term
				Float res  = (z_[j] - fixed_vec[j]);
				vec[0]    += log(sqrt_2pi ) + res * res / Float(2.0);

				// True prior term
				res     = fixed_vec[j];
				vec[0] += log(sqrt_2pi) + res * res / Float(2.0);
			}
			return vec;
		}
	public:
		// ------------------------------------------------------------------
		// User defined virtual functions
		virtual vector<a5_double> joint_like(
			const vector<a5_double>& fixed_vec  ,
			const vector<a5_double>& random_vec )
		{	return vector<a5_double>(0); } // empty vector
		//
		virtual vector<a1_double> prior_like(
			const vector<a1_double>& fixed_vec  )
		{	return implement_prior_like(fixed_vec); }
		//
		virtual vector<a1_double> constraint(
			const vector<a1_double>& fixed_vec  )
		{	return vector<a1_double>(0); } // empty vector
		//
		virtual void fatal_error(const std::string& error_message)
		{	std::cerr << "Error: " << error_message << std::endl;
			assert(false);
		}
		//
		virtual void warning(const std::string& warning_message)
		{	std::cerr << "Warning: " << warning_message << std::endl;
			assert(false);
		}
		// ------------------------------------------------------------------
	};
}

bool no_random_xam(void)
{
	bool   ok = true;
	double inf = std::numeric_limits<double>::infinity();
	double tol = 1e-8;

	// fixed effects
	size_t n_fixed  = 3;
	vector<double>
		fixed_lower(n_fixed), fixed_in(n_fixed), fixed_upper(n_fixed);
	for(size_t j = 0; j < n_fixed; j++)
	{	fixed_lower[j] = - inf;
		fixed_in[j]    = 0.0;
		fixed_upper[j] = inf;
	}
	//
	// no random effects
	size_t n_random = 0;
	vector<double> random_in(0);
	//
	// no constriants
	vector<double> constraint_lower(0), constraint_upper(0);
	//
	vector<double> z(n_fixed);
	for(size_t i = 0; i < n_fixed; i++)
		z[i] = double(i+1);

	// object that is derived from approx_mixed
	approx_derived approx_object(n_fixed, n_random, z);
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

	for(size_t j = 0; j < n_fixed; j++)
		ok &= CppAD::abs( fixed_out[j] - z[j] / 2.0 ) <= tol;

	return ok;
}
// END C++
