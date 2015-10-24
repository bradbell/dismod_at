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

$head size_map$$
The return value has prototype
$codei%
	std::map<std::string, size_t> %size_map%
%$$
It represent the size of certain aspects of the problem.
For each of $code mixed_cppad$$ member variables listed below
$codei%
	%size_map%[%member_variable%]
%$$
is the corresponding size.

$subhead hes_ran_row$$
$icode%size_map%["hes_ran_row"]%$$ is the
number of non-zero entries in hessian of
random negative log-likelihood w.r.t random effects
$latex f_{uu}^{(2)} ( \theta , u )$$.

$subhead hes_cross_row$$
$icode%size_map%["hes_cross_row"]%$$ is the
number of non-zero cross entries in hessian of
random negative log-likelihood w.r.t fixed and random effects
$latex f_{u \theta}^{(2)} ( \theta , u )$$.

$subhead a0_ran_like$$
$icode%size_map%["a0_ran_like"]%$$ is the
number of variables in the $code ADFun<double>$$ version of
$cref/ran_like/mixed_cppad_ran_like/$$.

$subhead a1_ran_like$$
$icode%size_map%["a1_ran_like"]%$$ is the
number of variables in the $code ADFun<a1_double>$$ version of
$cref/ran_like/mixed_cppad_ran_like/$$.

$subhead hes_ran_0$$
$icode%size_map%["hes_ran_0_"]%$$ is the
number of variables in the $code ADFun<double>$$ object that
computes the Hessian with respect to the random effects.

$subhead newton_atom$$
$icode%size_map%["newton_atom"]%$$ is the
number of variables in the $code ADFun<double>$$
object used to evaluate the newton step
$latex \[
	s = f_{uu}^{(2)} ( \theta , u )^{-1} v
\] $$
and the log of the determinant of the matrix being inverted.

$subhead ran_obj_2$$
$icode%size_map%["ran_obj_2"]%$$ is the
number of variables in the $code ADFun<double>$$
object used to evaluate Hessian, w.r.t. fixed effects, of the objective
$latex r^{(2)} ( \theta )$$.

$subhead fix_like$$
$icode%size_map%["fix_like"]%$$ is the
number of variables in the $code ADFun<double>$$ function
that computes the fixed negative log likelihood $latex g( \theta )$$.

$subhead constraint$$
$icode%size_map%["constraint"]%$$ is the
number of variables in the $code ADFun<double>$$ function
that computes the general constraints for the fixed effects.

$subhead num_bytes$$
Is the number of bytes currently allocated by
$code CppAD::thread_alloc$$ for the current thread.

$head Example$$
The file $cref mixed_derived_xam.cpp$$ contains an example
of using $code initialize$$.

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
	size_map["hes_ran_row"]   = hes_ran_row_.size();
	size_map["hes_cross_row"] = hes_cross_row_.size();
	size_map["a0_ran_like"]   = a0_ran_like_.size_var();
	size_map["a1_ran_like"]   = a1_ran_like_.size_var();
	size_map["hes_ran_0"]     = hes_ran_0_.size_var();
	size_map["newton_atom"]   = newton_atom_.size_var();
	size_map["ran_obj_2"]     = ran_obj_2_.size_var();
	size_map["fix_like"]      = fix_like_.size_var();
	size_map["constraint"]    = constraint_.size_var();
	//
	size_t thread             = CppAD::thread_alloc::thread_num();
	size_map["num_bytes"]     = CppAD::thread_alloc::inuse(thread);
	return size_map;
}

} // END_DISMOD_AT_NAMESPACE
