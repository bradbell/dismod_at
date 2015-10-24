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
$begin mixed_cppad_record_constraint$$
$spell
	cppad
	jac
	vec
	const
	Cpp
	Jacobian
$$

$section mixed_cppad: Record Prior The Constraint Function$$

$head Syntax$$
$codei%record_constraint(%fixed_vec%)%$$

$head Private$$
This function is $code private$$ to the $code mixed_cppad$$ class
and cannot be used by a derived
$cref/mixed_object/mixed_cppad_derived_ctor/mixed_object/$$.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_vec%
%$$
It specifies the value of the
$cref/fixed effects/mixed_cppad/Fixed Effects, theta/$$
vector $latex \theta$$ at which the recording is made.

$head constraint_$$
The input value of the member variable
$codei%
	CppAD::ADFun<double> constraint_
%$$
does not matter.
Upon return it contains the corresponding recording for the
$cref/constraint/mixed_cppad_constraint/$$ function $latex c( \theta )$$.
Note that the function result is the

$head constraint_jac_row_, constraint_jac_col_$$
The input value of the member variables
$codei%
	CppAD::vector<size_t> constraint_jac_row_, constraint_jac_col_
%$$
do not matter.
Upon return they contain the row indices and column indices
that correspond to non-zero elements in the Jacobian corresponding to
$code constraint_$$.

$head constraint_jac_work_$$
The input value of the member variables
$codei%
	CppAD::sparse_jacobian_work constraint_jac_work_
%$$
does not matter.
Upon return it contains the work information for reuse by calls of the form
$codei%
	constraint_.SparseJacobianForward(
		%x%,
		%not_used%,
		constraint_jac_row_,
		constraint_jac_col_,
		%jac%,
		constraint_jac_work_
	)
%$$

$end
*/
namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void mixed_cppad::record_constraint(const d_vector& fixed_vec  )
{	assert( fixed_vec.size() == n_fixed_ );
	assert( ! record_constraint_done_ );

	// ------------------------------------------------------------------------
	// constraint_
	// ------------------------------------------------------------------------
	// convert to an a1d_vector
	a1d_vector a1_theta(n_fixed_);
	for(size_t j = 0; j < n_fixed_; j++)
		a1_theta[j] = fixed_vec[j];

	// start recording a1_double operations
	Independent(a1_theta);

	// compute constraint
	a1d_vector a1_vec = constraint(a1_theta);

	// check for empty recording case
	if( a1_vec.size() == 0 )
	{	// abort recording AD<double>
		CppAD::AD<double>::abort_recording();
		record_constraint_done_ = true;
		return;
	}

	// save the recording
	constraint_.Dependent(a1_theta, a1_vec);

	// optimize the recording
	constraint_.optimize();

	// ------------------------------------------------------------------------
	// constraint_jac_row_, constraint_jac_col_, constraint_jac_work_
	// ------------------------------------------------------------------------
	// compute the sparsity pattern for the Jacobian
	typedef CppAD::vector< std::set<size_t> > sparsity_pattern;
	sparsity_pattern r(n_fixed_);
	for(size_t j = 0; j < n_fixed_; j++)
		r[j].insert(j);
	sparsity_pattern pattern = constraint_.ForSparseJac(n_fixed_, r);

	// convert sparsity to row and column index form
	constraint_jac_row_.clear();
	constraint_jac_col_.clear();
	std::set<size_t>::iterator itr;
	for(size_t i = 0; i < constraint_.Range(); i++)
	{	for(itr = pattern[i].begin(); itr != pattern[i].end(); itr++)
		{	size_t j = *itr;
			constraint_jac_row_.push_back(i);
			constraint_jac_col_.push_back(j);
		}
	}

	// compute the work vector for reuse during Jacobian sparsity calculations
	d_vector jac( constraint_jac_row_.size() );
	constraint_.SparseJacobianForward(
		fixed_vec            ,
		pattern              ,
		constraint_jac_row_  ,
		constraint_jac_col_  ,
		jac                  ,
		constraint_jac_work_
	);
	// ------------------------------------------------------------------------
	// constraint_hes_row_, constraint_hes_col_, constraint_hes_work_
	// ------------------------------------------------------------------------
	// no need to recalculate forward sparsity pattern.
	//
	// sparsity pattern for the Hessian
	sparsity_pattern s(1);
	for(size_t i = 0; i < constraint_.Range(); i++ )
		s[0].insert(i);
	pattern.clear();
	pattern = constraint_.RevSparseHes(n_fixed_, s);

	// determine row and column indices in lower triangle of Hessian
	constraint_hes_row_.clear();
	constraint_hes_col_.clear();
	for(size_t i = 0; i < n_fixed_; i++)
	{	for(itr = pattern[i].begin(); itr != pattern[i].end(); itr++)
		{	size_t j = *itr;
			// only compute lower triangular part
			if( i >= j )
			{	constraint_hes_row_.push_back(i);
				constraint_hes_col_.push_back(j);
			}
		}
	}
	size_t K = constraint_hes_row_.size();

	// compute the work vector for reuse during Hessian sparsity calculations
	d_vector weight( constraint_.Range() ), hes(K);
	constraint_.SparseHessian(
		fixed_vec            ,
		weight               ,
		pattern              ,
		constraint_hes_row_  ,
		constraint_hes_col_  ,
		hes                  ,
		constraint_hes_work_
	);
	//
	record_constraint_done_ = true;
	return;
}


} // END_DISMOD_AT_NAMESPACE
