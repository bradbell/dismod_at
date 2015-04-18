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
	xam
	vec
	const
	CppAD
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

$head fixed_in$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_in%
%$$
It specifies the initial value for the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$ during the optimization process.

$head fixed_upper$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_upper%
%$$
It specifies the upper bound for the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$.

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
	%fixed_lower%[%j%]% <= %fixed_out%[%j%] <= %fixed_upper%[%j%]
%$$
for $icode%j% = 0 , %...%, %n_fixed_%-1%$$.

$head Laplace Approximation$$
The $cref/theory/approx_mixed_theory/$$ for the
Laplace approximation optimization only includes the case where
the $cref/joint density/approx_mixed_joint_density/$$ is smooth.

$comment%
	example/devel/approx_mixed/optimize_fixed_xam.cpp
%$$

$head Example$$
The file $code optimize_fixed_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.
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

	// Create an instance of the example problem
	SmartPtr<ipopt_fixed> xam_nlp = new ipopt_fixed(
		fixed_lower, fixed_in, fixed_upper, random_in
	);

	// Create an instance of an IpoptApplication
	SmartPtr<Ipopt::IpoptApplication> app = IpoptApplicationFactory();

	// Turn off all Ipopt printed output
	app->Options()->SetIntegerValue("print_level", 0);
	app->Options()->SetStringValue("sb", "yes");

	// variable to hold status values returned by app
	Ipopt::ApplicationReturnStatus status;

	// initialize app
	status = app->Initialize();
	ok    &= status == Ipopt::Solve_Succeeded;

	// solve the problem
	status = app->OptimizeTNLP(xam_nlp);
	ok    &= status == Ipopt::Solve_Succeeded;
	ok    &= xam_nlp->finalize_solution_ok_;

	d_vector fixed_out;
	return fixed_out;
}

} // END_DISMOD_AT_NAMESPACE
