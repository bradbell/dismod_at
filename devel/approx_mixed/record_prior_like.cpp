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
$begin approx_mixed_record_prior_like$$
$spell
	jac
	vec
	const
	Cpp
	Jacobian
	var
$$

$section approx_mixed: Record Prior Negative Log-Likelihood For Fixed Effects$$

$head Syntax$$
$codei%record_prior_like(%fixed_vec%)%$$

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

$head record_prior_like_done_$$
When this function is called, this member variable must be false.
Upon return it is true.

$head prior_like_$$
On input, the member variable
$codei%
	CppAD::ADFun<double> prior_like_
%$$
must be empty; i.e., $code prior_like_.size_var() == 0$$.
If the return value for
$cref/prior_like/approx_mixed_prior_like/$$ is empty,
$code prior_like_$$ is not modified.
Otherwise,
upon return it contains the corresponding recording for the
$cref/prior_like/approx_mixed_prior_like/$$.
Note that the function result is the
$cref/negative log-density vector/approx_mixed/Negative Log-Density Vector/$$
corresponding to the function
$cref/g(theta)/approx_mixed_theory/Prior Negative Log-Likelihood, g(theta)/$$.

$head prior_jac_row_, prior_jac_col_$$
The input value of the member variables
$codei%
	CppAD::vector<size_t> prior_jac_row_, prior_jac_col_
%$$
do not matter.
Upon return they contain the row indices and column indices
that correspond to non-zero elements in the Jacobian corresponding to
$code prior_like_$$.

$head prior_jac_work_$$
The input value of the member variables
$codei%
	CppAD::sparse_jacobian_work prior_jac_work_
%$$
does not matter.
Upon return it contains the work information for reuse by calls of the form
$codei%
	prior_like_.SparseJacobianForward(
		%x%, %not_used%, prior_jac_row_, prior_jac_col_, %jac%, prior_jac_work_
	)
%$$

$end
*/
namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void approx_mixed::record_prior_like(const d_vector& fixed_vec  )
{	assert( fixed_vec.size() == n_fixed_ );
	assert( ! record_prior_like_done_ );

	// ------------------------------------------------------------------------
	// prior_like_
	// ------------------------------------------------------------------------
	// convert to an a1d_vector
	a1d_vector a1_theta(n_fixed_);
	for(size_t j = 0; j < n_fixed_; j++)
		a1_theta[j] = fixed_vec[j];

	// start recording a1_double operations
	Independent(a1_theta);

	// compute prior_like
	a1d_vector a1_vec = prior_like(a1_theta);
	if( a1_vec.size() == 0 )
	{	CppAD::AD<double>::abort_recording();
		record_prior_like_done_ = true;
		assert( prior_like_.size_var() == 0 );
		return;
	}

	// save the recording
	prior_like_.Dependent(a1_theta, a1_vec);

	// optimize the recording
	prior_like_.optimize();

	// ------------------------------------------------------------------------
	// prior_jac_row_, prior_jac_col_, prior_jac_work_
	// ------------------------------------------------------------------------
	// compute the sparsity pattern for the Jacobian
	typedef CppAD::vector< std::set<size_t> > sparsity_pattern;
	sparsity_pattern r(n_fixed_);
	for(size_t j = 0; j < n_fixed_; j++)
		r[j].insert(j);
	sparsity_pattern pattern = prior_like_.ForSparseJac(n_fixed_, r);

	// convert sparsity to row and column index form
	prior_jac_row_.clear();
	prior_jac_col_.clear();
	std::set<size_t>::iterator itr;
	for(size_t i = 0; i < prior_like_.Range(); i++)
	{	for(itr = pattern[i].begin(); itr != pattern[i].end(); itr++)
		{	size_t j = *itr;
			prior_jac_row_.push_back(i);
			prior_jac_col_.push_back(j);
		}
	}

	// compute the work vector for reuse during Jacobian sparsity calculations
	d_vector jac( prior_jac_row_.size() );
	prior_like_.SparseJacobianForward(
		fixed_vec       ,
		pattern         ,
		prior_jac_row_  ,
		prior_jac_col_  ,
		jac             ,
		prior_jac_work_
	);
	// ------------------------------------------------------------------------
	// prior_hes_row_, prior_hes_col_, prior_hes_work_
	// ------------------------------------------------------------------------
	// no need to recalculate forward sparsity pattern.
	//
	// sparsity pattern for the Hessian
	sparsity_pattern s(1);
	for(size_t i = 0; i < prior_like_.Range(); i++ )
		s[0].insert(i);
	pattern.clear();
	pattern = prior_like_.RevSparseHes(n_fixed_, s);

	// determine row and column indices in lower triangle of Hessian
	prior_hes_row_.clear();
	prior_hes_col_.clear();
	for(size_t i = 0; i < n_fixed_; i++)
	{	for(itr = pattern[i].begin(); itr != pattern[i].end(); itr++)
		{	size_t j = *itr;
			// only compute lower triangular part
			if( i >= j )
			{	prior_hes_row_.push_back(i);
				prior_hes_col_.push_back(j);
			}
		}
	}
	size_t K = prior_hes_row_.size();

	// compute the work vector for reuse during Hessian sparsity calculations
	d_vector weight( prior_like_.Range() ), hes(K);
	prior_like_.SparseHessian(
		fixed_vec       ,
		weight          ,
		pattern         ,
		prior_hes_row_  ,
		prior_hes_col_  ,
		hes             ,
		prior_hes_work_
	);

	record_prior_like_done_ = true;
	return;
}


} // END_DISMOD_AT_NAMESPACE
