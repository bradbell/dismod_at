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
$begin approx_mixed_record_hes_ran$$
$spell
	hes
	vec
	const
	Cpp
$$

$section approx_mixed: Record Hessian of Joint Density w.r.t Random Effects$$

$head Syntax$$
$codei%record_hes_ran(%fixed_vec%, %random_vec%)%$$

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

$head hes_ran_$$
The input value of the member variable
$codei%
	CppAD::ADFun<a3_double> hes_ran_
%$$
does not matter.
Upon return it contains the corresponding recording for the lower triangle of
$latex \[
	f_{uu}^{(2)} ( \theta , u )
\]$$
see $cref/f(theta, u)/approx_mixed_theory/Joint Density, f(theta, u)/$$.
Note that the matrix is symmetric and hence can be recovered from
its lower triangle.

$head hes_ran_row_, hes_ran_col_$$
The input value of the member variables
$codei%
	CppAD::vector<size_t> hes_ran_row_, hes_ran_col_
%$$
do not matter.
Upon return the contain the row indices and column indices
for the sparse Hessian represented by $code hes_ran_$$; i.e.
$codei%hes_ran_row_[%i%]%$$ and $codei%hes_ran_col_[%i%]%$$
are the row and column indices for the Hessian element
that corresponds to the $th i$$ component of the function
corresponding to $code hes_ran_$$.


$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void approx_mixed::record_hes_ran(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	size_t i, j;

	//	create an a4d_vector containing (theta, u)
	a4d_vector a4_both( n_fixed_ + n_random_ );
	pack(fixed_vec, random_vec, a4_both);

	// start recording using a4_double operations
	CppAD::Independent( a4_both );

	// create an a5d_vector containing theta and u
	a5d_vector a5_theta(n_fixed_), a5_u(n_random_);
	unpack(a5_theta, a5_u, a4_both);

	// compute f(u) using a5_double operations
	CppAD::Independent(a5_u);
	//
	a5d_vector a5_both(n_fixed_ + n_random_);
	a5d_vector a5_vec = joint_density(a5_theta, a5_u);
	a5d_vector a5_sum(1);
	a5_sum[0]    = a5_vec[0];
	size_t n_abs = a5_vec.size() - 1;
	for(i = 0; i < n_abs; i++)
		a5_sum[0] += abs( a5_vec[1 + i] );

	// create an ADFun object corresponding to f(u)
	CppAD::ADFun<a4_double> a4_f;
	a4_f.Dependent(a5_u, a5_sum);

	// compute sparsity pattern corresponding to f_u^{(1)} (u)
	typedef CppAD::vector< std::set<size_t> > sparsity_pattern;
	sparsity_pattern r(n_random_);
	for(i = 0; i < n_random_; i++)
		r[i].insert(i);
	a4_f.ForSparseJac(n_random_, r);

	// compute sparsity pattern corresponding to f_uu^{(2)} (u)
	sparsity_pattern s(1);
	assert( s[0].empty() );
	s[0].insert(0);
	sparsity_pattern pattern = a4_f.RevSparseHes(n_random_, s);

	// determine row and column indices in lower triangle of Hessian
	hes_ran_row_.clear();
	hes_ran_col_.clear();
	std::set<size_t>::iterator itr;
	for(i = 0; i < n_random_; i++)
	{	for(itr = pattern[i].begin(); itr != pattern[i].end(); itr++)
		{	j = *itr;
			// only compute lower triangular part
			if( i >= j )
			{	hes_ran_row_.push_back(i);
				hes_ran_col_.push_back(j);
			}
		}
	}
	size_t K = hes_ran_row_.size();

	// compute lower triangle of sparse Hessian f_uu^2 (u)
	a4d_vector a4_theta(n_fixed_), a4_u(n_random_), a4_w(1), a4_hes(K);
	unpack(a4_theta, a4_u, a4_both);
	//
	a4_w[0] = 1.0;
	CppAD::sparse_hessian_work work;
	a4_f.SparseHessian(
		a4_u, a4_w, pattern, hes_ran_row_, hes_ran_col_, a4_hes, work
	);

	// complete recording of f_uu^2 (u, theta)
	hes_ran_.Dependent(a4_both, a4_hes);

	// optimize the recording
	hes_ran_.optimize();
}


} // END_DISMOD_AT_NAMESPACE
