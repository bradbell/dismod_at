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
$begin approx_mixed_laplace_hes_fix$$
$spell
	hes
	vec
	const
	Cpp
	xam
$$

$section Hessian of Laplace Approximation w.r.t Fixed Effects$$

$head Syntax$$
$icode%approx_object%.laplace_hes_fix(
	%fixed_vec%, %random_vec%, %row_out%, %col_out%, %val_out%
)%$$

$head Purpose$$
This routine computes the Hessian w.r.t the fixed effects of
random part of the of the objective; i.e.,
$latex \[
	H_\beta^{(2)} ( \beta , \theta , u )
\] $$
See $cref/H(beta, theta, u)/
	approx_mixed_theory/Random Part of Objective
	/H(beta, theta, u)
/$$.

$head approx_object$$
We use $cref/approx_object/approx_mixed_derived_ctor/approx_object/$$
to denote an object of a class that is
derived from the $code approx_mixed$$ base class.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_vec%
%$$
It specifies the value of the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %random_vec%
%$$
Given the fixed effects $latex \theta$$, it is the corresponding optimal value
for the random effects vector; i.e.,
$cref/u^(theta)/approx_mixed_theory/Objective/u^(theta)/$$.

$head row_out$$
This argument has prototype
$codei%
	CppAD::vector<size_t>& %row_out%
%$$
If the input size of this array is non-zero,
the entire vector must be the same
as for a previous call to $code laplace_hes_fix$$.
If it's input size is zero,
upon return it contains the row indices for the Hessian elements
that are possibly non-zero;
$codei%
	%row_out%[%k%] < %n_fixed%
%$$
for all $icode%k% = 0 , %...%, %row_out%.size()-1%$$

$head col_out$$
This argument has prototype
$codei%
	CppAD::vector<size_t>& %col_out%
%$$
If the input size of this array is non-zero,
the entire vector must be the same as for
a previous call to $code laplace_hes_fix$$.
If it's input size is zero,
upon return it contains the column indices for the Hessian elements
that are possibly non-zero (and will have the same size as $icode row_out$$).
Note that only the lower triangle of the Hessian is computed and hence
$codei%
	%col_out%[%k%] <= %row_out%[%k%]
%$$
for all $icode%k% = 0 , %...%, %row_out%.size()-1%$$

$head val_out$$
This argument has prototype
$codei%
	CppAD::vector<double>& %val_out%
%$$
If the input size of this array is non-zero, it must have the same size
as for a previous call to $code laplace_hes_fix$$.
Upon return, it contains the value of the Hessian elements
that are possibly non-zero (and will have the same size as $icode row_out$$).

$children%
	example/devel/approx_mixed/private/laplace_hes_fix_xam.cpp
%$$
$head Example$$
The file $cref laplace_hes_fix_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// ----------------------------------------------------------------------------
// laplace_hes_fix
void approx_mixed::laplace_hes_fix(
	const d_vector&          fixed_vec   ,
	const d_vector&          random_vec  ,
	CppAD::vector<size_t>&   row_out     ,
	CppAD::vector<size_t>&   col_out     ,
	d_vector&                val_out     )
{	assert( record_hes_fix_done_ );

	// size of outputs
	size_t n_nonzero = hes_fix_row_.size();
	if( n_nonzero == 0 )
	{	// special case where Hessian of Laplace approximation is zero.
		assert( row_out.size() == 0 );
		assert( col_out.size() == 0 );
		assert( val_out.size() == 0 );
		return;
	}

	// number of fixed and random effects
	assert( n_fixed_  == fixed_vec.size() );
	assert( n_random_ == random_vec.size() );
	assert( hes_fix_col_.size() == n_nonzero );

	// make sure outputs have proper dimension
	assert( row_out.size() == col_out.size() );
	assert( row_out.size() == val_out.size() );

	// create an a2d_vector containing (beta, theta, u)
	a1d_vector a1_beta_theta_u( 2 * n_fixed_ + n_random_ );
	pack(fixed_vec, fixed_vec, random_vec, a1_beta_theta_u);
	a2d_vector a2_beta_theta_u( 2 * n_fixed_ + n_random_ );
	for(size_t j = 0; j < 2 * n_fixed_ + n_random_; j++)
		a2_beta_theta_u[j] = a1_beta_theta_u[j];

	// create an a2d weighting vector
	a2d_vector a2_w(1);
	a2_w[0] = 1.0;

	// create an a2d vector for the results
	a2d_vector a2_val_out( hes_fix_row_.size() );

	// compute the sparse Hessian
	laplace_2_.SparseHessian(
		a2_beta_theta_u,
		a2_w,
		hes_fix_sparsity_,
		hes_fix_row_,
		hes_fix_col_,
		a2_val_out,
		hes_fix_work_
	);

	if( row_out.size() == 0 )
	{	row_out.resize(n_nonzero);
		col_out.resize(n_nonzero);
		val_out.resize(n_nonzero);
		for(size_t k = 0; k < n_nonzero; k++)
		{	row_out[k] = hes_fix_row_[k];
			col_out[k] = hes_fix_col_[k];
		}
	}
	for(size_t k = 0; k < n_nonzero; k++)
		val_out[k] = Value( Value( a2_val_out[k] ) );

# ifndef NDEUBG
	for(size_t k = 0; k < n_nonzero; k++)
	{	assert( row_out[k] == hes_fix_row_[k] );
		assert( col_out[k] == hes_fix_col_[k] );
	}
# endif
}


} // END_DISMOD_AT_NAMESPACE

