// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin mixed_cppad_record_ran_obj$$
$spell
	cppad
	obj
	vec
	const
	Cpp
$$

$section Record Approximation for Random Part of Objective$$

$head Syntax$$
$codei%record_ran_obj(%order%, %fixed_vec%, %random_vec%)%$$

$head Private$$
This function is $code private$$ to the $code mixed_cppad$$ class
and cannot be used by a derived
$cref/mixed_object/mixed_cppad_derived_ctor/mixed_object/$$.

$head order$$
This argument has prototype
$icode%
	size_t %order%
%$$
It specifies the order of accuracy when approximating
$cref/u^(theta)/mixed_cppad_theory/Objective/u^(theta)/$$
and must be zero, one, or two.
The zero order recording is shortest, but can only compute
$cref/r(theta)
	/mixed_cppad_theory
	/Objective
	/Random Part of Objective, r(theta)
/$$.
The first order recording is longer, and can compute
both $latex r( \theta )$$ and its derivative.
The second order recording is longest, and can compute
$latex r( \theta )$$, and its derivative, and its Hessian.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_vec%
%$$
It specifies the value of the
$cref/fixed effects/mixed_cppad/Fixed Effects, theta/$$
vector $latex \theta$$ at which the recording is made.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %random_vec%
%$$
It specifies the value of the
$cref/random effects/mixed_cppad/Random Effects, u/$$
vector $latex u$$ at which the recording is made.

$head ran_obj_order_$$
The input value of the member variable
$codei%
	CppAD::ADFun<double> ran_obj_%order%_
%$$
does not matter.
Upon return it contains a an recording for the
approximate random part of the objective; see
$cref/H(beta, theta, u)
	/mixed_cppad_theory
	/Differentiating Random Part of Objective
	/Approximate Random Objective, H(beta, theta, u)
/$$.

$end
*/
# include <Eigen/Sparse>
# include <dismod_at/mixed_cppad.hpp>


namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// ----------------------------------------------------------------------------
void mixed_cppad::record_ran_obj(
	size_t          order      ,
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	assert( order == 2 );
	assert( ! record_ran_obj_done_ );
	assert( record_newton_atom_done_ );

	//	create an a1d_vector containing (beta, theta, u)
	a1d_vector beta_theta_u( 2 * n_fixed_ + n_random_ );
	pack(fixed_vec, fixed_vec, random_vec, beta_theta_u);

	// start recording a1_double operations
	CppAD::Independent( beta_theta_u );

	// split back out to beta, theta, u
	a1d_vector beta(n_fixed_), theta(n_fixed_), u(n_random_);
	unpack(beta, theta, u, beta_theta_u);

	// evaluate gradient f_u^{(1)} (beta , u )
	a1d_vector grad(n_random_);
	grad = ran_like_grad(beta, u);

	// Evaluate the log determinant of f_{uu}^{(2)} ( theta , u)
	// and Newton step s = f_{uu}^{(2)} ( theta , u) f_u^{(1)} (beta, u)
	a1d_vector theta_u_v(n_fixed_ + 2 * n_random_ );
	for(size_t j = 0; j < n_fixed_; j++)
		theta_u_v[j] = theta[j];
	for(size_t j = 0; j < n_random_; j++)
	{	theta_u_v[n_fixed_ + j]             = u[j];
		theta_u_v[n_fixed_ + n_random_ + j] = grad[j];
	}
	a1d_vector logdet_step(1 + n_random_);
	newton_atom_.eval(theta_u_v, logdet_step);
	//
	// constant term
	double pi   = CppAD::atan(1.0) * 4.0;
	double constant_term = CppAD::log(2.0 * pi) * double(n_random_) / 2.0;
	//
	a1d_vector both(n_fixed_ + n_random_), f(1), H(1);
	// -----------------------------------------------------------------------
	// U(beta, theta, u)
	a1d_vector U(n_random_);
	for(size_t j = 0; j < n_random_; j++)
		U[j] = u[j] - logdet_step[1 + j];

	// evaluate gradient f_u^{(1)} (beta , U )
	grad = ran_like_grad(beta, U);

	// Evaluate the log determinant and newton step
	a1d_vector beta_U_v(n_fixed_ + 2 * n_random_ );
	for(size_t j = 0; j < n_fixed_; j++)
		beta_U_v[j] = beta[j];
	for(size_t j = 0; j < n_random_; j++)
	{	beta_U_v[n_fixed_ + j]             = U[j];
		beta_U_v[n_fixed_ + n_random_ + j] = grad[j];
	}
	newton_atom_.eval(beta_U_v, logdet_step);
	// -----------------------------------------------------------------------
	// W(beta, theta, u)
	a1d_vector W(n_random_);
	for(size_t j = 0; j < n_random_; j++)
		W[j] = U[j] - logdet_step[1 + j];

	// Evaluate the log determinant
	a1d_vector beta_W_v(n_fixed_ + 2 * n_random_ );
	for(size_t j = 0; j < n_fixed_; j++)
		beta_W_v[j] = beta[j];
	for(size_t j = 0; j < n_random_; j++)
	{	beta_W_v[n_fixed_ + j]             = W[j];
		beta_W_v[n_fixed_ + n_random_ + j] = 0.0;
	}
	newton_atom_.eval(beta_W_v, logdet_step);
	pack(beta, W, both);
	f    = a1_ran_like_.Forward(0, both);
	H[0] = logdet_step[0] / 2.0 + f[0] - constant_term;
	//
	ran_obj_2_.Dependent(beta_theta_u, H);
	ran_obj_2_.optimize();
	//
	record_ran_obj_done_ = true;
	return;
}

} // END_DISMOD_AT_NAMESPACE
