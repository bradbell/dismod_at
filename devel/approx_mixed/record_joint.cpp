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

$section approx_mixed: Record the Joint Density Evaluator Function$$

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
vector $latex \theta$$ at which the member variable
$codei%
	CppAD::ADFun<a1_double> joint_
%$$ is recorded.

$head random_vec$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %random_vec%
%$$
It specifies the value of the
$cref/random effects/approx_mixed/Random Effects, u/$$
vector $latex u$$ at which the member variable
$codei%
	CppAD::ADFun<a1_double> joint_
%$$ is recorded.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void approx_mixed::record_joint(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
{	size_t i, j;

	//	create an a2d_vector containing (theta, u)
	a2d_vector both( n_fixed_ + n_random_ );
	for(j = 0; j < n_fixed_; j++)
		both[j] = a2_double( fixed_vec[j] );
	for(j = 0; j < n_random_; j++)
		both[n_fixed_ + j] = a2_double( random_vec[j] );

	// start recording f(theta,u) using a2_double operaitons
	CppAD::Independent( both );

	// extract the fixed and random effects
	a2d_vector theta(n_fixed_), u(n_random_);
	for(j = 0; j < n_fixed_; j++)
		theta[j] = both[j];
	for(j = 0; j < n_random_; j++)
		u[j] = both[n_fixed_ + j];

	// compute f(theta, u)
	a2d_vector vec = joint_density(theta, u);
	a2d_vector sum(1);
	sum[0]       = vec[0];
	size_t n_abs = vec.size() - 1;
	for(i = 0; i < n_abs; i++)
		sum[0] += abs( vec[1 + j] );

	// set f(theta, u)
	joint_.Dependent(both, sum);
}


} // END_DISMOD_AT_NAMESPACE
