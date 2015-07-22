// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/approx_mixed.hpp>
/*
$begin approx_mixed_joint_grad_ran$$
$spell
	vec
	const
	Cpp
	xam
$$

$section Gradient of Joint Negative Log-Likelihood With Respect to Random Effects$$

$head Syntax$$
$icode%grad% = %approx_object%.joint_grad_ran( %fixed_vec%, %random_vec%)%$$

$head Purpose$$
This routine computes the gradient of the negative log of the joint negative log-likelihood
$cref/f(theta, u)/approx_mixed_theory/Joint Negative Log-Likelihood, f(theta, u)/$$
with respect to the random effects vector $latex u$$; i.e.
$latex \[
	f_u^{(1)} ( \theta, u )
\] $$

$head approx_object$$
We use $cref/approx_object/approx_mixed_derived_ctor/approx_object/$$
to denote an object of a class that is
derived from the $code approx_mixed$$ base class.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<a3_double>& %fixed_vec%
%$$
It specifies the value of the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<a3_double>& %random_vec%
%$$
It specifies the value of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$.

$head grad$$
The return value has prototype
$codei%
	CppAD::vector<a3_double>& %grad%
%$$
It contains the gradient $latex f_u^{(1)} ( \theta , u )$$.

$children%
	example/devel/approx_mixed/private/joint_grad_ran_xam.cpp
%$$
$head Example$$
The file $cref joint_grad_ran_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// ----------------------------------------------------------------------------
// joint_grad_ran
CppAD::vector<approx_mixed::a3_double> approx_mixed::joint_grad_ran(
	const a3d_vector&        fixed_vec   ,
	const a3d_vector&        random_vec  )
{	assert( record_grad_ran_done_ );

	// number of fixed and random effects
	assert( n_fixed_  == fixed_vec.size() );
	assert( n_random_ == random_vec.size() );

	// create an a3d_vector containing (theta, u)
	a3d_vector both_vec( n_fixed_ + n_random_ );
	pack(fixed_vec, random_vec, both_vec);

	// compute the gradient
	size_t order = 0;
	return grad_ran_.Forward(order, both_vec);
}


} // END_DISMOD_AT_NAMESPACE

