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
	//
	// temporaries in joint_density
	a5d_vector                                  pack_vec_tmp_;
	CppAD::vector< residual_struct<a5_double> > data_like_tmp_;
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
// ---------------------------------------------------------------------------
// joint_density
a5d_vector joint_density(
	const a5d_vector& fixed_vec   ,
	const a5d_vector& random_vec  )
{	// check if temporay needs to be sized
	if( pack-vec_tmp_.size() == 0 )
		pack_vec_tmp_.resize( pack_object_.size() );
	//
	// put the fixed and random effects into pack_vec
	put_fixed_effect(pack_object_, pack_vec_tmp_, fixed_vec);
	put_random_effect(pack_object_, pack_vec_tmp_, random_vec);
	//
	// evaluate the data and prior residuals
	data_like_tmp_  = data_object_.like_all(pack_vec);
	prior_ran_tmp_  = prior_object_.random(pack_vec);
	//
	// number of data and prior residuals
	size_t n_data_like  = data_like_tmp_.size();
	size_t n_prior_ran   = prior_ran_tmp_.size();
	//
	// check if temporary needs to be sized
	if( joint_den_tmp_.size() == 0 )
	{	// count the number of absolute value terms
		size_t n_abs = 0;
		for(size_t i = 0; i < n_data_like; i++)
		{	density_enum density = data_like_tmp_[i].density;
			if( density == laplace_enum || density == log_laplace_enum )
				n_abs++;
		}
		for(size_t i = 0; i < n_prior_ran; i++)
		{	density_enum density = prior_ran_tmp_[i].density;
			if( density == laplace_enum || density == log_laplace_enum )
				n_abs++;
		}
		// size joint_den_tmp_
		joint_den_tmp_.resize(1 + n_abs);
	}
	// initialize summation of smooth part
	joint_den_tmp_[0] = a5_double(0.0);
	// initialize index for non-smooth part
	size_t iabs = 0;
	//
	// data_like terms
	for(size_t i = 0; i < n_data_like; i++)
	{	joint_den_tmp[0] += data_like_tmp_[i].log_den_smooth;
		density_enum density = data_like_tmp_[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			joint_den_tmp_[1 + i_abs++] = data_like_tmp_[i].log_den_sub_abs;
	}
	//
	// random effects prior
	for(size_t i = 0; i < n_prior_ran; i++)
	{	joint_den_tmp[0] += prior_ran_tmp_[i].log_den_smooth;
		density_enum density = prior_ran_tmp_[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			joint_den_tmp_[1 + i_abs++] = prior_ran_tmp_[i].log_den_sub_abs;
	}
	//
	// convert from log-density to negative log dnesiyyt
	joint_den_tmp[0] = - joint_den_tmp[0];
	//
	return joint_den_tmp_;
}

} // DISMOD_AT_END_NAMESPACE
