// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
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
$$

$section Priors in Variable ID Order$$

$head Syntax$$
$codei%pack_prior% %var2prior%(%pack_object%, %s_info_vec%)
%$$
$icode%size%           = %var2prior%.size()
%$$
$icode%const_value%    = %var2prior%.const_value(%var_id%)
%$$
$icode%value_prior_id% = %var2prior%.%value_prior_id(%var_id%)
%$$
$icode%dage_prior_id%  = %var2prior%.%dage_prior_id(%var_id%)
%$$
$icode%dtime_prior_id% = %var2prior%.%dtime_prior_id(%var_id%)
%$$
$icode%dage_var_id%    = %var2prior%.%dage_next(%var_id%)
%$$
$icode%dtime_var_id%   = %var2prior%.%dtime_next(%var_id%)
%$$
$icode%fixed_effect%   = %var2prior%.%fixed_effect(%var_id%)
%$$

$head Prototype$$
$srcfile%devel/utility/pack_prior.cpp
	%0%// BEGIN CTOR_PROTOTYPE%// END CTOR_PROTOTYPE%1%
%$$

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

$head var_id$$
The variable names that end in
$cref/var_id/var_table/var_id/$$
have type $code size_t$$ and are an index in the variable table.

$head prior_id$$
The variable names that end in
$cref/prior_id/prior_table/prior_id/$$
have type $code size_t$$ and are an index in the prior table.

$head const_value$$
For each $icode var_id$$,
either $icode const_value$$ this is nan or
$icode value_prior_id$$ is null but not both.
If $icode value_prior_id$$ is null,
$icode const_value$$ is the value at this variable is constrained to.

$head value_prior_id$$
Either this is $code null$$ or $icode const_value$$ is nan but not both.
If $icode const_value$$ is nan,
$icode value_prior_id$$ identifies the prior for
the value of this variable.

$head dage_prior_id$$
If $icode dage_prior_id$$ is null,
this variable corresponding to the maximum age in a smoothing grid.
Otherwise $icode dage_prior_id$$ identifies the  prior for
the difference between the variable with index
$icode var_id$$ and the variable with index $icode dage_var_id$$.

$head dtime_prior_id$$
If $icode dtime_prior_id$$ is null,
this variable corresponding to the maximum time in a smoothing grid.
Otherwise $icode dtime_prior_id$$ identifies the prior for
the difference between the variable with index
$icode var_id$$ and the variable with index $icode dtime_var_id$$.

$head dage_var_id$$
If $icode dage_prior_id$$ is not null, this is the
next variable in the age difference.

$head dtime_var_id$$
If $icode dtime_prior_id$$ is not null, this is the
next variable in the time difference.

$head fixed_effect$$
This $code bool$$ is true (false) if the variable with index
$icode var_id$$ is a
$cref/fixed effect/model_variables/Fixed Effects, theta/$$
(random effect).

$children%
	example/devel/utility/pack_var_prior_xam.cpp
%$$
$head Example$$
The file $cref pack_var_prior_xam.cpp$$
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

// set_prior
void pack_prior::set_prior(
	CppAD::vector<dismod_at::pack_prior::one_prior_struct>&   prior_vec  ,
	size_t                                                    offset     ,
	const dismod_at::smooth_info&                             s_info     )
{	//
	size_t n_age     = s_info.age_size();
	size_t n_time    = s_info.time_size();
	//
	// loop over grid points for this smoothing in age, time order
	for(size_t i = 0; i < n_age; i++)
	{	for(size_t j = 0; j < n_time; j++)
		{	// var_id
			size_t var_id   = offset + i * n_time + j;
			//
			// n_time
			prior_vec[var_id].n_time = n_time;
			//
			// value prior
			prior_vec[var_id].value_prior_id = s_info.value_prior_id(i, j);
			prior_vec[var_id].const_value    = s_info.const_value(i, j);
			//
			// dage prior
			prior_vec[var_id].dage_prior_id = s_info.dage_prior_id(i, j);
			CPPAD_ASSERT_UNKNOWN( i + 1 < n_age ||
				prior_vec[var_id].dage_prior_id == DISMOD_AT_NULL_SIZE_T
			);
			//
			// dtime_prior
			prior_vec[var_id].dtime_prior_id = s_info.dtime_prior_id(i, j);
			CPPAD_ASSERT_UNKNOWN( j + 1 < n_time ||
				prior_vec[var_id].dtime_prior_id == DISMOD_AT_NULL_SIZE_T
			);
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
	//
	size_t n_var       = pack_object.size();
	size_t n_child     = pack_object.child_size();
	size_t n_integrand = pack_object.integrand_size();
	size_t n_smooth    = s_info_vec.size();
	//
	// -----------------------------------------------------------------------
	// initialize everyting as nan or null
	// except fixed effect which is initialized as true.
	prior_vec_.resize(n_var);
	for(size_t var_id = 0; var_id < n_var; ++var_id)
	{	prior_vec_[var_id].n_time         = DISMOD_AT_NULL_SIZE_T;
		prior_vec_[var_id].const_value    = nan;
		prior_vec_[var_id].value_prior_id = DISMOD_AT_NULL_SIZE_T;
		prior_vec_[var_id].dage_prior_id  = DISMOD_AT_NULL_SIZE_T;
		prior_vec_[var_id].dtime_prior_id = DISMOD_AT_NULL_SIZE_T;
		//
		// default value for fixed_effect is true
		prior_vec_[var_id].fixed_effect   = true;
	}
	//
	// get priors for standard devaition multipliers
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
				prior_vec_[offset].value_prior_id = prior_id;
			}
		}
	}
	// ------------------------------------------------------------------------
	// get priors for rates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	// this rate
		for(size_t j = 0; j <= n_child; j++)
		{	// child if j < n_child, otherwise parent
			info             = pack_object.rate_info(rate_id, j);
			size_t smooth_id = info.smooth_id;
			if( smooth_id != DISMOD_AT_NULL_SIZE_T )
			{	size_t offset    = info.offset;
				set_prior(prior_vec_, offset, s_info_vec[smooth_id]);
				//
				// check for random effects variables
				if( j < n_child )
				{	for(size_t i = 0; i < info.n_var; i++)
						prior_vec_[offset + i].fixed_effect = false;
				}
			}
		}
	}
	// ------------------------------------------------------------------------
	// get priors for rate mean covariates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t n_cov = pack_object.mulcov_rate_value_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.mulcov_rate_value_info(rate_id, j);
			size_t offset    = info.offset;
			size_t smooth_id = info.smooth_id;
			set_prior(prior_vec_, offset, s_info_vec[smooth_id]);
		}
	}
	// ------------------------------------------------------------------------
	// get priors for measurement covariates
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	// measurement mean covariates for this integrand
		size_t n_cov = pack_object.mulcov_meas_value_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.mulcov_meas_value_info(integrand_id, j);
			size_t offset    = info.offset;
			size_t smooth_id = info.smooth_id;
			set_prior(prior_vec_, offset, s_info_vec[smooth_id]);
		}
		// measurement std covariates for this integrand
		n_cov = pack_object.mulcov_meas_std_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.mulcov_meas_std_info(integrand_id, j);
			size_t offset    = info.offset;
			size_t smooth_id = info.smooth_id;
			set_prior(prior_vec_, offset, s_info_vec[smooth_id]);
		}
	}
	return;
}

} // END_DISMOD_AT_NAMESPACE
