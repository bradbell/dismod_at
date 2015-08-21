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
$begin approx_mixed_record_hes_ran$$
$spell
	hes hes
	vec
	const
	Cpp
	logdet
$$

$section Record Hessian of Random Negative Log-Likelihood w.r.t Random Effects$$

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

$head hes_ran_row_, hes_ran_col_$$
The input value of the member variables
$codei%
	CppAD::vector<size_t> hes_ran_row_, hes_ran_col_
%$$
do not matter.
Upon return they contain the row indices and column indices
for the sparse Hessian represented by $code hes_ran_$$; i.e.
$codei%hes_ran_row_[%i%] - n_fixed_%$$
and
$codei%hes_ran_col_[%i%] - n_fixed_%$$
are the row and column indices for the Hessian element
that corresponds to the $th i$$ component of the function
corresponding to $code hes_ran_$$.

$head hes_ran_work_$$
The input value of the member variable
$codei%
	CppAD::sparse_hessian_work hes_ran_work_
%$$
does not matter.
Upon return it contains the necessary information so that
$codei%
	a1_ran_like_.SparseHessian(
		%a1_both_vec%,
		%a1_w%,
		%not_used%,
		hes_ran_row_,
		hes_ran_col_,
		%a1_val_out%,
		hes_ran_work_
	);
%$$
can be used to calculate the lower triangle of the sparse Hessian
$latex \[
	f_{uu}^{(2)} ( \theta , u )
\]$$
see $cref/f(theta, u)/
	approx_mixed_theory/
	Random Negative Log-Likelihood, f(theta, u)
/$$.
Note that the matrix is symmetric and hence can be recovered from
its lower triangle.

$head newton_atom_$$
The input value of the member variable
$codei%
	newton_step newton_atom_
%$$
must be the same as after its constructor; i.e.,
no member functions had been called.
Upon return, $code newton_atom_$$ can be used to compute
the log of the determinant and the Newton step using
$codei%
	newton_atom_(%a1_theta_u_v%, %a1_logdet_step%)
%$$
be more specific, $icode%a1_logdet_step%[0]%$$ is the log of the determinant of
$latex f_{uu}^{(2)} ( \theta , u ) $$ and the rest of the vector is the
Newton step
$latex \[
	s = f_{uu}^{(2)} ( \theta , u )^{-1} v
\] $$


$contents%devel/approx_mixed/newton_step.cpp
%$$

$end
*/

# define DISMOD_AT_SET_SPARSITY 1

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void approx_mixed::record_hes_ran(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	assert( ! record_hes_ran_done_ );
	assert( fixed_vec.size() == n_fixed_ );
	assert( random_vec.size() == n_random_ );
	size_t i, j;

	// total number of variables
	size_t n_total = n_fixed_ + n_random_;

	//	create an a1d_vector containing (theta, u)
	a1d_vector a1_both(n_total);
	pack(fixed_vec, random_vec, a1_both);

	// compute Jacobian sparsity corresponding to parital w.r.t. random effects
# if DISMOD_AT_SET_SPARSITY
	typedef CppAD::vector< std::set<size_t> > sparsity_pattern;
	sparsity_pattern r(n_total);
	for(i = n_fixed_; i < n_total; i++)
		r[i].insert(i);
# else
	typedef CppAD::vectorBool sparsity_pattern;
	sparsity_pattern r(n_total * n_total);
	for(i = 0; i < n_total; i++)
	{	for(j = 0; j < n_total; j++)
			r[i * n_total + j] = (i >= n_fixed_) && (i == j);
	}
# endif
	a1_ran_like_.ForSparseJac(n_total, r);

	// compute sparsity pattern corresponding to paritls w.r.t. (theta, u)
	// of partial w.r.t. u of f(theta, u)
	bool transpose = true;
	sparsity_pattern s(1), pattern;
# if DISMOD_AT_SET_SPARSITY
	assert( s[0].empty() );
	s[0].insert(0);
# else
	s[0] = true;
# endif
	pattern = a1_ran_like_.RevSparseHes(n_total, s, transpose);

	// compute hes_ran_row_ and hes_ran_col_
	// determine row and column indices in lower triangle of Hessian
	hes_ran_row_.clear();
	hes_ran_col_.clear();
# if DISMOD_AT_SET_SPARSITY
	std::set<size_t>::iterator itr;
	for(i = n_fixed_; i < n_total; i++)
	{	for(itr = pattern[i].begin(); itr != pattern[i].end(); itr++)
		{	j = *itr;
			assert( j >= n_fixed_ );
			// only compute lower triangular part of Hessian w.r.t u only
			if( i >= j )
			{	hes_ran_row_.push_back(i);
				hes_ran_col_.push_back(j);
			}
		}
	}
# else
	for(i = n_fixed_; i < n_total; i++)
	{	for(j = n_fixed_; j < n_total; j++)
		{	if( pattern[i * n_total + j] && i >= j )
			{	// only compute lower triangular of Hessian w.r.t u only
				if( i >= j )
				{	hes_ran_row_.push_back(i);
					hes_ran_col_.push_back(j);
				}
			}
		}
	}
# endif

	// create a weighting vector
	a1d_vector a1_w(1);
	a1_w[0] = 1.0;

	// place where results go (not usd here)
	a1d_vector a1_val_out( hes_ran_row_.size() );

	// compute the work vector
	a1_ran_like_.SparseHessian(
		a1_both,
		a1_w,
		pattern,
		hes_ran_row_,
		hes_ran_col_,
		a1_val_out,
		hes_ran_work_
	);
	//
	newton_atom_.initialize(a1_ran_like_, fixed_vec, random_vec);
	//
	record_hes_ran_done_ = true;
}


} // END_DISMOD_AT_NAMESPACE
