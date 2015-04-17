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

/*
$begin approx_mixed_laplace_eval$$
$spell
	eval
	vec
	const
	Cpp
$$

$section approx_mixed: Evaluate Joint Part of the Laplace Approximation$$

$head Syntax$$
$icode%H% = %approx_object%.laplace_eval(%beta%, %theta%, %u%)%$$

$head Purpose$$
This routine evaluates the joint part of the Laplace approximation objective
$cref/H(beta, theta, u)/approx_mixed_theory
	/Joint Part of Objective
	/H(beta, theta, u)
/$$.

$head approx_object$$
We use $cref/approx_object/approx_mixed_derived_ctor/approx_object/$$
to denote an object of a class that is
derived from the $code approx_mixed$$ base class.

$head beta$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %beta%
%$$
It specifies the value of the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \beta$$ at which $latex H( \beta , \theta , u)$$ is evaluated.

$head theta$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %theta%
%$$
It specifies the value of the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$ at which $latex H( \beta , \theta , u)$$ is evaluated.

$head u$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %u%
%$$
It specifies the value of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$ at which $latex H( \beta , \theta , u)$$ is evaluated.

$head H$$
The return value has prototype
$codei%
	double %H%
%$$
and is the value of the joint part of the Laplace approximation
$cref/H(beta, theta, u)/approx_mixed_theory
	/Joint Part of Objective
	/H(beta, theta, u)
/$$.

$children%
	example/devel/approx_mixed/laplace_eval_xam.cpp
%$$
$head Example$$
The file $cref laplace_eval_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

double approx_mixed::laplace_eval(
	const d_vector& beta  ,
	const d_vector& theta ,
	const d_vector& u     )
{	assert( laplace_0_.Domain() == 2 * n_fixed_ + n_random_ );
	assert( laplace_0_.Range() == 1 );

	// evaluate H(beta, theta, u)
	d_vector beta_theta_u(2 * n_fixed_ + n_random_);
	pack(beta, theta, u, beta_theta_u);
	d_vector H = laplace_0_.Forward(0, beta_theta_u);

	return H[0];
}


} // END_DISMOD_AT_NAMESPACE
