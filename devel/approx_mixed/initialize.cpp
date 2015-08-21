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
	std
	obj
	vec
	const
	Cpp
	var
	hes
$$

$section approx_mixed: Initialization After Constructor$$

$head Syntax$$
$icode%size_map% = approx_object%.initialize(%fixed_vec%, %random_vec%)%$$

$head Purpose$$
Some of the $code approx_mixed$$ initialization requires calling the
derived class version of the
$cref/ran_like/approx_mixed_ran_like/$$ function.
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

$head size_map$$
The return value has prototype
$codei%
	std::map<std::string, size_t> %size_map%
%$$
It represent the size of certain aspects of the problem as follows:
$codei%

%size_map%["fix_like"]
%$$
	number of variables in the $code ADFun<double>$$ version of
	$cref/fix_like/approx_mixed_fix_like/$$.
$codei%

%size_map%["a1_ran_like"]
%$$
number of variables in the $code ADFun<a1_double>$$ version of
$cref/ran_like/approx_mixed_ran_like/$$.
$codei%

%size_map%["a0_ran_like"]
%$$
number of variables in the $code ADFun<double>$$ version of
$cref/ran_like/approx_mixed_ran_like/$$.
$codei%

%size_map%["hes_ran"]
%$$
number of non-zero entries in hessian of
random negative log-likelihood
$cref/f(theta, u)
	/approx_mixed_theory
	/Random Negative Log-Likelihood, f(theta, u)
/$$
with respect to fixed effects $latex \theta$$.
$codei%

%size_map%["newton_atom"]
%$$
number of variables in the $code ADFun<double>$$
object used to evaluate the newton step
$latex \[
	s = f_{uu}^{(2)} ( \theta , u )^{-1} v
\] $$
and the log of the determinant of the matrix being inverted.
$codei%

%size_map%["hes_fix"]
%$$
number of non-zero entries in hessian of fixed part of objective
$cref/g(theta)
	/approx_mixed_theory
	/Fixed Negative Log-Likelihood, g(theta)
/$$
with respect to the random effects $latex \theta$$.
$codei%

%size_map%["ran_obj_0"]
%$$
number of variables in the $code ADFun<double>$$
object used to evaluate the
$cref/random part of objective
	/approx_mixed_theory
	/Objective
	/Random Part of Objective, r(theta)
/$$.
$codei%

%size_map%["ran_obj_1"]
%$$
number of variables in the $code ADFun<double>$$
object used to evaluate the derivative, w.r.t fixed effects, of the
$cref/random part of objective
	/approx_mixed_theory
	/Objective
	/Random Part of Objective, r(theta)
/$$.
$codei%

%size_map%["ran_obj_2"]
%$$
number of variables in the $code ADFun<double>$$
object used to evaluate Hessian, w.r.t. fixed effects, of the
$cref/random part of objective
	/approx_mixed_theory
	/Objective
	/Random Part of Objective, r(theta)
/$$.

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
the member variable $codei%a%k%_ran_like_%$$ is the corresponding
$cref/ran_like_/approx_mixed_private/n_random_ > 0/ran_like_/$$.
$lnext
If $icode%n_random_ > 0%$$,
the member variable
$cref/hes_ran_/approx_mixed_private/n_random_ > 0/hes_ran_/$$.
$lnext
If $icode%n_random_ > 0%$$,
for $icode%k% = 0%, ... ,% 2%$$,
the member variable $codei%ran_obj_%k%_%$$ is the $th k$$ order
$cref/random objective/approx_mixed_private/n_random_ > 0/ran_obj_k_/$$.
$lnext
If $icode%n_random_ > 0%$$,
the member variable
$cref/hes_fix_/approx_mixed_private/n_random_ > 0/hes_fix_/$$.
$lnext
The member variable
$cref/fix_like_/approx_mixed_private/fix_like_/$$.
$lnext
The member variable
$cref/initialize_done_/approx_mixed_private/initialize_done_/$$
must be false when this routine is called and will be true when it returns.
$lend

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

std::map<std::string, size_t> approx_mixed::initialize(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	if( initialize_done_ )
	{	fatal_error("approx_mixed::initialize was called twice");
	}
	if( n_random_ > 0 )
	{
		assert( ! record_ran_like_done_ );
		assert( ! record_hes_ran_done_ );
		assert( ! record_ran_obj_done_[0] );
		assert( ! record_ran_obj_done_[1] );
		assert( ! record_ran_obj_done_[2] );

		// ran_like_
		record_ran_like(fixed_vec, random_vec);
		// hes_ran_
		record_hes_ran(fixed_vec, random_vec);
		// ran_obj_0_
		record_ran_obj(0, fixed_vec, random_vec);
		// ran_obj_1_
		record_ran_obj(1, fixed_vec, random_vec);
		// ran_obj_0_
		record_ran_obj(2, fixed_vec, random_vec);
		// hes_fix_
		record_hes_fix(fixed_vec, random_vec);

		assert( record_ran_like_done_ );
		assert( record_hes_ran_done_ );
		assert( record_ran_obj_done_[0] );
		assert( record_ran_obj_done_[1] );
		assert( record_ran_obj_done_[2] );
	}
	assert( ! record_fix_like_done_ );
	assert( ! record_constraint_done_ );

	// fix_like_
	record_fix_like(fixed_vec);
	// constraint_
	record_constraint(fixed_vec);

	assert( record_fix_like_done_ );
	assert( record_constraint_done_ );

	// initialize_done_
	initialize_done_ = true;

	// return value
	std::map<std::string, size_t> size_map;
	size_map["fix_like"]     = fix_like_.size_var();
	size_map["a0_ran_like"]  = a0_ran_like_.size_var();
	size_map["a1_ran_like"]  = a1_ran_like_.size_var();
	size_map["ran_obj_0"]    = ran_obj_0_.size_var();
	size_map["ran_obj_1"]    = ran_obj_1_.size_var();
	size_map["ran_obj_2"]    = ran_obj_2_.size_var();
	size_map["hes_ran"]      = hes_ran_row_.size();
	size_map["newton_atom"]  = newton_atom_.size_var();
	size_map["hes_fix"]      = hes_fix_row_.size();
	return size_map;
}

} // END_DISMOD_AT_NAMESPACE
