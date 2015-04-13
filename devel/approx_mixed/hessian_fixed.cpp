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
$begin approx_mixed_hessian_fixed$$
$spell
	vec
	const
	Cpp
	xam
$$

$section Hessian With Respect to Fixed Effects$$

$head Syntax$$
$icode%approx_object%.hessian_fixed(
	%fixed_vec%, %random_vec%, %row_out%, %col_out%, %val_out%
)%$$

$head Purpose$$
This routine computes the Hessian of the negative log of the
joint part of the objective
$cref/H(beta, theta, u)/
	approx_mixed_theory/Joint Part of Objective/H(beta, theta, u)/$$
with respect to the random effects vector $latex \beta$$; i.e.
$latex \[
	H_{\beta \beta}^{(2)} ( \theta, \theta, \hat{u} )
\] $$

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
as for a previous call to $code hessian_fixed$$.
If it's input size is zero,
upon return it contains the row indices for the Hessian elements
that are possibly non-zero.

$head col_out$$
This argument has prototype
$codei%
	CppAD::vector<size_t>& %col_out%
%$$
If the input size of this array is non-zero,
the entire vector must be the same as for
a previous call to $code hessian_fixed$$.
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
as for a previous call to $code hessian_fixed$$.
Upon return, it contains the value of the Hessian elements
that are possibly non-zero (and will have the same size as $icode row_out$$).

$children%
	example/devel/approx_mixed/hessian_fixed_xam.cpp
%$$
$head Example$$
The file $cref hessian_fixed_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// ----------------------------------------------------------------------------
// hessian_fixed
void approx_mixed::hessian_fixed(
	const d_vector&          fixed_vec   ,
	const d_vector&          random_vec  ,
	CppAD::vector<size_t>&   row_out     ,
	CppAD::vector<size_t>&   col_out     ,
	d_vector&                val_out     )
{
	// number of fixed and random effects
	assert( n_fixed_  == fixed_vec.size() );
	assert( n_random_ == random_vec.size() );

	// make sure hessian has been recorded
	if( hes_fix_.size_var() == 0 )
	{	std::cerr << "approx_mixed::initialize was not called before"
		<< " approx_mixed::hessian_fixed" << std::endl;
		exit(1);
	}

	// size of outputs
	size_t n_nonzero = hes_fix_row_.size();
	assert( hes_fix_col_.size() == n_nonzero );
	assert( hes_fix_.Range()    == n_nonzero );

	// make sure outputs have proper dimension
	assert( row_out.size() == col_out.size() );
	assert( row_out.size() == val_out.size() );

	// create an d_vector containing (beta, theta, u)
	d_vector beta_theta_u( 2 * n_fixed_ + n_random_ );
	pack(fixed_vec, fixed_vec, random_vec, beta_theta_u);

	// compute the sparse Hessian
	size_t order = 0;
	val_out = hes_fix_.Forward(order, beta_theta_u);
	assert( val_out.size() == n_nonzero );

	if( row_out.size() == 0 )
	{	row_out.resize(n_nonzero);
		col_out.resize(n_nonzero);
		for(size_t k = 0; k < n_nonzero; k++)
		{	row_out[k] = hes_fix_row_[k];
			col_out[k] = hes_fix_col_[k];
		}
	}
# ifndef NDEUBG
	else
	{	for(size_t k = 0; k < n_nonzero; k++)
		{	assert( row_out[k] == hes_fix_row_[k] );
			assert( col_out[k] == hes_fix_col_[k] );
		}
	}
# endif
}


} // END_DISMOD_AT_NAMESPACE

