// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_VARIABLE_VEC_HPP
# define DISMOD_AT_VARIABLE_VEC_HPP

# include <cppad/vector.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template <class Float>
class variable_vec {
	struct subvector {
		// offset in vec_ where this subvector starts
		size_t offset_;
		// multipliers for smoothing standard deviations
		size_t multiply_value_;
		size_t multiply_dage_;
		size_t multiply_dtime_;
		// smoothing used for each subvector
		smooth_grid& sg_;
	};
private:
	// Informaiton for each subvector
	CppAD::vector<subvector> subvec_; 

	// value for all the variables
	CppAD::vector<Float> vec_;
public:
	variable_vec(void);
};

} // END DISMOD_AT_NAMESPACE:
# endif
