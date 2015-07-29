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
$begin approx_mixed_record_grad_ran$$
$spell
	vec
	const
	Cpp
$$

$section approx_mixed: Record Gradient of Joint Negative Log-Likelihood w.r.t Random Effects$$

$head Syntax$$
$codei%record_grad_ran(%fixed_vec%, %random_vec%)%$$

$head Private$$
This function is $code private$$ to the $code approx_mixed$$ class
and cannot be used by a derived
$cref/approx_object/approx_mixed_derived_ctor/approx_object/$$.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_vec%
%$$
It specifies the value of the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$ at which the recording is made.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %random_vec%
%$$
It specifies the value of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$ at which the recording is made.

$head gradient_$$
The input value of the member variable
$codei%
	CppAD::ADFun<a3_double> grad_ran_
%$$
does not matter.
Upon return it contains the corresponding recording for the gradient
$latex f_u^{(1)} ( \theta , u )$$.


$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void approx_mixed::record_grad_ran(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	assert( ! record_grad_ran_done_ );

	//	create an a4d_vector containing (theta, u)
	a4d_vector a4_both( n_fixed_ + n_random_ );
	pack(fixed_vec, random_vec, a4_both);

	// start recording f_uu (theta, u) using a4_double operations
	CppAD::Independent( a4_both );

	// create an a5d_vector containing theta and u
	a5d_vector a5_theta(n_fixed_), a5_u(n_random_);
	unpack(a5_theta, a5_u, a4_both);

	// compute f(u) using a5_double operations
	CppAD::Independent(a5_u);
	//
	a5d_vector a5_both(n_fixed_ + n_random_);
	a5d_vector a5_vec = ran_like(a5_theta, a5_u);
	a5d_vector a5_sum(1);
	a5_sum[0]    = a5_vec[0];
	size_t n_abs = a5_vec.size() - 1;
	for(size_t i = 0; i < n_abs; i++)
		a5_sum[0] += abs( a5_vec[1 + i] );
	CppAD::ADFun<a4_double> a4_f;
	a4_f.Dependent(a5_u, a5_sum);

	// zero order forward mode
	a4d_vector a4_theta(n_fixed_), a4_u(n_random_);
	unpack(a4_theta, a4_u, a4_both);
	a4_f.Forward(0, a4_u);

	// first order reverse
	a4d_vector a4_grad(n_random_), a4_w(1);
	a4_w[0] = a4_double( 1.0 );
	a4_grad = a4_f.Reverse(1, a4_w);

	// complete recording of f_u^{(1)} (u, theta)
	grad_ran_.Dependent(a4_both, a4_grad);

	// optimize the recording
	grad_ran_.optimize();
	//
	record_grad_ran_done_ = true;
}


} // END_DISMOD_AT_NAMESPACE
