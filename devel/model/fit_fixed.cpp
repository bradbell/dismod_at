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
public:
	// type declarations
	typedef CppAD::vector< CppAD::AD<double> > a1d_vector;
	typedef CppAD::vector<a5_double>           a5d_vector;
	//
private:
	// const member variables
	const pack_info&    pack_object_;
	const data_model&   data_object_;
	const prior_model&  prior_object_;
	//
	// temporaries in joint_density
	a5d_vector                                  a5_pack_vec_tmp_;
	CppAD::vector< residual_struct<a5_double> > data_like_tmp_;
	CppAD::vector< residual_struct<a5_double> > prior_ran_tmp_;
	a5d_vector                                  joint_den_tmp_;
	// temporaries in prior_density
	a1d_vector                                  random_vec_tmp_;
	a1d_vector                                  a1_pack_vec_tmp_;
	a1d_vector                                  prior_fix_tmp_;
	a1d_vector                                  prior_den_tmp_;
public:
	// constructor
	approx_derived(
		const pack_infi&   pack_object    ,
		const data_model&  data_object    ,
		const prior_model& prior_object ) :
	// base class constructor
	approx_mixed(
		size_fixed_effect(pack_object) , // n_fixed
		size_random_effect(pack_object)  // n_random
	) ,
	pack_object_( pack_object )     ,
	data_object_( data_object )     ,
	prior_object_( prior_object )
	{ }
	// pass joint density to base class
	virtual a5d_vector joint_density(
		const a5d_vector& fixed_vec   ,
		const a5d_vector& random_vec
	);
	// pass prior density to base class
	virtual a1d_vector prior_density(
		const a1d_vector& fixed_vec
	);
};
// ---------------------------------------------------------------------------
// joint_density
approx_derived::a5d_vector approx_derived::joint_density(
	const a5d_vector& fixed_vec   ,
	const a5d_vector& random_vec  )
{	// check if temporay needs to be sized
	if( a5_pack_vec_tmp_.size() == 0 )
		a5_pack_vec_tmp_.resize( pack_object_.size() );
	//
	// put the fixed and random effects into pack_vec
	put_fixed_effect(pack_object_, a5_pack_vec_tmp_, fixed_vec);
	put_random_effect(pack_object_, a5_pack_vec_tmp_, random_vec);
	//
	// evaluate the data and prior residuals
	data_like_tmp_  = data_object_.like_all(pack_vec);
	prior_ran_tmp_  = prior_object_.random(pack_vec);
	//
	// number of data and prior residuals
	size_t n_data_like  = data_like_tmp_.size();
	size_t n_prior_ran   = prior_ran_tmp_.size();
	//
	// check if this temporary needs to be sized
	if( prior_den_tmp_.size() == 0 )
	{	// count the number of absolute value terms
		size_t n_abs = 0;
		for(size_t i = 0; i < n_prior_ran; i++)
		{	density_enum density = prior_ran_tmp_[i].density;
			if( density == laplace_enum || density == log_laplace_enum )
				n_abs++;
		}
		// size joint_den_tmp_
		prior_den_tmp_.resize(1 + n_abs);
	}
	// initialize summation of smooth part
	prior_den_tmp_[0] = CppAD::AD(0.0);
	// initialize index for non-smooth part
	size_t iabs = 0;
	//
	// prior_ terms
	for(size_t i = 0; i < n_data_like; i++)
	{	prior_den_tmp[0] += prior_ran_tmp_[i].log_den_smooth;
		density_enum density = data_like_tmp_[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			prior_den_tmp_[1 + i_abs++] = prior_ran_tmp_[i].log_den_sub_abs;
	}
	// convert from log-density to negative log density
	joint_den_tmp[0] = - joint_den_tmp[0];
	//
	return joint_den_tmp_;
}
// ---------------------------------------------------------------------------
// prior_density
approx_derived::a1d_vector approx_derived::prior_density(
	const a1d_vector& fixed_vec   )
{	// check if this temporay needs to be sized
	if( random_vec_tmp_.size() == 0 )
		random_vec_tmp_.resize( size_random_effect(pack_object_) );
	if( prior_fix_tmp_.size() == 0 )
		prior_fix_tmp_.resize( size_fixed_effect(pack_object_) );
	if( a1_pack_vec_tmp_.size() == 0 )
		a1_pack_vec_tmp_.resize( pack_object_.size() );
	//
	// set random_vec_tmp_ to nan
	for(size_t i = 0; i < random_vec_tmp_.size(); i++)
		random_vec_tmp_[i] = CppAD::nan( CppAD::AD<double>(0.0) );
	//
	// put the fixed and random effects into pack_vec
	put_fixed_effect(pack_object_, a1_pack_vec_tmp_, fixed_vec);
	put_random_effect(pack_object_, a1_pack_vec_tmp_, random_vec_tmp_);
	//
	// evaluate the fixed effects prior
	prior_fix_tmp_  = prior_object_.fixed(pack_vec);
	//
	// number of data and prior residuals
	size_t n_prior_fix   = prior_fix_tmp_.size();
	//
	// check if this temporary needs to be sized
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
	// convert from log-density to negative log density
	joint_den_tmp[0] = - joint_den_tmp[0];
	//
	return prior_den_tmp_;
}

} // DISMOD_AT_END_NAMESPACE
