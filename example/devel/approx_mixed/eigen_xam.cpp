// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
A      = [ 5, 4, 2; 4, 5, 1; 2, 1, 5]
det(A) = 36
inv(A) = [ 24, -18, -6; -18, 21, 3; -6, 3, 9] / det(A)
*/
# include <Eigen/Sparse>
# include <vector>
# include <iostream>
# include <cppad/cppad.hpp>

bool eigen_xam(void)
{	bool ok = true;
	double eps = 100. * std::numeric_limits<double>::epsilon();

	using Eigen::Dynamic;
	typedef CppAD::AD<double>                     real;
	typedef Eigen::SparseMatrix<real>             real_sparse_matrix;
	typedef Eigen::Matrix<real, Dynamic, Dynamic> real_dense_matrix;

	// size of the matrix
	size_t n = 3;

	double A_inv[] = {
		 24.0, -18.0, -6.0,
		-18.0,  21.0,  3.0,
		 -6.0,   3.0,  9.0
	};
	for(size_t i = 0; i < sizeof(A_inv)/sizeof(A_inv[0]); i++)
		A_inv[i] /= 36.;

	// declare spares matrix
	real_sparse_matrix A(n, n);

	// create Matrix
	A.insert(0, 0) = 5.0;
	A.insert(0, 1) = 4.0;
	A.insert(0, 2) = 2.0;
	A.insert(1, 0) = 4.0;
	A.insert(1, 1) = 5.0;
	A.insert(1, 2) = 1.0;
	A.insert(2, 0) = 2.0;
	A.insert(2, 1) = 1.0;
	A.insert(2, 2) = 5.0;

	// compute cholesky factor
	Eigen::SimplicialLDLT<real_sparse_matrix> chol(A);
	real det = chol.determinant();
	ok      &= det == 36.;

	real_dense_matrix diag = chol.vectorD();
	ok &= size_t( diag.size() ) == n;  
	real log_det = 0.0;
	for(size_t i = 0; i < n; i++)
		log_det += log( diag(i) );
	ok &= abs( exp(log_det) / det - 1.0 ) < eps;

	// initialize dense column vector as zero
	real_dense_matrix b(n, 1);
	for(size_t i = 0; i < n; i++)
		b(i) = 0.0;

	// check computation of the inverse of A
	for(size_t j = 0; j < n; j++)
	{	b(j) = 1.0;
		real_dense_matrix x = chol.solve(b);
		b(j) = 0.0;
		for(size_t i = 0; i < n; i++)
			ok &= CppAD::abs( x(i) / A_inv[i*n + j] - 1.0 ) <= eps;
	}
	return ok;
}
