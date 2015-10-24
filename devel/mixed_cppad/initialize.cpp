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
$begin mixed_cppad_initialize$$
$spell
	logdet
	cppad
	std
	obj
	vec
	const
	Cpp
	var
	hes
	num
	alloc
$$

$section mixed_cppad: Initialization After Constructor$$

$head Syntax$$
$icode%size_map% = mixed_object%.initialize(%fixed_vec%, %random_vec%)%$$

$head Purpose$$
Some of the $code mixed_cppad$$ initialization requires calling the
derived class version of the
$cref/ran_like/mixed_cppad_ran_like/$$ function.
Hence this initialization cannot be done until
after the $cref/derived constructor/mixed_cppad_derived_ctor/$$ completes.

$head mixed_object$$
We use $cref/mixed_object/mixed_cppad_derived_ctor/mixed_object/$$
to denote an object of a class that is
derived from the $code mixed_cppad$$ base class.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_vec%
%$$
It specifies the value of the
$cref/fixed effects/mixed_cppad/Fixed Effects, theta/$$
vector $latex \theta$$ at which certain $code CppAD::ADFun$$
objects are recorded.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %random_vec%
%$$
It specifies the value of the
$cref/random effects/mixed_cppad/Random Effects, u/$$
vector $latex u$$ at which certain $code CppAD::ADFun$$
objects are recorded.

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
	$cref/fix_like/mixed_cppad_fix_like/$$.
$codei%

%size_map%["a1_ran_like"]
%$$
number of variables in the $code ADFun<a1_double>$$ version of
$cref/ran_like/mixed_cppad_ran_like/$$.
$codei%

%size_map%["a0_ran_like"]
%$$
number of variables in the $code ADFun<double>$$ version of
$cref/ran_like/mixed_cppad_ran_like/$$.
$codei%

%size_map%["hes_ran"]
%$$
number of non-zero entries in hessian of
random negative log-likelihood
$cref/f(theta, u)
	/mixed_cppad_theory
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
	/mixed_cppad_theory
	/Fixed Negative Log-Likelihood, g(theta)
/$$
with respect to the random effects $latex \theta$$.
$codei%

%size_map%["ran_obj_2"]
%$$
number of variables in the $code ADFun<double>$$
object used to evaluate Hessian, w.r.t. fixed effects, of the
$cref/random part of objective
	/mixed_cppad_theory
	/Objective
	/Random Part of Objective, r(theta)
/$$.
$codei%

%size_map%["num_bytes"]
%$$
number of bytes currently allocated by $code CppAD::thread_alloc$$
for the current thread.

$head Example$$
The file $cref mixed_derived_xam.cpp$$ contains an example
of using $code initialize$$.

$head Private Member Variables$$
The following $code mixed_cppad$$ private member variables are
$code CppAD::ADFun$$ functions that are assumed to be empty when
$code initialize$$ is called; i.e.,
the corresponding $code size_var()$$ is zero.
They will contain the corresponding recordings when $code initialize$$ returns:
$list number$$
If $icode%n_random_ > 0%$$,
for $icode%k% = 0%, ... ,% 4%$$,
the member variable $codei%a%k%_ran_like_%$$ is the corresponding
$cref/ran_like_/mixed_cppad_private/n_random_ > 0/ran_like_/$$.
$lnext
If $icode%n_random_ > 0%$$,
the member variable
$cref/hes_ran_/mixed_cppad_private/n_random_ > 0/hes_ran_/$$.
$lnext
If $icode%n_random_ > 0%$$,
the member variable $codei%ran_obj_2_%$$ is the second order
$cref/random objective/mixed_cppad_private/n_random_ > 0/ran_obj_2_/$$.
$lnext
If $icode%n_random_ > 0%$$,
the member variable
$cref/hes_fix_/mixed_cppad_private/n_random_ > 0/hes_fix_/$$.
$lnext
The member variable
$cref/fix_like_/mixed_cppad_private/fix_like_/$$.
$lnext
The member variable
$cref/initialize_done_/mixed_cppad_private/initialize_done_/$$
must be false when this routine is called and will be true when it returns.
$lend

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

std::map<std::string, size_t> mixed_cppad::initialize(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	if( initialize_done_ )
	{	fatal_error("mixed_cppad::initialize was called twice");
	}
	if( n_random_ > 0 )
	{

		// ran_like_
		assert( ! record_ran_like_done_ );
		record_ran_like(fixed_vec, random_vec);
		assert( record_ran_like_done_ );

		// hes_ran_
		assert( ! record_hes_ran_done_ );
		record_hes_ran(fixed_vec, random_vec);
		assert( record_hes_ran_done_ );

		// hes_cross_
		assert( ! record_hes_cross_done_ );
		record_hes_cross(fixed_vec, random_vec);
		assert( record_hes_cross_done_ );

# if MIXED_CPPAD_NEWTON
		// newton_atom_
		assert( ! record_newton_atom_done_ );
		newton_atom_.initialize(a1_ran_like_, fixed_vec, random_vec);
		record_newton_atom_done_ = true;

		// ran_obj_2_
		assert( ! record_ran_obj_done_ );
		record_ran_obj(2, fixed_vec, random_vec);
		assert( record_ran_obj_done_ );

		// hes_fix_
		assert( ! record_hes_fix_done_ );
		record_hes_fix(fixed_vec, random_vec);
		assert( record_hes_fix_done_ );
# endif
	}

	// fix_like_
	assert( ! record_fix_like_done_ );
	record_fix_like(fixed_vec);
	assert( record_fix_like_done_ );

	// constraint_
	assert( ! record_constraint_done_ );
	record_constraint(fixed_vec);
	assert( record_constraint_done_ );

	// initialize_done_
	initialize_done_ = true;

	// return value
	std::map<std::string, size_t> size_map;
	size_map["fix_like"]     = fix_like_.size_var();
	size_map["a0_ran_like"]  = a0_ran_like_.size_var();
	size_map["a1_ran_like"]  = a1_ran_like_.size_var();
	size_map["ran_obj_2"]    = ran_obj_2_.size_var();
	size_map["hes_ran"]      = hes_ran_row_.size();
	size_map["newton_atom"]  = newton_atom_.size_var();
	size_map["hes_fix"]      = hes_fix_row_.size();
	size_t thread            = CppAD::thread_alloc::thread_num();
	size_map["num_bytes"]    = CppAD::thread_alloc::inuse(thread);
	return size_map;
}

} // END_DISMOD_AT_NAMESPACE
