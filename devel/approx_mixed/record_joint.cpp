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
For $icode%k% = 0, 1, 2%$$, the input value of the member variable
$codei%
	CppAD::ADFun<a%k%_double> a%k%_joint_density_
%$$
does not matter.
Upon return it contains the corresponding recording for the
$cref/joint_density/approx_mixed_joint_density/$$
$latex f ( \theta , u )$$.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void approx_mixed::record_joint(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	size_t j;

	// ------------------------------------------------------------------
	// record a2_joint_density_
	// ------------------------------------------------------------------
	// combine into one vector
	a3d_vector a3_both( n_fixed_ + n_random_ );
	for(j = 0; j < n_fixed_; j++)
		a3_both[j] = a3_double( fixed_vec[j] );
	for(j = 0; j < n_random_; j++)
		a3_both[n_fixed_ + j] = a3_double( random_vec[j] );

	// start recording a3_double operations
	CppAD::Independent(a3_both);

	// extract the fixed and random effects
	a3d_vector a3_theta(n_fixed_), a3_u(n_random_);
	for(j = 0; j < n_fixed_; j++)
		a3_theta[j] = a3_both[j];
	for(j = 0; j < n_random_; j++)
		a3_u[j] = a3_both[n_fixed_ + j];

	// compute joint_density using a3_double operations
	a3d_vector a3_vec = joint_density(a3_theta, a3_u);

	// save the recording 
	a2_joint_density_.Dependent(a3_both, a3_vec);

	// optimize the recording
	a2_joint_density_.optimize();

	// ------------------------------------------------------------------
	// record a1_joint_density_
	// ------------------------------------------------------------------
	a2d_vector a2_both( n_fixed_ + n_random_ );
	for(size_t j = 0; j < n_fixed_ + n_random_; j++)
		a2_both[j] = Value( a3_both[j] );
	CppAD::Independent(a2_both);

	// compute joint_density using a3_double operations
	a2d_vector a2_vec = a2_joint_density_.Forward(0, a2_both);

	// save the result 
	a1_joint_density_.Dependent(a2_both, a2_vec);

	// optimize the recording
	a1_joint_density_.optimize();

	// ------------------------------------------------------------------
	// record a0_joint_density_
	// ------------------------------------------------------------------
	a1d_vector a1_both( n_fixed_ + n_random_ );
	for(size_t j = 0; j < n_fixed_ + n_random_; j++)
		a1_both[j] = Value( a2_both[j] );
	CppAD::Independent(a1_both);

	// compute joint_density using a3_double operations
	a1d_vector a1_vec = a1_joint_density_.Forward(0, a1_both);

	// save the result 
	a0_joint_density_.Dependent(a1_both, a1_vec);

	// optimize the recording
	a0_joint_density_.optimize();
}


} // END_DISMOD_AT_NAMESPACE
