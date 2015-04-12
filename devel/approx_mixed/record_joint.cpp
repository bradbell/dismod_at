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
$begin approx_mixed_record_joint$$
$spell
	vec
	const
	Cpp
$$

$section approx_mixed: Record Joint Density For Different Levels of AD$$

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

$head joint_density_$$
For $icode%k% = 0, 1, 2, 3%$$, the input value of the member variable
$codei%
	CppAD::ADFun<a%k%_double> a%k%_joint_density_
%$$
does not matter.
Upon return it contains the corresponding recording for the
$cref/joint_density/approx_mixed_joint_density/$$
$latex f ( \theta , u )$$.

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

		// compute joint_density using AD<Float> operations
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
{
	// ------------------------------------------------------------------
	// record a3_joint_density_
	// ------------------------------------------------------------------
	// combine into one vector
	a4d_vector a4_both( n_fixed_ + n_random_ );
	pack(fixed_vec, random_vec, a4_both);

	// start recording a4_double operations
	Independent(a4_both);

	// extract the fixed and random effects
	a4d_vector a4_theta(n_fixed_), a4_u(n_random_);
	unpack(a4_theta, a4_u, a4_both);

	// compute joint_density using a4_double operations
	a4d_vector a4_vec = joint_density(a4_theta, a4_u);

	// save the recording
	a3_joint_density_.Dependent(a4_both, a4_vec);

	// optimize the recording
	a3_joint_density_.optimize();
	// ------------------------------------------------------------------
	//
	// both
	d_vector both(n_fixed_ + n_random_);
	pack(fixed_vec, random_vec, both);
	//
	// record a2_joint_density_
	record_next_joint(both, a3_joint_density_, a2_joint_density_);
	//
	// record a1_joint_density_
	record_next_joint(both, a2_joint_density_, a1_joint_density_);
	//
	// record a0_joint_density_
	a1d_vector a1_both( n_fixed_ + n_random_ );
	record_next_joint(both, a1_joint_density_, a0_joint_density_);
}


} // END_DISMOD_AT_NAMESPACE
