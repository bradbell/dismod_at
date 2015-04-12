// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/approx_mixed.hpp>
# include <coin/IpIpoptApplication.hpp>
# include <coin/IpTNLP.hpp>
# include <cppad/ipopt/solve_result.hpp>

/*
$begin approx_mixed_optimize_fixed$$
$spell
	vec
	const
	CppAD
$$

$section Optimize Fixed Effects$$

$head Syntax$$
$icode%fixed_out% =%$$
$icode%approx_object%.optimize_fixed(%fixed_in%, %random_in%)%$$

$head Purpose$$
This routine maximizes the Laplace approximation for the
negative log-likelihood
$cref/L(theta)/approx_mixed_theory/Objective, L(theta)/$$
$latex \[
L( \theta ) \approx - \log \left[
	\int \B{p} ( y | \theta , u) \B{p} ( u | \theta ) \B{p} ( \theta ) \R{d} u
\right]
\] $$

$head approx_object$$
We use $cref/approx_object/approx_mixed_derived_ctor/approx_object/$$
to denote an object of a class that is
derived from the $code approx_mixed$$ base class.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_in%
%$$
It specifies the initial value used for the optimization of the
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
Thre return value has prototype
$codei%
	CppAD::vector<double> %fixed_out%
%$$
It is the final value (obtained by optimization) of the
fixed effects vector $latex \theta$$.

$head Laplace Approximation$$
The $cref/theory/approx_mixed_throey/$$ for the
Laplace approximation optimization only includes the case where
the $cref/joint density/approx_mixed_joint_density/$$ is smooth.

$children%
	example/devel/approx_mixed/optimize_fixed_xam.cpp
%$$
$head Example$$
The file $cref optimize_fixed_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.
$end
*/
// ----------------------------------------------------------------------------
namespace { // BEGIN_EMPTY_NAMESPACE

// types used by this file
typedef Ipopt::Number                Number;
typedef Ipopt::Index                 Index;
typedef Ipopt::TNLP::IndexStyleEnum  IndexStyleEnum;

// ----------------------------------------------------------------------------
// derived from the Ipopt base class for non-linear programming problems.
class ipopt_nlp_fixed : public Ipopt::TNLP { // BEGIN_IPOPT_NLP_FIXED


}; // END_IPOPT_NLP_FIXED
}  // END_EMPTY_NAMESPACE
// ----------------------------------------------------------------------------


