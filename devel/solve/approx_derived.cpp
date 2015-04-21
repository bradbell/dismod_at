// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/a5_double.hpp>

namespace dismod_at { // DISMOD_AT_BEGIN_NAMSPACE

// declare approx_derived class
class approx_derived : public approx_mixed {
private:
	// type declarations
	typedef CppAD::vector<a5_double> a5d_vector;
	// const member variables
	const data_object_;

	// temporaries
	CppAD::vector< residual_struct<a5_double> > like_all_tmp_;
	CppAD::vector< residual_struct<a5_double> > prior_ran_tmp_;
	a5d_vector                                  joint_den_tmp_;
public:
	// constructor
	approx_derived(
		size_t n_fixed               ,
		size_t n_random              ,
		const data_model data_object ) :
	approx_mixed(n_fixed, n_random) ,
	data_object_( data_object )     ,
	{ }
	// pass joint density to base class
	virtual a5d_vector joint_density(
		const a5d_vector& fixed_vec   ,
		const a5d_vector& random_vec
	);
	// pass prior density to base class
	virtual a5d_vector prior_density(
		const a5d_vector& fixed_vec
	);
};

// joint_density
a5d_vector joint_density(
	const a5d_vector& fixed_vec   ,
	const a5d_vector& random_vec  )
{
	like_all_tmp_  = data_object_.like_all(pack_vec);
	prior_ran_tmp_ = prior_object_.random(pack_vec);
	for(size_t i = 0;

}

} // DISMOD_AT_END_NAMESPACE
