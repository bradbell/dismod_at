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

	// set a1d_f(theta, u)
	joint_.Dependent(both, sum);
}


} // END_DISMOD_AT_NAMESPACE
