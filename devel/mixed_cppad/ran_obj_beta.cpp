// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/mixed_cppad.hpp>
/*
$begin mixed_cppad_ran_obj_beta$$
$spell
	cppad
	obj
	vec
	const
	Cpp
$$

$section Partial of Random Part of Objective w.r.t Fixed Effects$$

$head Syntax$$
$icode%H_beta% = %mixed_object%.ran_obj_beta(%beta%, %theta%, %u%)%$$

$head Purpose$$
This routine evaluates the partial w.r.t the fixed effects of the
random part of the objective; i.e.,
$latex \[
	H_\beta^{(1)} ( \beta , \theta , u )
\]$$
see
$cref/H(beta, theta, u)
	/mixed_cppad_theory
	/Differentiating Random Part of Objective
	/Approximate Random Objective, H(beta, theta, u)
/$$.

$head mixed_object$$
We use $cref/mixed_object/mixed_cppad_derived_ctor/mixed_object/$$
to denote an object of a class that is
derived from the $code mixed_cppad$$ base class.

$head beta$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %beta%
%$$
It specifies the value of the
$cref/fixed effects/mixed_cppad/Fixed Effects, theta/$$
vector $latex \beta$$ at which the partial is evaluated.

$head theta$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %theta%
%$$
It specifies the value of the
$cref/fixed effects/mixed_cppad/Fixed Effects, theta/$$
vector $latex \theta$$ at which the partial is evaluated.

$head u$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %u%
%$$
It specifies the value of the
$cref/random effects/mixed_cppad/Random Effects, u/$$
vector $latex u$$ at which the partial is evaluated.

$head H_beta$$
The return value has prototype
$codei%
	d_vector %H_beta%
%$$
and is the value of the partial derivative.

$children%
	example/devel/mixed_cppad/private/ran_obj_beta_xam.cpp
%$$
$head Example$$
The file $cref ran_obj_beta_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$end
*/
namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

mixed_cppad::d_vector mixed_cppad::ran_obj_beta(
	const d_vector& beta  ,
	const d_vector& theta ,
	const d_vector& u     )
{	assert( record_ran_obj_done_[1] );
	assert( ran_obj_1_.Domain() == 2 * n_fixed_ + n_random_ );
	assert( ran_obj_1_.Range() == 1 );

	// pack all the arguments into one vector.
	d_vector beta_theta_u(2 * n_fixed_ + n_random_);
	pack(beta, theta, u, beta_theta_u);

	// execute a zero order forward sweep
	ran_obj_1_.Forward(0, beta_theta_u);

	// compute the gradient H w.r.t (beta, theta, u)
	d_vector w(1);
	w[0] = 1.0;
	d_vector H_beta_theta_u = ran_obj_1_.Reverse(1, w);

	// extract H_beta
	d_vector H_beta(n_fixed_), H_theta(n_fixed_), H_u(n_random_);
	unpack(H_beta, H_theta, H_u, H_beta_theta_u);

	return H_beta;
}


} // END_DISMOD_AT_NAMESPACE
