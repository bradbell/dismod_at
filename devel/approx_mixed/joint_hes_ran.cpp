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
$begin approx_mixed_joint_hes_ran$$
$spell
	hes
	vec
	const
	Cpp
	xam
$$

$section Hessian With Respect to Random Effects$$

$head Syntax$$
$icode%approx_object%.joint_hes_ran(
	%fixed_vec%, %random_vec%, %row_out%, %col_out%, %val_out%
)%$$

$head Purpose$$
This routine computes the Hessian of the negative log of the joint density
$cref/f(theta, u)/approx_mixed_theory/Joint Density, f(theta, u)/$$
with respect to the random effects vector $latex u$$; i.e.
$latex \[
	f_{uu}^{(2)} ( \theta, u )
\] $$

$head approx_object$$
We use $cref/approx_object/approx_mixed_derived_ctor/approx_object/$$
to denote an object of a class that is
derived from the $code approx_mixed$$ base class.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<a3_double>& %fixed_vec%
%$$
It specifies the value of the
$cref/fixed effects/approx_mixed/Fixed Effects, theta/$$
vector $latex \theta$$.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<a3_double>& %random_vec%
%$$
It specifies the value of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$.

$head row_out$$
This argument has prototype
$codei%
	CppAD::vector<size_t>& %row_out%
%$$
If the input size of this array is non-zero,
the entire vector must be the same
as for a previous call to $code joint_hes_ran$$.
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
a previous call to $code joint_hes_ran$$.
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
	CppAD::vector<a3_double>& %val_out%
%$$
If the input size of this array is non-zero, it must have the same size
as for a previous call to $code joint_hes_ran$$.
Upon return, it contains the value of the Hessian elements
that are possibly non-zero (and will have the same size as $icode row_out$$).

$children%
	example/devel/approx_mixed/joint_hes_ran_xam.cpp
%$$
$head Example$$
The file $cref joint_hes_ran_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// ----------------------------------------------------------------------------
// joint_hes_ran
void approx_mixed::joint_hes_ran(
	const a3d_vector&        fixed_vec   ,
	const a3d_vector&        random_vec  ,
	CppAD::vector<size_t>&   row_out     ,
	CppAD::vector<size_t>&   col_out     ,
	a3d_vector&              val_out     )
{
	// make sure initialize has been called
	if( hes_ran_.size_var() == 0 )
	{	std::cerr << "approx_mixed::initialize was not called before"
		<< " approx_mixed::joint_hes_ran" << std::endl;
		exit(1);
	}
	// number of fixed and random effects
	assert( n_fixed_  == fixed_vec.size() );
	assert( n_random_ == random_vec.size() );

	// size of outputs
	size_t n_nonzero = hes_ran_row_.size();
	assert( hes_ran_col_.size() == n_nonzero );
	assert( hes_ran_.Range()    == n_nonzero );

	// make sure outputs have proper dimension
	assert( row_out.size() == col_out.size() );
	assert( row_out.size() == val_out.size() );

	// create an a3d_vector containing (theta, u)
	a3d_vector both_vec( n_fixed_ + n_random_ );
	pack(fixed_vec, random_vec, both_vec);

	// compute the sparse Hessian
	size_t order = 0;
	val_out = hes_ran_.Forward(order, both_vec);
	assert( val_out.size() == n_nonzero );

	if( row_out.size() == 0 )
	{	row_out.resize(n_nonzero);
		col_out.resize(n_nonzero);
		for(size_t k = 0; k < n_nonzero; k++)
		{	row_out[k] = hes_ran_row_[k];
			col_out[k] = hes_ran_col_[k];
		}
	}
# ifndef NDEUBG
	else
	{	for(size_t k = 0; k < n_nonzero; k++)
		{	assert( row_out[k] == hes_ran_row_[k] );
			assert( col_out[k] == hes_ran_col_[k] );
		}
	}
# endif
}


} // END_DISMOD_AT_NAMESPACE

