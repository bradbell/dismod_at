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
$begin approx_mixed_record_hes_fix$$
$spell
	hes
	vec
	const
	Cpp
$$

$section approx_mixed: Record Hessian of Joint Density w.r.t Fixed Effects$$

$head Syntax$$
$codei%record_hes_fix(%fixed_vec%, %random_vec%)%$$

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
It specifies the initial value for the
$cref/random effects/approx_mixed/Random Effects, u/$$ optimization.

$head hes_fix_$$
The input value of the member variable
$codei%
	CppAD::ADFun<double> hes_fix_
%$$
does not matter.
Upon return it contains the corresponding recording for the lower triangle of
$latex \[
	H_{\beta \beta}^{(2)} ( \beta, \theta , u )
\] $$
see $cref/H(beta, theta, u)
	/approx_mixed_theory/Objective, Joint Part/ H(beta, theta, u)/$$
Note that the matrix is symmetric and hence can be recovered from
its lower triangle.

$head hes_fix_row_, hes_fix_col_$$
The input value of the member variables
$codei%
	CppAD::vector<size_t> hes_fix_row_, hes_fix_col_
%$$
do not matter.
Upon return the contain the row indices and column indices
for the sparse Hessian represented by $code hes_fix_$$; i.e.
$codei%hes_fix_row_[%i%]%$$ and $codei%hes_fix_col_[%i%]%$$
are the row and column indices for the Hessian element
that corresponds to the $th i$$ component of the function
corresponding to $code hes_fix_$$.


$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void approx_mixed::record_hes_fix(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	size_t i, j;

	//	create an a1d_vector containing (theta, u)
	a1d_vector a1_beta_theta_u( 2 * n_fixed_ + n_random_ );
	pack(fixed_vec, fixed_vec, random_vec, a1_beta_theta_u);

	// start recording using a1_double operations
	CppAD::Independent( a1_beta_theta_u );

	// create an a2d_vector containing beta, theta and u
	a2d_vector a2_beta(n_fixed_), a2_theta(n_fixed_), a2_u(n_random_);
	unpack(a2_beta, a2_theta, a2_u, a1_beta_theta_u);

	// compute H(beta) using a2_double operations
	CppAD::Independent(a2_beta);
	a2d_vector a2_H(1);
	a2_H[0] = laplace_eval(a2_beta, a2_theta, a2_u);

	// create an ADFun object corresponding to H(beta)
	CppAD::ADFun<a1_double> a1_f;
	a1_f.Dependent(a2_beta, a2_H);

	// compute sparsity pattern corresponding to H_beta^{(1)} (beta)
	typedef CppAD::vector< std::set<size_t> > sparsity_pattern;
	sparsity_pattern r(n_fixed_);
	for(i = 0; i < n_fixed_; i++)
		r[i].insert(i);
	a1_f.ForSparseJac(n_fixed_, r);

	// compute sparsity pattern corresponding to H_{beta beta}^{(2)} (beta)
	sparsity_pattern s(1);
	assert( s[0].empty() );
	s[0].insert(0);
	sparsity_pattern pattern = a1_f.RevSparseHes(n_fixed_, s);

	// determine row and column indices in lower triangle of Hessian
	hes_fix_row_.clear();
	hes_fix_col_.clear();
	std::set<size_t>::iterator itr;
	for(i = 0; i < n_fixed_; i++)
	{	for(itr = pattern[i].begin(); itr != pattern[i].end(); itr++)
		{	j = *itr;
			// only compute lower triangular part
			if( i >= j )
			{	hes_fix_row_.push_back(i);
				hes_fix_col_.push_back(j);
			}
		}
	}
	size_t K = hes_fix_row_.size();

	// compute lower triangle of sparse Hessian H_{beta beta}^{(2)} (beta)
	a1d_vector a1_beta(n_fixed_), a1_theta(n_fixed_), a1_u(n_random_);
	a1d_vector a1_w(1), a1_hes(K);
	unpack(a1_beta, a1_theta, a1_u, a1_beta_theta_u);
	//
	a1_w[0] = 1.0;
	CppAD::sparse_hessian_work work;
	a1_f.SparseHessian(
		a1_beta, a1_w, pattern, hes_fix_row_, hes_fix_col_, a1_hes, work
	);

	// complete recording of H_{beta beta}^{(2)} (beta, theta, u)
	hes_fix_.Dependent(a1_beta_theta_u, a1_hes);

	// optimize the recording
	hes_fix_.optimize();
}

} // END_DISMOD_AT_NAMESPACE
