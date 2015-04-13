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
$begin approx_mixed_record_laplace$$
$spell
	vec
	const
	Cpp
$$

$section approx_mixed: Record Joint Part of Laplace Approximation$$

$head Syntax$$
$codei%record_laplace(%fixed_vec%, %random_vec%)%$$

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

$head laplace_$$
The input value of the member variable
$codei%
	CppAD::ADFun<a1_double> laplace_
%$$
does not matter.
Upon return it contains the corresponding recording for the
joint part of the Laplace approximation objective,
$cref/h[theta, u^(theta)]/approx_mixed_theory
	/Objective, Joint Part
	/h[theta, u^(theta)]
/$$.

$end
*/
# include <Eigen/Sparse>
# include <dismod_at/approx_mixed.hpp>

namespace {
	typedef dismod_at::approx_mixed::a2d_vector a2d_vector;
	typedef dismod_at::approx_mixed::a3d_vector a3d_vector;

	a2d_vector a2_gradient_random(
		dismod_at::approx_mixed& object, a2d_vector& a2_beta, a2d_vector& a2_u
	)
	{
		a3d_vector a3_beta( a2_beta.size() ), a3_u( a2_u.size() );
		for(size_t j = 0; j < a2_beta.size(); j++)
			a3_beta[j] = a2_beta[j];
		for(size_t j = 0; j < a2_u.size(); j++)
			a3_u[j] = a2_u[j];
		a3d_vector a3_vec = object.gradient_random(a3_beta, a3_u);
		a2d_vector a2_vec( a3_vec.size() );
		for(size_t j = 0; j < a3_vec.size(); j++)
			a2_vec[j] = Value( a3_vec[j] );
		return a2_vec;
	}
	void a2_hessian_random(
		dismod_at::approx_mixed& object,
		a2d_vector& a2_beta,
		a2d_vector& a2_u,
		CppAD::vector<size_t>& row,
		CppAD::vector<size_t>& col,
		a2d_vector& a2_val
	)
	{
		a3d_vector a3_beta( a2_beta.size() ), 
			a3_u( a2_u.size() ), a3_val( a2_val.size() );
		for(size_t j = 0; j < a2_beta.size(); j++)
			a3_beta[j] = a2_beta[j];
		for(size_t j = 0; j < a2_u.size(); j++)
			a3_u[j] = a2_u[j];
		object.hessian_random(a3_beta, a3_u, row, col, a3_val);
		a2_val.resize( a3_val.size() );
		for(size_t j = 0; j < a3_val.size(); j++)
			a2_val[j] = Value( a3_val[j] );
		return;
	}
  
}

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void approx_mixed::record_laplace(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{
	//	create an a2d_vector containing (beta, theta, u)
	a2d_vector beta_theta_u( 2 * n_fixed_ + n_random_ );
	pack(fixed_vec, fixed_vec, random_vec, beta_theta_u);

	// start recording a2_double operations
	CppAD::Independent( beta_theta_u );

	// split back out to beta, theta, u
	a2d_vector beta(n_fixed_), theta(n_fixed_), u(n_random_);
	unpack(beta, theta, u, beta_theta_u);

	// evaluate gradient f_u^{(1)} (beta , u )
	a2d_vector grad = a2_gradient_random(*this, beta, u);

	// evaluate the hessian f_{uu}^{(2)} (theta, u)
	CppAD::vector<size_t> row, col;
	a2d_vector val;
	a2_hessian_random(*this, theta, u, row, col, val);

	// declare eigen matrix types
	using Eigen::Dynamic;
	typedef Eigen::Matrix<a2_double, Dynamic, Dynamic> dense_matrix;
	typedef Eigen::SparseMatrix<a2_double>             sparse_matrix;

	// create a lower triangular eigen sparse matrix representation of Hessian
	sparse_matrix hessian(n_random_, n_random_);
	size_t K = row.size();
	for(size_t k = 0; k < K; k++)
		hessian.insert(row[k], col[k]) = val[k];

	// compute an LDL^T Cholesky factorization of f_{uu}^{(2)}(theta, u)
	Eigen::SimplicialLDLT<sparse_matrix, Eigen::Lower> chol;
	chol.analyzePattern(hessian);
	chol.factorize(hessian);

	// newton_step = f_{uu}^{(2)} (theta , u)^{-1} f_u^{(1)} (beta, u)
	dense_matrix rhs(n_random_, 1);
	for(size_t j = 0; j < n_random_; j++)
		rhs(j) = grad[j];
	dense_matrix newton_step = chol.solve(rhs);

	// compute U(beta, theta, u)
	a2d_vector U(n_random_);
	for(size_t j = 0; j < n_random_; j++)
		U[j] = u[j] - newton_step(j);

	// evaluate hessian f_{uu}^{(2)}(beta, U)
	a2_hessian_random(*this, beta, U, row, col, val);

	// compute an LDL^T Cholesky factorization of f_{uu}^{(2)}(beta, U)
	for(size_t k = 0; k < K; k++)
		hessian.coeffRef(row[k], col[k]) = val[k];
	chol.factorize(hessian);

	// logdet f_{uu}^{(2)} [beta, U(beta, theta, u)]
	dense_matrix diag = chol.vectorD();
	a2_double logdet = a2_double(0.0);
	for(size_t j = 0; j < n_random_; j++)
		logdet += log( diag(j) );

	// f[beta, U(beta, theta, u)]
	a2d_vector both(n_fixed_ + n_random_);
	pack(beta, U, both);
	a2d_vector vec = a2_joint_density_.Forward(0, both);
	a2_double sum = vec[0];
	size_t n_abs = vec.size() - 1;
	for(size_t i = 0; i < n_abs; i++)
		sum += abs( vec[1 + i] );

	// constant term
	double pi   = CppAD::atan(1.0) * 4.0;
	double constant_term = CppAD::log(2.0 * pi) * double(n_random_) / 2.0;

	// H(beta, theta, u)
	a2d_vector H(1);
	H[0] = logdet / 2.0 + sum - constant_term;


	// complete recording of H(beta, theta, u)
	laplace_.Dependent(beta_theta_u, H);

	// optimize the recording
	laplace_.optimize();
}


} // END_DISMOD_AT_NAMESPACE
