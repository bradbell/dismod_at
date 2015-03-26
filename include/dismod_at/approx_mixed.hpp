// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_APPROX_MIXED_HPP
# define DISMOD_AT_APPROX_MIXED_HPP
# include <cppad/cppad.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

# define DISMOD_AT_DEFINE_JOINT_DENSITY(Float)        \
	virtual CppAD::vector< Float > joint_density(     \
		const CppAD::vector< Float >& fixed_vec  ,    \
		const CppAD::vector< Float >& random_vec ) = 0;

class approx_mixed {
public:
	// density for data and random effects
	// (pure vritual function so must be defined by derived class)
	DISMOD_AT_DEFINE_JOINT_DENSITY( double )
	DISMOD_AT_DEFINE_JOINT_DENSITY( CppAD::AD<double> )
	DISMOD_AT_DEFINE_JOINT_DENSITY( CppAD::AD< CppAD::AD<double> > )

	// optimize_random
	CppAD::vector<double> optimize_random(
		const CppAD::vector<double>& fixed_vec ,
		const CppAD::vector<double>& random_in
	);
};

} // END_DISMOD_AT_NAMESPACE

# endif
