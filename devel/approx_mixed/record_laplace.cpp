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
	CppAD::ADFun<a2_double> laplace_
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

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void approx_mixed::record_laplace(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{
	//	create an a3d_vector containing (beta, theta, u)
	a3d_vector beta_theta_u( 2 * n_fixed_ + n_random_ );
	pack(fixed_vec, fixed_vec, random_vec, beta_theta_u);

	// start recording a3_double operations
	CppAD::Independent( beta_theta_u );

	// split back out to beta, theta, u
	a3d_vector beta(n_fixed_), theta(n_fixed_), u(n_random_);
	unpack(beta, theta, u, beta_theta_u);

	// evaluate gradient f_u^{(1)} (beta , u )
	a3d_vector grad = gradient_random(beta, u);

	// evaluate the hessian f_{uu}^{(2)} (theta, u)
	CppAD::vector<size_t> row, col;
	a3d_vector val;
	hessian_random(theta, u, row, col, val);

	// declare eigen matrix types
	using Eigen::Dynamic;
	typedef Eigen::Matrix<a3_double, Dynamic, Dynamic> dense_matrix;
	typedef Eigen::SparseMatrix<a3_double>             sparse_matrix;

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
	a3d_vector U(n_random_);
	for(size_t j = 0; j < n_random_; j++)
		U[j] = u[j] - newton_step(j);

	// evaluate hessian f_{uu}^{(2)}(beta, U)
	hessian_random(beta, U, row, col, val);

	// compute an LDL^T Cholesky factorization of f_{uu}^{(2)}(beta, U)
	for(size_t k = 0; k < K; k++)
		hessian.coeffRef(row[k], col[k]) = val[k];
	chol.factorize(hessian);

	// logdet f_{uu}^{(2)} [beta, U(beta, theta, u)]
	dense_matrix diag = chol.vectorD();
	a3_double logdet = a3_double(0.0);
	for(size_t j = 0; j < n_random_; j++)
		logdet += log( diag(j) );

	// f[beta, U(beta, theta, u)]
	a3d_vector both(n_fixed_ + n_random_);
	pack(beta, U, both);
	a3d_vector vec = a3_joint_density_.Forward(0, both);
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
	laplace_.Dependent(beta_theta_u, H);

	// optimize the recording
	laplace_.optimize();
}


} // END_DISMOD_AT_NAMESPACE
