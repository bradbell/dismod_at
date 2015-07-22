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
$begin approx_mixed_record_joint$$
$spell
	vec
	const
	Cpp
$$

$section approx_mixed: Record Joint Negative Log-Likelihood For Different Levels of AD$$

$head Syntax$$
$codei%record_joint(%fixed_vec%, %random_vec%)%$$

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

$head joint_like_$$
For $icode%k% = 0, 1, 2, 3, 4%$$ the input value of the member variable
$codei%
	CppAD::ADFun<a%k%_double> a%k%_joint_like_
%$$
does not matter.
Upon return it contains the corresponding recording for the
$cref/joint_like/approx_mixed_joint_like/$$.
Note that the function result is the
$cref/negative log-density vector/approx_mixed/Negative Log-Density Vector/$$
corresponding to the function
$cref/f(theta , u )/approx_mixed_theory/Joint Negative Log-Likelihood, f(theta, u)/$$.

$end
*/
namespace {
	using CppAD::AD;
	using CppAD::ADFun;
	using CppAD::vector;
	using CppAD::Independent;

	template <class Float>
	void record_next_joint(
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

		// compute joint_like using AD<Float> operations
		vector< AD<Float> >  ad_vec = ad_float_record.Forward(0, ad_arg);

		// save the recording
		float_record.Dependent(ad_arg, ad_vec);

		// optimize the recording
		float_record.optimize();
	}
}

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void approx_mixed::record_joint(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	assert( ! record_joint_done_ );
	// ------------------------------------------------------------------
	// record a3_joint_like_
	// ------------------------------------------------------------------
	// combine into one vector
	a5d_vector a5_both( n_fixed_ + n_random_ );
	pack(fixed_vec, random_vec, a5_both);

	// start recording a5_double operations
	Independent(a5_both);

	// extract the fixed and random effects
	a5d_vector a5_theta(n_fixed_), a5_u(n_random_);
	unpack(a5_theta, a5_u, a5_both);

	// compute joint_like using a5_double operations
	a5d_vector a5_vec = joint_like(a5_theta, a5_u);

	// save the recording
	a4_joint_like_.Dependent(a5_both, a5_vec);

	// optimize the recording
	a4_joint_like_.optimize();
	// ------------------------------------------------------------------
	//
	// both
	d_vector both(n_fixed_ + n_random_);
	pack(fixed_vec, random_vec, both);
	//
	// record a3_joint_like_
	record_next_joint(both, a4_joint_like_, a3_joint_like_);
	//
	// record a2_joint_like_
	record_next_joint(both, a3_joint_like_, a2_joint_like_);
	//
	// record a1_joint_like_
	record_next_joint(both, a2_joint_like_, a1_joint_like_);
	//
	// record a0_joint_like_
	a1d_vector a1_both( n_fixed_ + n_random_ );
	record_next_joint(both, a1_joint_like_, a0_joint_like_);
	//
	record_joint_done_ = true;
}


} // END_DISMOD_AT_NAMESPACE
