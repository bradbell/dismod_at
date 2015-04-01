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
private:
	// private typedefs -------------------------------------
	typedef CppAD::AD<double>                a1_double;
	typedef CppAD::AD< CppAD::AD<double> >   a2_double;
	typedef CppAD::vector<double>            d_vector;
	typedef CppAD::vector<a1_double>         a1d_vector;
	typedef CppAD::vector<a2_double>         a2d_vector;
	// private data -------------------------------------------
	const size_t n_fixed_;        // number of fixed effects
	const size_t n_random_;       // number of random effects

	// joint likelihood of fixed, random effects f(theta, u)
	CppAD::ADFun<a1_double> joint_;
	// --------------------------------------------------------
	// private functions
	void record_joint(
		const d_vector& fixed_vec ,
		const d_vector& random_vec
	);
public:
	// constructor
	approx_mixed(size_t n_fixed, size_t n_random)
	:
	n_fixed_(n_fixed)   ,
	n_random_(n_random)
	{ }
	// density for data and random effects
	// (pure vritual function so must be defined by derived class)
	DISMOD_AT_DEFINE_JOINT_DENSITY( double )
	DISMOD_AT_DEFINE_JOINT_DENSITY( a1_double )
	DISMOD_AT_DEFINE_JOINT_DENSITY( a2_double )

	// optimize_random
	d_vector optimize_random(
		const d_vector& fixed_vec ,
		const d_vector& random_in
	);
};

} // END_DISMOD_AT_NAMESPACE

# endif
