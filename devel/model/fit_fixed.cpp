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
# include <dismod_at/fit_fixed.hpp>
# include <dismod_at/pack_prior.hpp>

namespace dismod_at { // DISMOD_AT_BEGIN_NAMSPACE

// ---------------------------------------------------------------------------
// constructor
fit_fixed::fit_fixed(
	const CppAD::vector<prior_struct>& prior_table  ,
	const CppAD::vector<smooth_info>&  s_info_vec   ,
	const pack_info&                   pack_object  ,
	const data_model&                  data_object  ,
	const prior_model&                 prior_object ) :
// base class constructor
approx_mixed(
	size_fixed_effect(pack_object) , // n_fixed
	size_random_effect(pack_object)  // n_random
) ,
prior_table_   ( prior_table  )   ,
s_info_vec_    ( s_info_vec   )   ,
pack_object_   ( pack_object  )   ,
data_object_   ( data_object )    ,
prior_object_  ( prior_object )
{	value_prior_ = pack_value_prior(pack_object, s_info_vec);
}
// ---------------------------------------------------------------------------
// joint_density
fit_fixed::a5d_vector fit_fixed::joint_density(
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
	data_like_tmp_  = data_object_.like_all(a5_pack_vec_tmp_);
	prior_ran_tmp_  = prior_object_.random(a5_pack_vec_tmp_);
	//
	// number of data and prior residuals
	size_t n_data_like  = data_like_tmp_.size();
	size_t n_prior_ran   = prior_ran_tmp_.size();
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
	//
	// initialize index for non-smooth part
	size_t i_abs = 0;
	//
	// data_like terms
	for(size_t i = 0; i < n_data_like; i++)
	{	joint_den_tmp_[0] += data_like_tmp_[i].logden_smooth;
		density_enum density = data_like_tmp_[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			joint_den_tmp_[1 + i_abs++] = data_like_tmp_[i].logden_sub_abs;
	}
	//
	// random effects prior
	for(size_t i = 0; i < n_prior_ran; i++)
	{	joint_den_tmp_[0] += prior_ran_tmp_[i].logden_smooth;
		density_enum density = prior_ran_tmp_[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			joint_den_tmp_[1 + i_abs++] = prior_ran_tmp_[i].logden_sub_abs;
	}
	// convert from log-density to negative log density
	joint_den_tmp_[0] = - joint_den_tmp_[0];
	//
	return joint_den_tmp_;
}
// ---------------------------------------------------------------------------
// prior_density
fit_fixed::a1d_vector fit_fixed::prior_density(
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
		random_vec_tmp_[i] = CppAD::nan( a1_double(0.0) );
	//
	// put the fixed and random effects into pack_vec
	put_fixed_effect(pack_object_, a1_pack_vec_tmp_, fixed_vec);
	put_random_effect(pack_object_, a1_pack_vec_tmp_, random_vec_tmp_);
	//
	// evaluate the fixed effects prior
	prior_fix_tmp_  = prior_object_.fixed(a1_pack_vec_tmp_);
	//
	// number of data and prior residuals
	size_t n_prior_fix   = prior_fix_tmp_.size();
	//
	// check if this temporary needs to be sized
	if( prior_den_tmp_.size() == 0 )
	{	// count the number of absolute value terms
		size_t n_abs = 0;
		for(size_t i = 0; i < n_prior_fix; i++)
		{	density_enum density = prior_fix_tmp_[i].density;
			if( density == laplace_enum || density == log_laplace_enum )
				n_abs++;
		}
		// size joint_den_tmp_
		prior_den_tmp_.resize(1 + n_abs);
	}
	// initialize summation of smooth part
	prior_den_tmp_[0] = a1_double(0.0);
	//
	// initialize index for non-smooth part
	size_t i_abs = 0;
	//
	// prior_ terms
	for(size_t i = 0; i < n_prior_fix; i++)
	{	prior_den_tmp_[0] += prior_fix_tmp_[i].logden_smooth;
		density_enum density = prior_fix_tmp_[i].density;
		if( density == laplace_enum || density == log_laplace_enum )
			prior_den_tmp_[1 + i_abs++] = prior_fix_tmp_[i].logden_sub_abs;
	}
	//
	// convert from log-density to negative log density
	prior_den_tmp_[0] = - prior_den_tmp_[0];
	//
	return prior_den_tmp_;
}

} // DISMOD_AT_END_NAMESPACE
