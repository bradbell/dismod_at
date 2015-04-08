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
$begin approx_mixed_joint_laplace$$
$spell
	vec
	const
	Cpp
$$

$section approx_mixed: Joint Part of the Laplace Approximation Objective$$

$head Syntax$$
$icode%H% = %approx_object%.joint_laplace(%beta%, %theta%, %u%)%$$

$head Purppose$$
This routine evaluates the joint laplace of the Laplace approximation objective
$cref/H(beta, theta, u)/approx_mixed_theory
	/Joint Part
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
vector $latex \beta$$ at which the recording is made.

$head theta$$
This argument has prototype
$codei%
	const CppAD::vector<a1_double>& %theta%
%$$
It specifies the value of the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$ at which the recording is made.

$head u$$
This argument has prototype
$codei%
	const CppAD::vector<a1_double>& %u%
%$$
It specifies the value of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$ at which the recording is made.

$end
*/
# include <Eigen/Sparse>
# include <dismod_at/approx_mixed.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

CppAD::AD<double> approx_mixed::joint_laplace(
	const a1d_vector& beta  ,
	const a1d_vector& theta ,
	const a1d_vector& u
)
{
	// evaluate gradient f_u^{(1)} (beta , u )
	a1d_vector grad = gradient_random(beta, u);

	// evaluate the hessian f_{uu}^{(2)} (theta, u)
	CppAD::vector<size_t> row, col;
	a1d_vector val;
	hessian_random(theta, u, row, col, val);

	// declare eigen matrix types
	using Eigen::Dynamic;
	typedef Eigen::Matrix<a1_double, Dynamic, Dynamic> dense_matrix;
	typedef Eigen::SparseMatrix<a1_double>             sparse_matrix;

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
	a1d_vector U(n_random_);
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
	a1_double logdet = 0.0;
	for(size_t j = 0; j < n_random_; j++)
		logdet += log( diag(j) );

	// f[beta, U(beta, theta, u)]
	a1d_vector vec = joint_density(beta, U);
	a1_double sum = vec[0];
	size_t n_abs = vec.size() - 1;
	for(size_t i = 0; i < n_abs; i++)
		sum += abs( vec[1 + i] );

	// constant term
	double pi   = CppAD::atan(1.0) * 4.0;
	double constant_term = CppAD::log(2.0 * pi) * double(n_random_) / 2.0;

	// return H(beta, theta, u)
	return logdet / 2.0 + sum - constant_term;
}


} // END_DISMOD_AT_NAMESPACE
