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
$begin approx_mixed_laplace_beta$$
$spell
	vec
	const
	Cpp
$$

$section approx_mixed: Partial of Laplace Approximation w.r.t beta$$

$head Syntax$$
$icode%H_beta% = %approx_object%.laplace_beta(%beta%, %theta%, %u%)%$$

$head Purpose$$
This routine evaluates the partial w.r.t. $latex \beta$$ of the
joint part of the Laplace Approximation; i.e.,
$latex \[
	H_\beta^{(1)} ( \beta , \theta , u )
\]$$
See $cref/H(beta, theta, u)/approx_mixed_theory
	/Objective, Joint Part
	/H(beta, theta, u)
/$$.

$head approx_object$$
We use $cref/approx_object/approx_mixed_derived_ctor/approx_object/$$
to denote an object of a class that is
derived from the $code approx_mixed$$ base class.

$head beta$$
This argument has prototype
$codei%
	const CppAD::vector<a1_double>& %beta%
%$$
It specifies the value of the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \beta$$ at which the partial is evaluated.

$head theta$$
This argument has prototype
$codei%
	const CppAD::vector<a1_double>& %theta%
%$$
It specifies the value of the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$ at which the partial is evaluated.

$head u$$
This argument has prototype
$codei%
	const CppAD::vector<a1_double>& %u%
%$$
It specifies the value of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$ at which the partial is evaluated.

$head H_beta$$
The return value has prototype
$codei%
	a1d_double %H_beta%
%$$
and is the value of the partial derivative.

$children%
	example/devel/approx_mixed/laplace_beta_xam.cpp
%$$
$head Example$$
The file $cref laplace_beta_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$end
*/
# include <Eigen/Sparse>
# include <dismod_at/approx_mixed.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

approx_mixed::a1d_vector approx_mixed::laplace_beta(
	const a1d_vector& beta  ,
	const a1d_vector& theta ,
	const a1d_vector& u     )
{	assert( laplace_.Domain() == 2 * n_fixed_ + n_random_ );
	assert( laplace_.Range() == 1 );

	// evaluate the gradient of H(beta, theta, u)
	a1d_vector beta_theta_u(2 * n_fixed_ + n_random_);
	pack(beta, theta, u, beta_theta_u);

	// execute a zero order forward sweep
	laplace_.Forward(0, beta_theta_u);

	// compute the gradient H w.r.t (beta, theta, u)
	a1d_vector w(1);
	w[0] = a1_double(1.0);
	a1d_vector H_beta_theta_u = laplace_.Reverse(1, w);

	// extract H_beta
	a1d_vector H_beta(n_fixed_), H_theta(n_fixed_), H_u(n_random_);
	unpack(H_beta, H_theta, H_u, H_beta_theta_u);

	return H_beta;
}


} // END_DISMOD_AT_NAMESPACE
