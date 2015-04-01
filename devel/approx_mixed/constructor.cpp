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

approx_mixed::approx_mixed(
	const d_vector& fixed_vec  ,
	const d_vector& random_vec )
	:
	n_fixed_  ( fixed_vec.size()  )   ,
	n_random_ ( random_vec.size() )
{
# if 0
	size_t j;

	//	create an a2d_vector containing (theta, u)
	a2d_vector a2_both( n_fixed_ + n_random_ );
	for(j = 0; i < n_fixed_; j++)
		a2_both[j] = a2_double( fixed_vec[j] );
	for(j = 0; i < n_random_; j++)
		a2_both[n_fixed_ + j] = a2_double( random_vec[j] );

	// start recording f(u, theta) using a2_double operaitons
	CppAD::Independent( a2_both )

	// extract the fixed and random effects
	a2_vector a2_fixed_vec(n_fixed_);
	a2_vector a2_rnadom_vec(n_random_);
# endif


}


} // END_DISMOD_AT_NAMESPACE
