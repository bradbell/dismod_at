// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/mixed_cppad.hpp>
/*
$begin mixed_cppad_ran_like_hes$$
$spell
	cppad
	hes
	vec
	const
	Cpp
	xam
$$

$section Hessian of Negative Log-Likelihood w.r.t. Random Effects$$

$head Syntax$$
$icode%mixed_object%.ran_like_hes(%a1_both_vec%, %a1_val_out%)%$$

$head Purpose$$
This routine computes the Hessian of the
negative log of the random negative log-likelihood
$cref/f(theta, u)
	/mixed_cppad_theory/
	Random Negative Log-Likelihood, f(theta, u)
/$$
with respect to the random effects vector $latex u$$; i.e.
$latex \[
	f_{uu}^{(2)} ( \theta, u )
\] $$

$head mixed_object$$
We use $cref/mixed_object/mixed_cppad_derived_ctor/mixed_object/$$
to denote an object of a class that is
derived from the $code mixed_cppad$$ base class.

$head a1_both_vec$$
This argument has prototype
$codei%
	const CppAD::vector<a1_double>& %a1_both_vec%
%$$
The following call will unpack the fixed and random effects from
$icode a1_both_vec$$:
$codei%
	unpack(%a1_fixed_vec%, %a1_random_vec%, %a1_both_vec%)
%$$

$subhead a1_fixed_vec$$
This argument has prototype
$codei%
	CppAD::vector<a1_double>& %a1_fixed_vec%
%$$
It specifies the value of the
$cref/fixed effects/mixed_cppad/Fixed Effects, theta/$$
vector $latex \theta$$.

$subhead a1_random_vec$$
This argument has prototype
$codei%
	CppAD::vector<a1_double>& %a1_random_vec%
%$$
It specifies the value of the
$cref/random effects/mixed_cppad/Random Effects, u/$$
vector $latex u$$.

$head hes_ran_row_$$
This $code mixed_cppad$$ member variable has prototype
$codei%
	const CppAD::vector<size_t> hes_ran_row_
%$$
It contains the row indices for the Hessian elements
that are possibly non-zero.
This is set by $code record_hes_ran$$ and hence
$code record_hes_ran_done_$$ must be true.
These indices are with respect to both
the fixed and random effect with
$codei%
	hes_ran_row_[%k%] - n_fixed_
%$$
the index with respect to the random effects only.

$head hes_ran_col_$$
This $code mixed_cppad$$ member variable has prototype
$codei%
	const CppAD::vector<size_t> hes_ran_col_
%$$
It contains the column indices for the Hessian elements
that are possibly non-zero.
Note that only the lower triangle of the Hessian is computed and hence
$codei%
	hes_ran_col_[%k%] <= hes_ran_row_[%k%]
%$$
for all $icode%k% = 0 , %...%, hes_ran_row_.size()-1%$$
These indices are with respect to both
the fixed and random effect with
$codei%
	hes_ran_col_[%k%] - n_fixed_
%$$
the index with respect to the random effects only.

$head a1_val_out$$
This argument has prototype
$codei%
	CppAD::vector<a1_double>& %a1_val_out%
%$$
If the input size must be equal to $code hes_ran_row_.size()$$.
Upon return, it contains the value of the Hessian elements
that are possibly non-zero where
$icode%val_out%[%k%]%$$ corresponds to row
$codei%hes_ran_row_[%k%] - n_fixed_%$$ and column
$codei%hes_ran_col_[%k%] - n_fixed_%$$ of the Hessian.

$children%
	example/devel/mixed_cppad/private/ran_like_hes_xam.cpp
%$$
$head Example$$
The file $cref ran_like_hes_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// ----------------------------------------------------------------------------
// ran_like_hes
void mixed_cppad::ran_like_hes(
	const a1d_vector&        a1_both_vec   ,
	a1d_vector&              a1_val_out    )
{	assert( record_hes_ran_done_ );

	// size of outputs
	size_t n_nonzero = hes_ran_row_.size();
	assert( hes_ran_col_.size() == n_nonzero );
	assert( a1_val_out.size() == n_nonzero );
	if( n_nonzero == 0 )
	{	// special case where Hessian is zero.
		return;
	}

	// create an a1d weight vector
	a1d_vector a1_w(1);
	a1_w[0] = 1.0;

	// First call to SparseHessian is in record_hes_ran
	CppAD::vector< std::set<size_t> > not_used(0);

	// compute the sparse Hessian
	a1_ran_like_.SparseHessian(
		a1_both_vec,
		a1_w,
		not_used,
		hes_ran_row_,
		hes_ran_col_,
		a1_val_out,
		hes_ran_work_
	);
}


} // END_DISMOD_AT_NAMESPACE

