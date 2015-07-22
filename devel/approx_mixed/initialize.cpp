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
$begin approx_mixed_initialize$$
$spell
	vec
	const
	Cpp
	var
	hes
$$

$section approx_mixed: Initialization After Constructor$$

$head Syntax$$
$icode%approx_object%.initialize(%fixed_vec%, %random_vec%)%$$

$head Purpose$$
Some of the $code approx_mixed$$ initialization requires calling the
derived class version of the
$cref/joint_like/approx_mixed_joint_like/$$ function.
Hence this initialization cannot be done until
after the $cref/derived constructor/approx_mixed_derived_ctor/$$ completes.

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
vector $latex \theta$$ at which certain $code CppAD::ADFun$$
objects are recorded.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %random_vec%
%$$
It specifies the value of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$ at which certain $code CppAD::ADFun$$
objects are recorded.

$head Example$$
The file $cref approx_derived_xam.cpp$$ contains an example
of using $code initialize$$.

$head Private Member Variables$$
The following $code approx_mixed$$ private member variables are
$code CppAD::ADFun$$ functions that are assumed to be empty when
$code initialize$$ is called; i.e.,
the corresponding $code size_var()$$ is zero.
They will contain the corresponding recordings when $code initialize$$ returns:
$list number$$
If $icode%n_random_ > 0%$$,
for $icode%k% = 0%, ... ,% 4%$$,
the member variable $codei%a%k%_joint_like_%$$ is the corresponding
$cref/joint_like_/approx_mixed_private/n_random_ > 0/joint_like_/$$.
$lnext
If $icode%n_random_ > 0%$$,
the member variable
$cref/grad_ran_/approx_mixed_private/n_random_ > 0/grad_ran_/$$.
$lnext
If $icode%n_random_ > 0%$$,
the member variable
$cref/hes_ran_/approx_mixed_private/n_random_ > 0/hes_ran_/$$.
$lnext
If $icode%n_random_ > 0%$$,
for $icode%k% = 0%, ... ,% 2%$$,
the member variable $codei%laplace_%k%_%$$ is the $th k$$ order
$cref/Laplace approximation/approx_mixed_private/n_random_ > 0/laplace_k_/$$.
$lnext
If $icode%n_random_ > 0%$$,
the member variable
$cref/hes_fix_/approx_mixed_private/n_random_ > 0/hes_fix_/$$.
$lnext
The member variable
$cref/prior_like_/approx_mixed_private/prior_like_/$$.
$lnext
The member variable
$cref/initialize_done_/approx_mixed_private/initialize_done_/$$
must be false when this routine is called and will be true when it returns.
$lend

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void approx_mixed::initialize(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	if( initialize_done_ )
	{	fatal_error("approx_mixed::initialize was called twice");
	}
	if( n_random_ > 0 )
	{
		assert( ! record_joint_done_ );
		assert( ! record_grad_ran_done_ );
		assert( ! record_hes_ran_done_ );
		assert( ! record_laplace_done_[0] );
		assert( ! record_laplace_done_[1] );
		assert( ! record_laplace_done_[2] );

		// joint_like_
		record_joint(fixed_vec, random_vec);
		// grad_ran_
		record_grad_ran(fixed_vec, random_vec);
		// hes_ran_
		record_hes_ran(fixed_vec, random_vec);
		// laplace_0_
		record_laplace(0, fixed_vec, random_vec);
		// laplace_1_
		record_laplace(1, fixed_vec, random_vec);
		// laplace_0_
		record_laplace(2, fixed_vec, random_vec);
		// hes_fix_
		record_hes_fix(fixed_vec, random_vec);

		assert( record_joint_done_ );
		assert( record_grad_ran_done_ );
		assert( record_hes_ran_done_ );
		assert( record_laplace_done_[0] );
		assert( record_laplace_done_[1] );
		assert( record_laplace_done_[2] );
	}
	assert( ! record_prior_done_ );
	assert( ! record_constraint_done_ );

	// prior_like_
	record_prior(fixed_vec);
	// constraint_
	record_constraint(fixed_vec);

	assert( record_prior_done_ );
	assert( record_constraint_done_ );

	// initialize_done_
	initialize_done_ = true;
}

} // END_DISMOD_AT_NAMESPACE
