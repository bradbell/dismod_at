// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin approx_mixed_optimize_fixed$$
$spell
	ipopt
	xam
	vec
	const
	CppAD
	std
$$

$section Optimize Fixed Effects$$

$head Syntax$$
$icode%fixed_out% =%$$
$icode%approx_object%.optimize_fixed(
	%fixed_lower%, %fixed_in%, %fixed_upper%, %random_in%
)%$$

$head Purpose$$
This routine maximizes the Laplace approximation for the
negative log-likelihood
$cref/L(theta)/approx_mixed_theory/Objective/L(theta)/$$
$latex \[
L( \theta ) \approx - \log \left[
	\int \B{p} ( y | \theta , u) \B{p} ( u | \theta ) \B{p} ( \theta ) \R{d} u
\right]
\] $$

$head approx_object$$
We use $cref/approx_object/approx_mixed_derived_ctor/approx_object/$$
to denote an object of a class that is
derived from the $code approx_mixed$$ base class.

$head fixed_lower$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_lower%
%$$
It specifies the lower bound for the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$.
Note that
$code%
	- std::numeric_limits<double>::infinity()
%$$
is used for minus infinity; i.e., no lower bound.

$head fixed_in$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_in%
%$$
It specifies the initial value for the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$ during the optimization process.
It must hold for each $icode j$$ that
$codei%
	%fixed_lower%[%j%] <= %fixed_in%[%j%] <= %fixed_upper%[%j%]
%$$

$head fixed_upper$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_upper%
%$$
It specifies the upper bound for the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$.
Note that
$code%
	std::numeric_limits<double>::infinity()
%$$
is used for plus infinity; i.e., no upper bound.

$head random_in$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %random_in%
%$$
It specifies the initial value used for the optimization of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$.

$head fixed_out$$
The return value has prototype
$codei%
	CppAD::vector<double> %fixed_out%
%$$
It is the final value (obtained by optimization) of the
fixed effects vector.
This vector satisfies its bounds; i.e.,
$codei%
	%fixed_lower%[%j%] <= %fixed_out%[%j%] <= %fixed_upper%[%j%]
%$$
for $icode%j% = 0 , %...%, %n_fixed_%-1%$$.

$head Laplace Approximation$$
The $cref/theory/approx_mixed_theory/$$ for the
Laplace approximation optimization only includes the case where
the $cref/joint density/approx_mixed_joint_density/$$ is smooth.

$children%example/devel/approx_mixed/optimize_fixed_xam.cpp
	%include/dismod_at/ipopt_fixed.hpp
%$$

$head Example$$
The file $cref optimize_fixed_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$head ipopt_fixed$$
The  class $cref ipopt_fixed$$ is used by $code optimize_fixed$$
to optimize the fixed effects.
It's specifications are not part of the $cref approx_mixed$$ public interface.

$end
------------------------------------------------------------------------------
*/
# include <coin/IpIpoptApplication.hpp>
# include <dismod_at/approx_mixed.hpp>
# include <dismod_at/ipopt_fixed.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

CppAD::vector<double> approx_mixed::optimize_fixed(
	const d_vector& fixed_lower ,
	const d_vector& fixed_in    ,
	const d_vector& fixed_upper ,
	const d_vector& random_in   )
{	bool ok = true;
	using Ipopt::SmartPtr;

# ifndef NDEBUG
	assert( fixed_lower.size() == fixed_in.size() );
	assert( fixed_lower.size() == fixed_upper.size() );
	for(size_t j = 0; j < fixed_lower.size(); j++)
	{	assert( fixed_lower[j] <= fixed_in[j] );
		assert( fixed_in[j]    <= fixed_upper[j] );
	}
# endif

	// create a reference to this object
	approx_mixed& approx_object(*this);

	// Create an instance of the problem
	SmartPtr<ipopt_fixed> fixed_nlp = new ipopt_fixed(
		fixed_lower, fixed_in, fixed_upper, random_in, approx_object
	);

	// Create an instance of an IpoptApplication
	SmartPtr<Ipopt::IpoptApplication> app = IpoptApplicationFactory();

	// Set options
	app->Options()->SetIntegerValue("print_level", 0); // default is 5
	app->Options()->SetStringValue("sb", "yes");
	app->Options()->SetStringValue("derivative_test", "second-order");
	app->Options()->SetStringValue("derivative_test_print_all", "yes");

	// Set values used for minus and plus infinity
	app->Options()->SetNumericValue(
		"nlp_lower_bound_inf", fixed_nlp->nlp_lower_bound_inf()
	);

	// variable to hold status values returned by app
	Ipopt::ApplicationReturnStatus status;

	// initialize app
	status = app->Initialize();
	ok    &= status == Ipopt::Solve_Succeeded;
	if( ! ok )
	{	std::cerr << "optimize_fixed: initalization failed" << std::endl;
		exit(1);
	}

	// solve the problem
	status = app->OptimizeTNLP(fixed_nlp);
	ok    &= status == Ipopt::Solve_Succeeded;
	if( ! ok )
	{	std::cerr << "optimize_fixed: ipopt failed to converge" << std::endl;
		exit(1);
	}
	ok    &= fixed_nlp->finalize_solution_ok_;
	if( ! ok )
	{	std::cerr << "optimize_fixed: solution check failed" << std::endl;
		exit(1);
	}
	//
	return fixed_nlp->fixed_opt();
}

} // END_DISMOD_AT_NAMESPACE
