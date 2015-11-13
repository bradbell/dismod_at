// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/cppad_mixed.hpp>
# include <dismod_at/chol_hes_ran.hpp>
# include <Eigen/Sparse>

/*
$begin chol_hes_ran$$
$spell
	init_chol_hes
	CppAD
	cppad
	const
	Cholesky
	namespace
	Eigen
	hpp
	Simplicial
	triangular
	dismod
$$

$section Sparse Cholesky Factorization of Hessian w.r.t Random Effects$$

$head Syntax$$
$codei%init_chol_hes_ran(%n_random%, %row%, %col%)%$$

$head Private$$
This function should not used by a derived
$cref/mixed_object/cppad_mixed_derived_ctor/mixed_object/$$.

$head Purpose$$
$code chol_hes_ran_$$ should be a $cref cppad_mixed_private$$ member variable,
but it is instead a static in the $code dismod_at$$ namespace
so that the warnings that Eigen generates
do not need to be suppressed by all the routines that include
$code dismod_at/cppad_mixed.hpp$$.

$head chol_hes_ran_$$
This variable has prototype
$codei%
	static Eigen::SimplicialLDLT<
		Eigen::SparseMatrix<double> , Eigen::Lower
	> chol_hes_ran_;
%$$
This is lower triangular Cholesky factorization of the Hessian of the
$cref/random likelihood
	/cppad_mixed_theory
	/Random Likelihood, f(theta, u)
/$$
with respect to the random effects; i.e.
$latex f_{uu}^{(2)} ( \theta , u )$$.

$head init_chol_hes_ran$$

$head n_random$$
This argument has prototype
$codei%
	size_t %n_random%
%$$

$subhead row$$
This argument has prototype
$codei%
	const CppAD::vector<size_t>& %row%
%$$
These are the non-zero row indices in the sparsity pattern for the Hessian.
All of its elements must be less than $icode n_random$$.

$subhead col$$
This argument has prototype
$codei%
	const CppAD::vector<size_t>& %col%
%$$
These are the non-zero column indices in the sparsity pattern for the Hessian.
It must have the same size as $icode row$$ and
all of its elements must be less than $icode n_random$$.

$subhead chol_hes_ran_$$
The input value of this factorization does not matter.
Upon return, the sparsity pattern has been analyzed; i.e.,
$codei%
	chol_hes_ran_.analyzePattern(%hessian_pattern%)
%$$
has been called with the pattern corresponding to $icode row$$ and
$icode col$$.

$codep */
	namespace dismod_at {
		Eigen::SimplicialLDLT<
			Eigen::SparseMatrix<double> , Eigen::Lower
		> chol_hes_ran_;
	}
/* $$

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void init_chol_hes_ran(
	size_t                       n_random ,
	const CppAD::vector<size_t>& row      ,
	const CppAD::vector<size_t>& col      )
{	double not_used = 1.0;

	Eigen::SparseMatrix<double> hessian_pattern(n_random, n_random);
	assert( row.size() == col.size() );
	for(size_t k = 0; k < row.size(); k++)
	{	assert( row[k] < n_random );
		assert( col[k] < n_random );
		hessian_pattern.insert(row[k], col[k]) = not_used;
	}
	// analyze the pattern for an LDL^T Cholesky factorization of
	// f_{uu}^{(2)}(theta, u)
	chol_hes_ran_.analyzePattern(hessian_pattern);
}


} // END_DISMOD_AT_NAMESPACE
