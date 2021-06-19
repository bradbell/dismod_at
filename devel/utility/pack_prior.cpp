// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/pack_prior.hpp>
# include <dismod_at/null_int.hpp>

/*
-------------------------------------------------------------------------------
$begin pack_prior$$
$spell
	var
	vec
	struct
	const
	dage
	dtime
	bool
	bnd_mulcov
	covariate
$$

$section Priors in Variable ID Order$$

$head Syntax$$
$codei%pack_prior %var2prior%(%pack_object%, %s_info_vec%)
%$$
$icode%size%           = %var2prior%.size()
%$$
$icode%const_value%    = %var2prior%.const_value(%var_id%)
%$$
$icode%value_prior_id% = %var2prior%.value_prior_id(%var_id%)
%$$
$icode%dage_prior_id%  = %var2prior%.dage_prior_id(%var_id%)
%$$
$icode%dtime_prior_id% = %var2prior%.dtime_prior_id(%var_id%)
%$$
$icode%dage_var_id%    = %var2prior%.dage_next(%var_id%)
%$$
$icode%dtime_var_id%   = %var2prior%.dtime_next(%var_id%)
%$$
$icode%fixed_effect%   = %var2prior%.fixed_effect(%var_id%)
%$$
$icode%var2prior%.bnd_mulcov(%bnd_mulcov_table%)
%$$
$icode%min_lower%      = %var2prior%.min_lower(%var_id%)
%$$
$icode%max_upper%      = %var2prior%.max_upper(%var_id%)
%$$

$head Prototype$$
$srcthisfile%0%// BEGIN CTOR_PROTOTYPE%// END CTOR_PROTOTYPE%1%
%$$

$head Notation$$

$subhead var_id$$
The variable names that end in
$cref/var_id/var_table/var_id/$$
have type $code size_t$$ and are an index in the variable table.

$subhead prior_id$$
The variable names that end in
$cref/prior_id/prior_table/prior_id/$$
have type $code size_t$$ and are an index in the prior table.

$head pack_object$$
is the $cref pack_info$$ information corresponding to
the $cref model_variables$$.

$head s_info_vec$$
For each $cref/smooth_id/smooth_table/smooth_id/$$,
$codei%
	%s_info_vec%[ %smooth_id% ]
%$$
is the corresponding $cref smooth_info$$ information.

$head var2prior$$
an object that reports that prior information as a function of
$cref/var_id/var_table/var_id/$$.

$head size$$
This is the number of variables in the model; i.e.,
it is equal to $icode%pack_object%.size()%$$.

$head const_value$$
For each $icode var_id$$,
if $icode const_value$$ is not nan (not null)
$icode const_value$$ is the value at this variable is constrained to.

$head smooth_id$$
This return value has type $code size_t$$.
If $icode smooth_id$$ is null, this variables is a
$cref/smoothing standard deviation multiplier
	/model_variables
	/Fixed Effects, theta
	/Smoothing Standard Deviation Multipliers, lambda
/$$.
In this case there is no standard deviation multiplier for the
value prior for this variables and the difference priors are null.
Otherwise, this is the smoothing used for this variable and the
offset for the corresponding standard deviation multipliers can be computed
using $cref pack_info_mulstd$$.

$head value_prior_id$$
If $icode const_value$$ is null,
$icode value_prior_id$$ must be null and identifies the prior for
the value of this variable.
The null prior for this case corresponds to a uniform
between minus and plus infinity.

$head dage_prior_id$$
If $icode dage_var_id$$ is not null,
$icode dage_prior_id$$ identifies the prior for
the difference between the variable with index
$icode var_id$$ and the variable with index $icode dage_var_id$$.
The null prior for this case corresponds to a uniform
between minus and plus infinity.

$head dtime_prior_id$$
If $icode dtime_var_id$$ is not null,
$icode dtime_prior_id$$ identifies the prior for
the difference between the variable with index
$icode var_id$$ and the variable with index $icode dtime_var_id$$.
The null prior for this case corresponds to a uniform
between minus and plus infinity.

$head dage_var_id$$
If $icode dage_prior_id$$ is not null, this is the
next variable in the age difference.
If $icode var_id$$ corresponds to the maximum age in the grid,
$icode dage_var_id$$ is null.

$head dtime_var_id$$
If $icode dtime_prior_id$$ is not null, this is the
next variable in the time difference.
If $icode var_id$$ corresponds to the maximum time in the grid,
$icode dtime_var_id$$ is null.

$head fixed_effect$$
This $code bool$$ is true (false) if the variable with index
$icode var_id$$ is a
$cref/fixed effect/model_variables/Fixed Effects, theta/$$
(random effect).

$head bnd_mulcov$$
This member function sets the maximum upper and minimum lower
limit for the covariate multipliers.
The initial $icode var2prior$$ corresponds the maximum being
plus infinity and the minimum being minus infinity.

$head bnd_mulcov_table$$
See$cref/bnd_mulcov_table/get_bnd_mulcov_table/bnd_mulcov_table/$$.

$head min_lower$$
Is the minimum lower limit for this variable corresponding to the
previous call to $code bnd_mulcov$$.
This is minus infinity before $code bnd_mulcov$$ is called.

$head max_upper$$
Is the maximum upper limit for this variable corresponding to the
previous call to $code bnd_mulcov$$.
This is plus infinity before $code bnd_mulcov$$ is called.

$children%
	example/devel/utility/pack_prior_xam.cpp
%$$
$head Example$$
The file $cref pack_prior_xam.cpp$$
contains an example and test that uses this routine.

$end
-------------------------------------------------------------------------------
*/
namespace dismod_at {

// size
size_t pack_prior::size(void) const
{	return prior_vec_.size(); }

// const_value
double pack_prior::const_value(size_t var_id) const
{	return prior_vec_[var_id].const_value; }

// smooth_id
size_t pack_prior::smooth_id(size_t var_id) const
{	return prior_vec_[var_id].smooth_id; }

// value_prior_id
size_t pack_prior::value_prior_id(size_t var_id) const
{	return prior_vec_[var_id].value_prior_id; }

// dage_prior_id
size_t pack_prior::dage_prior_id(size_t var_id) const
{	return prior_vec_[var_id].dage_prior_id; }

// dtime_prior_id
size_t pack_prior::dtime_prior_id(size_t var_id) const
{	return prior_vec_[var_id].dtime_prior_id; }

// dage_var_id
size_t pack_prior::dage_var_id(size_t var_id) const
{	return var_id + prior_vec_[var_id].n_time; }

// dtime_var_id
size_t pack_prior::dtime_var_id(size_t var_id) const
{	return var_id + 1; }

// fixed_effect
bool pack_prior::fixed_effect(size_t  var_id) const
{	return prior_vec_[var_id].fixed_effect; }

// set_prior_vec
// sets all fields except for min_lower and max_upper
void pack_prior::set_prior_vec(
	size_t                                                    offset       ,
	bool                                                      fixed_effect ,
	size_t                                                    mulcov_id    ,
	size_t                                                    smooth_id    ,
	const CppAD::vector<smooth_info>&                         s_info_vec   )
{	//
	smooth_info s_info  = s_info_vec[smooth_id];
	size_t n_age        = s_info.age_size();
	size_t n_time       = s_info.time_size();
	//
	// loop over grid points for this smoothing in age, time order
	for(size_t i = 0; i < n_age; i++)
	{	for(size_t j = 0; j < n_time; j++)
		{	// var_id
			size_t var_id   = offset + i * n_time + j;
			//
			prior_vec_[var_id].fixed_effect = fixed_effect;
			prior_vec_[var_id].mulcov_id    = mulcov_id;
			//
			// const_value
			double const_value            = s_info.const_value(i, j);
			prior_vec_[var_id].const_value = const_value;
			//
			// smooth_id
			prior_vec_[var_id].smooth_id = smooth_id;
			//
			// n_time
			prior_vec_[var_id].n_time = n_time;
			//
			// value prior
			size_t value_prior_id            = s_info.value_prior_id(i, j);
			prior_vec_[var_id].value_prior_id = value_prior_id;
			//
			// dage prior
			prior_vec_[var_id].dage_prior_id = s_info.dage_prior_id(i, j);
			CPPAD_ASSERT_UNKNOWN( i + 1 < n_age ||
				prior_vec_[var_id].dage_prior_id == DISMOD_AT_NULL_SIZE_T
			);
			//
			// dtime_prior
			prior_vec_[var_id].dtime_prior_id = s_info.dtime_prior_id(i, j);
			CPPAD_ASSERT_UNKNOWN( j + 1 < n_time ||
				prior_vec_[var_id].dtime_prior_id == DISMOD_AT_NULL_SIZE_T
			);
			//
# ifndef NDEBUG
			bool value_prior_null = value_prior_id == DISMOD_AT_NULL_SIZE_T;
			bool const_value_null = std::isnan( const_value );
			assert( ! (value_prior_null && const_value_null) );
			assert( ! value_prior_null || ! const_value_null );

# endif
		}
	}
	return;
}

// BEGIN CTOR_PROTOTYPE
pack_prior::pack_prior(
	const pack_info&                   pack_object ,
	const CppAD::vector<smooth_info>&  s_info_vec  )
// END CTOR_PROTOTYPE
{
	//
	pack_info::subvec_info info;
	double nan   = std::numeric_limits<double>::quiet_NaN();
	double inf   = std::numeric_limits<double>::infinity();
	//
	size_t n_var       = pack_object.size();
	size_t n_child     = pack_object.child_size();
	size_t n_integrand = pack_object.integrand_size();
	size_t n_smooth    = s_info_vec.size();
	//
	// -----------------------------------------------------------------------
	// initialize everyting to not defined except min_lower, max_upper
	prior_vec_.resize(n_var);
	for(size_t var_id = 0; var_id < n_var; ++var_id)
	{
		prior_vec_[var_id].min_lower      = - inf;
		prior_vec_[var_id].max_upper      = + inf;
		//
		prior_vec_[var_id].const_value    = nan;
		prior_vec_[var_id].n_time         = DISMOD_AT_NULL_SIZE_T;
		prior_vec_[var_id].smooth_id      = DISMOD_AT_NULL_SIZE_T;
		prior_vec_[var_id].mulcov_id      = DISMOD_AT_NULL_SIZE_T;
		prior_vec_[var_id].value_prior_id = DISMOD_AT_NULL_SIZE_T;
		prior_vec_[var_id].dage_prior_id  = DISMOD_AT_NULL_SIZE_T;
		prior_vec_[var_id].dtime_prior_id = DISMOD_AT_NULL_SIZE_T;
		// alternate value (should not matter)
		prior_vec_[var_id].fixed_effect   = bool( var_id % 2 );
	}
	//
	// get priors for smoothing standard deviation multipliers
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	// multipliers for this smoothing
		for(size_t k = 0; k < 3; k++)
		{	// value, dage, dtime in that order
			size_t offset     = pack_object.mulstd_offset(smooth_id, k);
			if( offset != DISMOD_AT_NULL_SIZE_T )
			{	size_t prior_id;
				switch(k)
				{	case 0:
					prior_id = s_info_vec[smooth_id].mulstd_value();
					break;

					case 1:
					prior_id = s_info_vec[smooth_id].mulstd_dage();
					break;

					case 2:
					prior_id = s_info_vec[smooth_id].mulstd_dtime();
					break;

					default:
					assert(false);
				}
				// this prior is for a constant; i.e., n_age = n_time = 1
				prior_vec_[offset].n_time         = 1;
				prior_vec_[offset].fixed_effect   = true;
				prior_vec_[offset].value_prior_id = prior_id;
			}
		}
	}
	// ------------------------------------------------------------------------
	// get priors for rate values
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	// this rate
		for(size_t j = 0; j <= n_child; j++)
		{	// child if j < n_child, otherwise parent
			info             = pack_object.node_rate_value_info(rate_id, j);
			size_t smooth_id = info.smooth_id;
			// if smooth_id is null this has no variables
			if( smooth_id != DISMOD_AT_NULL_SIZE_T )
			{	size_t offset       = info.offset;
				bool   fixed_effect = j == n_child;
				size_t mulcov_id    = info.mulcov_id;
				set_prior_vec(
					offset, fixed_effect, mulcov_id, smooth_id, s_info_vec
				);
			}
		}
	}
	// ------------------------------------------------------------------------
	// get priors for subgroup rate value covariates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t n_cov = pack_object.subgroup_rate_value_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	size_t n_sub = pack_object.subgroup_rate_value_n_sub(rate_id, j);
			for(size_t k = 0; k < n_sub; ++k)
			{	info   = pack_object.subgroup_rate_value_info(rate_id, j, k);
				size_t offset       = info.offset;
				size_t smooth_id    = info.smooth_id;
				bool   fixed_effect = false;
				size_t mulcov_id    = info.mulcov_id;
				set_prior_vec(
					offset, fixed_effect, mulcov_id, smooth_id, s_info_vec
				);
			}
		}
	}
	// ------------------------------------------------------------------------
	// get priors for group rate value covariates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t n_cov = pack_object.group_rate_value_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.group_rate_value_info(rate_id, j);
			size_t offset       = info.offset;
			size_t smooth_id    = info.smooth_id;
			bool   fixed_effect = true;
			size_t mulcov_id    = info.mulcov_id;
			set_prior_vec(
				offset, fixed_effect, mulcov_id, smooth_id, s_info_vec
			);
		}
	}
	// ------------------------------------------------------------------------
	// get priors for subgroup measurement value covariates
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	// measurement value covariates for this integrand
		size_t n_cov = pack_object.subgroup_meas_value_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	size_t n_sub =
				pack_object.subgroup_meas_value_n_sub(integrand_id, j);
			for(size_t k = 0; k < n_sub; ++k)
			{	info =
					pack_object.subgroup_meas_value_info(integrand_id, j, k);
				size_t offset       = info.offset;
				size_t smooth_id    = info.smooth_id;
				bool   fixed_effect = false;
				size_t mulcov_id    = info.mulcov_id;
				set_prior_vec(
					offset, fixed_effect, mulcov_id, smooth_id, s_info_vec
				);
			}
		}
	}
	// ------------------------------------------------------------------------
	// get priors for group measurement covariates
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	// measurement value covariates for this integrand
		size_t n_cov = pack_object.group_meas_value_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.group_meas_value_info(integrand_id, j);
			size_t offset       = info.offset;
			size_t smooth_id    = info.smooth_id;
			bool   fixed_effect = true;
			size_t mulcov_id    = info.mulcov_id;
			set_prior_vec(
				offset, fixed_effect, mulcov_id, smooth_id, s_info_vec
			);
		}
		// measurement std covariates for this integrand
		n_cov = pack_object.group_meas_noise_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.group_meas_noise_info(integrand_id, j);
			size_t offset       = info.offset;
			size_t smooth_id    = info.smooth_id;
			bool   fixed_effect = true;
			size_t mulcov_id    = info.mulcov_id;
			set_prior_vec(
				offset, fixed_effect, mulcov_id, smooth_id, s_info_vec
			);
		}
	}
	return;
}

} // END_DISMOD_AT_NAMESPACE
