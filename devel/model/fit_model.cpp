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
# include <dismod_at/fit_model.hpp>
# include <dismod_at/pack_prior.hpp>
/*
$begin fit_model$$
$spell
	vec
	const
	CppAD
	struct
$$

$section Fit the Fixed and Random Effects to the Model and Data$$

$head Syntax$$
$codei%fit_model %fit_object%(
	%pack_object%, %prior_table%, %s_info_vec%, %data_object%, %prior_object%
)
%$$
$codei%fit_object.run_fit()
%$$
$icode%optimal_fixed% = %fit_object%.get_optimal_fixed()
%$$
$icode%optimal_random% = %fit_object%.get_optimal_random()
%$$

$head fit_object$$
This object has prototype
$codei%
	fit_model %fit_object%
%$$

$head pack_object$$
This argument has prototype
$codei%
	const pack_info& %pack_object%
%$$
and is the $cref pack_info$$ information corresponding to the
$cref/model_variables/model_variable/$$.

$head prior_table$$
This argument has prototype
$codei%
	const CppAD::vector<prior_struct>&  %prior_table%
%$$
and is the $cref/prior_table/get_prior_table/prior_table/$$.

$head s_info_vec$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_info>& %s_info_vec%
%$$
For each $cref/smooth_id/smooth_table/smooth_id/$$,
$codei%
	%s_info_vec%[ %smooth_id% ]
%$$
is the corresponding $cref smooth_info$$ information.

$head data_object$$
This object has prototype
$codei%
	const data_model %data_object%
%$$
It contains the model for the data density;
see $cref/data_model/devel_data_model/$$.

$head prior_object$$
This object has prototype
$codei%
	const prior_model %prior_object%
%$$
It contains the model for the prior density; see $cref prior_model$$.

$head run_fit$$
Run the optimization process to determine the optimal fixed and random effects.

$head optimal_fixed$$
This return value has prototype
$codei%
	CppAD::vector<double> %optimal_fixed%
%$$
It is the optimal $cref/fixed effects/fixed_effect/$$ vector.

$head optimal_random$$
This return value has prototype
$codei%
	CppAD::vector<double> %optimal_random%
%$$
It is the optimal $cref/random effects/random_effect/$$ vector.

$children%example/devel/model/fit_model_xam.cpp
%$$
$head Example$$
The file $cref fit_model_xam.cpp$$ contains an example and test
of using this routine.

$end
*/

namespace dismod_at { // DISMOD_AT_BEGIN_NAMSPACE

// ===========================================================================
// public functions
// ===========================================================================
// constructor
fit_model::fit_model(
	const pack_info&                   pack_object  ,
	const CppAD::vector<prior_struct>& prior_table  ,
	const CppAD::vector<smooth_info>&  s_info_vec   ,
	const data_model&                  data_object  ,
	const prior_model&                 prior_object ) :
// base class constructor
approx_mixed(
	size_fixed_effect(pack_object) , // n_fixed
	size_random_effect(pack_object)  // n_random
) ,
n_fixed_       ( size_fixed_effect(pack_object)  )  ,
n_random_      ( size_random_effect(pack_object) )  ,
pack_object_   ( pack_object )                      ,
prior_table_   ( prior_table )                      ,
s_info_vec_    ( s_info_vec  )                      ,
data_object_   ( data_object )                      ,
prior_object_  ( prior_object )
{
	// value_prior_
	value_prior_ = pack_value_prior(pack_object, s_info_vec);
	size_t n_var = n_fixed_ + n_random_;
	assert( pack_object_.size() == n_var );
	assert( value_prior_.size() == n_var );
	// ---------------------------------------------------------------------
	// initialize the aprox_mixed object
	//
	CppAD::vector<double> pack_vec( n_var );
	// fixed_vec
	CppAD::vector<double> fixed_vec(n_fixed_);
	for(size_t i = 0; i < n_var; i++)
		pack_vec[i] = prior_table_[ value_prior_[i] ].mean;
	get_fixed_effect(pack_object_, pack_vec, fixed_vec);
	// random_vec
	CppAD::vector<double> random_vec(n_random_);
	for(size_t i = 0; i < n_var; i++)
		pack_vec[i] = prior_table_[ value_prior_[i] ].mean;
	get_random_effect(pack_object_, pack_vec, random_vec);
	//
	initialize(fixed_vec, random_vec);
}
// ---------------------------------------------------------------------------
// run_fit
void fit_model::run_fit(void)
{	size_t n_var = n_fixed_ + n_random_;
	assert( pack_object_.size() == n_var );
	assert( value_prior_.size() == n_var );
	CppAD::vector<double> pack_vec( n_var );

	// fixed_lower
	CppAD::vector<double> fixed_lower(n_fixed_);
	for(size_t i = 0; i < n_var; i++)
		pack_vec[i] = prior_table_[ value_prior_[i] ].lower;
	get_fixed_effect(pack_object_, pack_vec, fixed_lower);

	// fixed_in
	CppAD::vector<double> fixed_in(n_fixed_);
	for(size_t i = 0; i < n_var; i++)
		pack_vec[i] = prior_table_[ value_prior_[i] ].mean;
	get_fixed_effect(pack_object_, pack_vec, fixed_in);

	// fixed_upper
	CppAD::vector<double> fixed_upper(n_fixed_);
	for(size_t i = 0; i < n_var; i++)
		pack_vec[i] = prior_table_[ value_prior_[i] ].upper;
	get_fixed_effect(pack_object_, pack_vec, fixed_upper);

	// random_in
	CppAD::vector<double> random_in(n_random_);
	for(size_t i = 0; i < n_var; i++)
		pack_vec[i] = prior_table_[ value_prior_[i] ].mean;
	get_random_effect(pack_object_, pack_vec, random_in);

	// store result
	optimal_fixed_ = optimize_fixed(
		fixed_lower, fixed_in, fixed_upper, random_in
	);
	optimal_random_ = optimize_random(optimal_fixed_, random_in);
}
// ---------------------------------------------------------------------------
// get_optimal_fixed
CppAD::vector<double> fit_model::get_optimal_fixed(void)
{	return optimal_fixed_; }
// ---------------------------------------------------------------------------
// get_optimal_random
CppAD::vector<double> fit_model::get_optimal_random(void)
{	return optimal_random_; }
// ===========================================================================
// private functions
// ===========================================================================
// joint_density
fit_model::a5d_vector fit_model::joint_density(
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
fit_model::a1d_vector fit_model::prior_density(
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
