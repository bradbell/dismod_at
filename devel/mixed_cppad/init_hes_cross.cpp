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
# include <dismod_at/configure.hpp>

/*
$begin init_hes_cross$$
$spell
	init
	cppad
	hes hes
	vec
	const
	Cpp
	logdet
$$

$section Cross Terms of Sparse Hessian w.r.t Fixed and Random Effects$$

$head Syntax$$
$codei%init_hes_cross(%fixed_vec%, %random_vec%)%$$

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
vector $latex \theta$$ at which the initialization is done.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %random_vec%
%$$
It specifies the value of the
$cref/random effects/mixed_cppad/Random Effects, u/$$
vector $latex u$$ at which the initialization is done.

$head hes_cross_row_$$
The input value of this member variable does not matter.
Upon return
$codei%
	hes_cross_row_[%k%] - n_fixed_
%$$
is the random effects index for this cross partial in
$latex f_{u \theta}^{(2)}$$.

$head hes_cross_col_$$
The input value of this member variable does not matter.
Upon return
$codei%
	hes_cross_col_[%k%]
%$$
is the fixed effects index for this cross partial in
$latex f_{u \theta}^{(2)}$$.

$head Order$$
The results are in column major order; i.e.,
$codei%
	hes_cross_col_[%k%] <= hes_cross_col_[%k+1%]
	if( hes_cross_col_[%k%] == hes_cross_col_[%k+1%] )
		hes_cross_row_[%k%] < hes_cross_row_[%k+1%]
%$$

$head hes_cross_work_$$
The input value of the member variable
$codei%
	CppAD::sparse_hessian_work hes_cross_work_
%$$
does not matter.
Upon return it contains the necessary information so that
$codei%
	ran_like_fun_.SparseHessian(
		%both_vec%,
		%w%,
		%not_used%,
		hes_cross_row_,
		hes_cross_col_,
		%val_out%,
		hes_cross_work_
	);
%$$
can be used to calculate the non-zero cross terms in
$latex f_{u \theta}^{(2)}$$.

$children%
	example/devel/mixed_cppad/private/hes_cross_xam.cpp
%$$
$head Example$$
The file $cref hes_cross_xam.cpp$$ contains an example
and test of this procedure.
It returns true, if the test passes, and false otherwise.

$end
*/


namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void mixed_cppad::init_hes_cross(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{
	assert( ! init_hes_cross_done_ );
	assert( fixed_vec.size() == n_fixed_ );
	assert( random_vec.size() == n_random_ );
	size_t i, j;

	// total number of variables
	size_t n_total = n_fixed_ + n_random_;

	// create a d_vector containing (theta, u)
	d_vector both(n_total);
	pack(fixed_vec, random_vec, both);

	// compute Jacobian sparsity corresponding to parital w.r.t. fixed effects
# if MIXED_CPPAD_SET_SPARSITY
	typedef CppAD::vector< std::set<size_t> > sparsity_pattern;
	sparsity_pattern r(n_total);
	for(i = 0; i < n_fixed_; i++)
		r[i].insert(i);
# else
	typedef CppAD::vectorBool sparsity_pattern;
	sparsity_pattern r(n_total * n_total);
	for(i = 0; i < n_total; i++)
	{	for(j = 0; j < n_total; j++)
			r[i * n_total + j] = (i < n_fixed_) && (i == j);
	}
# endif
	ran_like_fun_.ForSparseJac(n_total, r);

	// compute sparsity pattern corresponding to paritls w.r.t. (theta, u)
	// of partial w.r.t. theta of f(theta, u)
	bool transpose = true;
	sparsity_pattern s(1), pattern;
# if MIXED_CPPAD_SET_SPARSITY
	assert( s[0].empty() );
	s[0].insert(0);
# else
	s[0] = true;
# endif
	pattern = ran_like_fun_.RevSparseHes(n_total, s, transpose);


	// User row index for random effect and column index for fixed effect
	CppAD::vector<size_t> row, col, key;
# if MIXED_CPPAD_SET_SPARSITY
	std::set<size_t>::iterator itr;
	for(i = n_fixed_; i < n_total; i++)
	{	for(itr = pattern[i].begin(); itr != pattern[i].end(); itr++)
		{	j = *itr;
			assert( j < n_fixed_ );
			row.push_back(i);
			col.push_back(j);
			key.push_back( i + j * n_total );
		}
	}
# else
	for(i = n_fixed_; i < n_total; i++)
	{	for(j = 0; j < n_fixed_; j++)
		{	if( pattern[i * n_total + j] )
			{	row.push_back(i);
				col.push_back(j);
				key.push_back( i + j * n_total );
			}
		}
	}
# endif
	// set hes_cross_row_ and hes_cross_col_ in colum major order
	size_t K = row.size();
	CppAD::vector<size_t> ind(K);
	CppAD::index_sort(key, ind);
	hes_cross_row_.resize(K);
	hes_cross_col_.resize(K);
	for(size_t k = 0; k < row.size(); k++)
	{	hes_cross_row_[k] = row[ ind[k] ];
		hes_cross_col_[k] = col[ ind[k] ];
	}

	// create a weighting vector
	d_vector w(1);
	w[0] = 1.0;

	// place where results go (not usd here)
	d_vector val_out(K);

	// compute the work vector
	ran_like_fun_.SparseHessian(
		both,
		w,
		pattern,
		hes_cross_row_,
		hes_cross_col_,
		val_out,
		hes_cross_work_
	);
	//
	init_hes_cross_done_ = true;
}


} // END_DISMOD_AT_NAMESPACE
