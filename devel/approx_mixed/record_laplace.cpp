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
$begin approx_mixed_record_laplace$$
$spell
	vec
	const
	Cpp
$$

$section approx_mixed: Record Random Part of Laplace Approximation$$

$head Syntax$$
$codei%record_laplace(%order%, %fixed_vec%, %random_vec%)%$$

$head Private$$
This function is $code private$$ to the $code approx_mixed$$ class
and cannot be used by a derived
$cref/approx_object/approx_mixed_derived_ctor/approx_object/$$.

$head order$$
This argument has prototype
$icode%
	size_t %order%
%$$
It specifies the order of accuracy when approximating
$cref/u^(theta)/approx_mixed_theory/Objective/u^(theta)/$$
and must be zero, one, or two.
The zero order recording is shortest, but can only compute
$cref/G(theta)/approx_mixed_theory/Objective/G(theta)/$$.
The first order recording is longer, and can compute
both $latex G( \theta )$$ and its derivative.
The second order recording is longest, and can compute
both $latex G( \theta )$$ and its derivative and its Hessian.

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

$head laplace_order_$$
The input value of the member variable
$codei%
	CppAD::ADFun<a2_double> laplace_%order%_
%$$
does not matter.
Upon return it contains a an recording for the
random part of the Laplace approximation objective; see
$cref/derivatives of G/approx_mixed_theory
	/Random Part of Objective
	/Derivatives of G
/$$.

$end
*/
# include <Eigen/Sparse>
# include <dismod_at/approx_mixed.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void approx_mixed::record_laplace(
	size_t          order      ,
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	assert( order <= 2 );
	assert( ! record_laplace_done_[order] );

	// declare eigen matrix types
	using Eigen::Dynamic;
	typedef Eigen::Matrix<a3_double, Dynamic, Dynamic> dense_matrix;
	typedef Eigen::SparseMatrix<a3_double>             sparse_matrix;

	//	create an a3d_vector containing (beta, theta, u)
	a3d_vector beta_theta_u( 2 * n_fixed_ + n_random_ );
	pack(fixed_vec, fixed_vec, random_vec, beta_theta_u);

	// start recording a3_double operations
	CppAD::Independent( beta_theta_u );

	// split back out to beta, theta, u
	a3d_vector beta(n_fixed_), theta(n_fixed_), u(n_random_);
	unpack(beta, theta, u, beta_theta_u);

	// evaluate the hessian f_{uu}^{(2)} (theta, u)
	CppAD::vector<size_t> row, col;
	a3d_vector val;
	ran_like_hes(theta, u, row, col, val);

	// create a lower triangular eigen sparse matrix representation of Hessian
	sparse_matrix hessian(n_random_, n_random_);
	size_t K = row.size();
	for(size_t k = 0; k < K; k++)
		hessian.insert(row[k], col[k]) = val[k];

	// compute an LDL^T Cholesky factorization of f_{uu}^{(2)}(theta, u)
	Eigen::SimplicialLDLT<sparse_matrix, Eigen::Lower> chol;
	chol.analyzePattern(hessian);
	chol.factorize(hessian);

	// First order section -------------------------------------------------
	a3d_vector U(n_random_), grad(n_random_);
	dense_matrix rhs(n_random_, 1), newton_step(n_random_, 1);
	if( order < 1 )
	{	for(size_t j = 0; j < n_random_; j++)
			U[j] = u[j];
	}
	else
	{	// evaluate gradient f_u^{(1)} (beta , u )
		grad = ran_like_grad(beta, u);

		// newton_step = f_{uu}^{(2)} (theta , u)^{-1} f_u^{(1)} (beta, u)
		for(size_t j = 0; j < n_random_; j++)
			rhs(j) = grad[j];
		newton_step = chol.solve(rhs);

		// compute U(beta, theta, u)
		for(size_t j = 0; j < n_random_; j++)
			U[j] = u[j] - newton_step(j);

		// evaluate hessian f_{uu}^{(2)}(beta, U) and compute its factorization
		ran_like_hes(beta, U, row, col, val);
		for(size_t k = 0; k < K; k++)
			hessian.coeffRef(row[k], col[k]) = val[k];
		chol.factorize(hessian);
	}
	// Second order section --------------------------------------------------
	a3d_vector W(n_random_);
	if( order < 1 )
	{	for(size_t j = 0; j < n_random_; j++)
			W[j] = U[j];
	}
	else
	{	// evaluate gradient f_u^{(1)} (beta , U )
		grad = ran_like_grad(beta, U);

		// newton_step = f_{uu}^{(2)} (beta , U)^{-1} f_u^{(1)} (beta, U)
		for(size_t j = 0; j < n_random_; j++)
			rhs(j) = grad[j];
		newton_step = chol.solve(rhs);

		// compute W(beta, theta, u)
		for(size_t j = 0; j < n_random_; j++)
			W[j] = U[j] - newton_step(j);

		// evaluate hessian f_{uu}^{(2)}(beta, W) and compute its factorization
		ran_like_hes(beta, W, row, col, val);
		for(size_t k = 0; k < K; k++)
			hessian.coeffRef(row[k], col[k]) = val[k];
		chol.factorize(hessian);
	}
	// Back to all orders ----------------------------------------------------
	//
	// logdet f_{uu}^{(2)} (beta, W)]
	dense_matrix diag = chol.vectorD();
	a3_double logdet = a3_double(0.0);
	for(size_t j = 0; j < n_random_; j++)
		logdet += log( diag(j) );

	// f[beta, W(beta, theta, u)]
	a3d_vector both(n_fixed_ + n_random_);
	pack(beta, W, both);
	a3d_vector vec = a3_ran_like_.Forward(0, both);
	a3_double sum = vec[0];
	size_t n_abs = vec.size() - 1;
	for(size_t i = 0; i < n_abs; i++)
		sum += abs( vec[1 + i] );

	// constant term
	double pi   = CppAD::atan(1.0) * 4.0;
	double constant_term = CppAD::log(2.0 * pi) * double(n_random_) / 2.0;

	// H(beta, theta, u)
	a3d_vector H(1);
	H[0] = logdet / 2.0 + sum - constant_term;

	// complete recording of H(beta, theta, u)
	if( order <= 1 )
	{	CppAD::ADFun<a2_double> a2f(beta_theta_u, H);
		//
		a1d_vector a1_beta_theta_u( 2 * n_fixed_ + n_random_), a1_H(1);
		a2d_vector a2_beta_theta_u( 2 * n_fixed_ + n_random_), a2_H(1);
		//
		pack(fixed_vec, fixed_vec, random_vec, a1_beta_theta_u);
		CppAD::Independent(a1_beta_theta_u);
		for(size_t j = 0; j < 2 * n_fixed_ + n_random_; j++)
			a2_beta_theta_u[j] = a1_beta_theta_u[j];
		a2_H     = a2f.Forward(0, a2_beta_theta_u);
		a1_H[0]  = Value( a2_H[0] );
		if( order == 0 )
		{	laplace_0_.Dependent(a1_beta_theta_u, a1_H);
			laplace_0_.optimize();
		}
		else
		{	assert( order == 1 );
			laplace_1_.Dependent(a1_beta_theta_u, a1_H);
			laplace_1_.optimize();
		}
	}
	else
	{	assert(order == 2 );
		laplace_2_.Dependent(beta_theta_u, H);
		laplace_2_.optimize();
	}
	record_laplace_done_[order] = true;
}

} // END_DISMOD_AT_NAMESPACE
