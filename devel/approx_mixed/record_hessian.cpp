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
$begin approx_mixed_record_hessian$$
$spell
	vec
	const
	Cpp
$$

$section approx_mixed: Record the Joint Density Evaluator Function$$

$head Syntax$$
$codei%record_hessian(%fixed_vec%, %random_vec%)%$$

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
It specifies the value of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$ at which the recording is made.

$head hessian_$$
The input value of the member variable
$codei%
	CppAD::ADFun<a1_double> hessian_
%$$
does not matter.
Upon return it contains the corresponding recording of
$latex f_{uu}^{(2)} ( \theta , u )$$.

$head hessian_row_, hessian_col_$$
The input value of the member variables
$codei%
	CppAD::vector<size_t> hessian_row_, hessian_col_
%$$
do not matter.
Upon return the contain the row indices and column indices
for the sparse Hessian represented by $code hessian_$$; i.e.
$codei%hessian_row_[%i%]%$$ and $codei%hessian_col_[%i%]%$$
are the row and column indices for the Hessian element
that corresponds to the $th i$$ component of the function
corresponding to $code hessian_$$.

$head joint_density$$
The member function $code joint_density$$ called
with arguments of type $code a2d_vector$$.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void approx_mixed::record_hessian(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	size_t i, j;

	//	create an a1d_vector containing (theta, u)
	a1d_vector a1_both( n_fixed_ + n_random_ );
	for(j = 0; j < n_fixed_; j++)
		a1_both[j] = fixed_vec[j];
	for(j = 0; j < n_random_; j++)
		a1_both[n_fixed_ + j] = random_vec[j];

	// start recording f_uu (theta, u) using a1_double operations
	CppAD::Independent( a1_both );

	// create an a2d_vector containing theta and u
	a2d_vector a2_theta(n_fixed_), a2_u(n_random_);
	for(j = 0; j < n_fixed_; j++)
		a2_theta[j] = a1_both[j];
	for(j = 0; j < n_random_; j++)
		a2_u[j] = a1_both[n_fixed_ + j];

	// compute f(u) using a2_double operations
	CppAD::Independent(a2_u);
	a2d_vector a2_vec = joint_density(a2_theta, a2_u);
	a2d_vector a2_sum(1);
	a2_sum[0]    = a2_vec[0];
	size_t n_abs = a2_vec.size() - 1;
	for(i = 0; i < n_abs; i++)
		a2_sum[0] += abs( a2_vec[1 + j] );
	CppAD::ADFun<a1_double> a1_f;
	a1_f.Dependent(a2_u, a2_sum);

	// compute sparsity pattern corresponding to f_u^1 (u)
	typedef CppAD::vector< std::set<size_t> > sparsity_pattern;
	sparsity_pattern r(n_random_);
	for(i = 0; i < n_random_; i++)
		r[i].insert(i);
	a1_f.ForSparseJac(n_random_, r);

	// compute sparsity pattern corresponding to f_uu^2 (u)
	sparsity_pattern s(1);
	assert( s[0].empty() );
	s[0].insert(0);
	sparsity_pattern pattern = a1_f.RevSparseHes(n_random_, s);

	// determine row and column indices in lower triangle of Hessian
	hessian_row_.clear();
	hessian_col_.clear();
	std::set<size_t>::iterator itr;
	for(i = 0; i < n_random_; i++)
	{	for(itr = pattern[i].begin(); itr != pattern[i].end(); itr++)
		{	j = *itr;
			// only compute lower triangular part
			if( i >= j )
			{	hessian_row_.push_back(i);
				hessian_col_.push_back(j);
			}
		}
	}
	size_t K = hessian_row_.size();

	// compute lower triangle of sparse Hessian f_uu^2 (u)
	a1d_vector a1_u(n_random_), a1_w(1), a1_hes(K);
	for(j = 0; j < n_random_; j++)
		a1_u[j] = a1_both[n_fixed_ + j];
	a1_w[0] = 1.0;
	CppAD::sparse_hessian_work work;
	a1_f.SparseHessian(
		a1_u, a1_w, pattern, hessian_row_, hessian_col_, a1_hes, work
	);

	// complete recording of f_uu^2 (u, theta)
	hessian_.Dependent(a1_both, a1_hes);

	// optimize the recording
	hessian_.optimize();
}


} // END_DISMOD_AT_NAMESPACE
