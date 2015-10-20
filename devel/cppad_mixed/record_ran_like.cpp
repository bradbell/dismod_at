// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/cppad_mixed.hpp>

/*
$begin cppad_mixed_record_ran_like$$
$spell
	cppad
	vec
	const
	Cpp
$$

$section cppad_mixed: Record Random Negative Log-Likelihood For Different Levels of AD$$

$head Syntax$$
$codei%record_ran_like(%fixed_vec%, %random_vec%)%$$

$head Private$$
This function is $code private$$ to the $code cppad_mixed$$ class
and cannot be used by a derived
$cref/mixed_object/cppad_mixed_derived_ctor/mixed_object/$$.

$head fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %fixed_vec%
%$$
It specifies the value of the
$cref/fixed effects/cppad_mixed/Fixed Effects, theta/$$
vector $latex \theta$$ at which the recording is made.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %random_vec%
%$$
It specifies the value of the
$cref/random effects/cppad_mixed/Random Effects, u/$$
vector $latex u$$ at which the recording is made.

$head ran_like_$$
The input value of the member variables
$codei%
	CppAD::ADFun<a%k%_double> a0_ran_like_
	CppAD::ADFun<a%k%_double> a1_ran_like_
%$$
do not matter.
Upon return they contain the corresponding recording for the
$cref/ran_like/cppad_mixed_ran_like/$$.
Note that the function result is the
$cref/negative log-density vector
	/cppad_mixed
	/Negative Log-Density Vector
/$$
corresponding to the function
$cref/f(theta , u )
	/cppad_mixed_theory/
	Random Negative Log-Likelihood, f(theta, u)
/$$.

$end
*/
namespace {
	using CppAD::AD;
	using CppAD::ADFun;
	using CppAD::vector;
	using CppAD::Independent;

	template <class Float>
	void record_next_ran_like(
		const vector<double>&      arg                   ,
		ADFun< AD<Float> >&        ad_float_record       ,
		ADFun<Float>&              float_record          )
	{
		// copy of arg with type AD<Float>
		vector< AD<Float> > ad_arg( arg.size() );
		for(size_t j = 0; j < arg.size(); j++)
			ad_arg[j] = AD<Float> ( arg[j] );

		// independent variables for this recording
		Independent(ad_arg);

		// compute ran_like using AD<Float> operations
		vector< AD<Float> >  ad_vec = ad_float_record.Forward(0, ad_arg);

		// save the recording
		float_record.Dependent(ad_arg, ad_vec);

		// optimize the recording
		float_record.optimize();
	}
}

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void cppad_mixed::record_ran_like(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	assert( ! record_ran_like_done_ );
	// ------------------------------------------------------------------
	// record a1_ran_like_
	// ------------------------------------------------------------------
	// combine into one vector
	a2d_vector a2_both( n_fixed_ + n_random_ );
	pack(fixed_vec, random_vec, a2_both);

	// start recording a2_double operations
	Independent(a2_both);

	// extract the fixed and random effects
	a2d_vector a2_theta(n_fixed_), a2_u(n_random_);
	unpack(a2_theta, a2_u, a2_both);

	// compute ran_like using a2_double operations
	a2d_vector a2_vec = ran_like(a2_theta, a2_u);
	if( a2_vec.size() == 0 )
	{	std::string error_message =
		"cppad_mixed: number of random effects > 0 and ran_like has size 0";
		fatal_error(error_message);
	}
	if( a2_vec.size() != 1 )
	{	std::string error_message =
		"cppad_mixed: ran_like does not have size zero or one.";
		fatal_error(error_message);
	}

	// save the recording
	a1_ran_like_.Dependent(a2_both, a2_vec);

	// optimize the recording
	a1_ran_like_.optimize();
	// ------------------------------------------------------------------
	//
	// both
	d_vector both(n_fixed_ + n_random_);
	pack(fixed_vec, random_vec, both);
	//
	// record a0_ran_like_
	record_next_ran_like(both, a1_ran_like_, a0_ran_like_);
	//
	record_ran_like_done_ = true;
}


} // END_DISMOD_AT_NAMESPACE
