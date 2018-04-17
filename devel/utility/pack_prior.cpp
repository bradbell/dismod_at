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

namespace {
	// ----------------------------------------------------------------------
	void set_value_prior(
		CppAD::vector<size_t>&            value_prior_id_out ,
		CppAD::vector<double>&            const_value_out    ,
		size_t                            offset             ,
		const dismod_at::smooth_info&     s_info             )
	{
		size_t n_age     = s_info.age_size();
		size_t n_time    = s_info.time_size();
		for(size_t i = 0; i < n_age; i++)
		{	for(size_t j = 0; j < n_time; j++)
			{	size_t index   = offset + i * n_time + j;
				value_prior_id_out[index] = s_info.value_prior_id(i, j);
				const_value_out[index]    = s_info.const_value(i, j);
			}
		}
	}
	// ----------------------------------------------------------------------
	using dismod_at::diff_prior_struct;
	void set_diff_prior(
		CppAD::vector<diff_prior_struct>&  ret_val ,
		size_t                             offset  ,
		const dismod_at::smooth_info&      s_info  )
	{	diff_prior_struct one_prior;
		size_t n_age     = s_info.age_size();
		size_t n_time    = s_info.time_size();
		//
		// dage_prior_id
		if( n_age > 1 )
		{	for(size_t i = 0; i < n_age-1; i++)
			{	for(size_t j = 0; j < n_time; j++)
				{	one_prior.direction    = diff_prior_struct::dage_enum;
					one_prior.minus_var_id = offset + i * n_time + j;
					one_prior.plus_var_id  = offset + (i+1) * n_time + j;
					one_prior.prior_id     = s_info.dage_prior_id(i, j);
					assert( one_prior.prior_id != DISMOD_AT_NULL_SIZE_T );
					ret_val.push_back(one_prior);
				}
			}
		}
		//
		// dtime_prior_id
		if( n_time > 1 )
		{	for(size_t i = 0; i < n_age; i++)
			{	for(size_t j = 0; j < n_time-1; j++)
				{	one_prior.direction    = diff_prior_struct::dtime_enum;
					one_prior.minus_var_id = offset + i * n_time + j;
					one_prior.plus_var_id  = offset + i * n_time + j+1;
					one_prior.prior_id     = s_info.dtime_prior_id(i, j);
					assert( one_prior.prior_id != DISMOD_AT_NULL_SIZE_T );
					ret_val.push_back(one_prior);
				}
			}
		}
	}
}

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
------------------------------------------------------------------------------
$begin pack_value_prior$$
$spell
	vec
	const
	CppAD
$$

$section Prior Ids In Same Order As Packed Variables$$

$head Syntax$$
$codei%pack_value_prior(
	%value_prior_id_out%, %const_value_out%, %pack_object%, %s_info_vec%
)%$$

$head pack_object$$
This argument has prototype
$codei%
	const pack_info& %pack_object%
%$$
and is the $cref pack_info$$ information corresponding to
the $cref model_variables$$.

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

$head value_prior_id_out$$
This argument has prototype
$codei%
	CppAD::vector<size_t>& %value_prior_id_out%
%$$
and on input it's size is zero.
Upon return, its size is $icode%pack_object%.size()%$$ and
the element $icode%value_prior_id%[%i%]%$$ is the
$cref/prior_id/prior_table/prior_id/$$ corresponding to the
$th i$$ model variable in the order determined by $icode pack_object$$.
If
$codei%
	%value_prior_id_out%[%i%] == DISMOD_AT_NULL_SIZE_T
%$$
the $th i$$ model variable is constrained to have value
$icode%const_value_out%[%i%]%$$.

$head const_value_out$$
This argument has prototype
$codei%
	CppAD::vector<double>& %const_value_out%
%$$
and on input it's size is zero.
Upon return, its size is $icode%pack_object%.size()%$$ and
the element $icode%const_value_out%[%i%]%$$ is
the value that the $th i$$ model variable is constrained to.
If
$codei%
	%const_value_out%[%i%] == nan
%$$
$icode%value_prior_id%[%i%]%$$ is not $code DISMOD_AT_NULL_SIZE_T$$
and it specifies the prior for the value of $th i$$ model variable.

$children%
	example/devel/utility/pack_value_prior_xam.cpp
%$$
$head Example$$
The file $cref pack_value_prior_xam.cpp$$
contains an example and test that uses this routine.

$end
*/

void pack_value_prior(
	CppAD::vector<size_t>&               value_prior_id_out ,
	CppAD::vector<double>&               const_value_out    ,
	const pack_info&                     pack_object        ,
	const CppAD::vector<smooth_info>&    s_info_vec         )
{	pack_info::subvec_info info;
	assert( value_prior_id_out.size() == 0 );
	assert( const_value_out.size() == 0 );
	//
	double nan = std::numeric_limits<double>::quiet_NaN();
	//
	// size the output values
	value_prior_id_out.resize( pack_object.size() );
	const_value_out.resize( pack_object.size() );
	//
	// some parameters
	size_t n_smooth    = s_info_vec.size();
	size_t n_child     = pack_object.child_size();
	size_t n_integrand = pack_object.integrand_size();

	// get priors for standard devaition multipliers
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	for(size_t k = 0; k < 3; k++)
		{	// mulstd
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
				value_prior_id_out[offset] = prior_id;
				const_value_out[offset]    = nan;
			}
		}
	}
	//
	// get priors for rates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	for(size_t j = 0; j <= n_child; j++)
		{	info             = pack_object.rate_info(rate_id, j);
			size_t smooth_id = info.smooth_id;
			if( smooth_id != DISMOD_AT_NULL_SIZE_T )
			{	size_t offset    = info.offset;
				set_value_prior(
					value_prior_id_out,
					const_value_out,
					offset,
					s_info_vec[smooth_id]
				);
			}
		}
	}

	// get priors for rate mean covariates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t n_cov = pack_object.mulcov_rate_value_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.mulcov_rate_value_info(rate_id, j);
			size_t smooth_id = info.smooth_id;
			if( smooth_id != DISMOD_AT_NULL_SIZE_T )
			{	size_t offset    = info.offset;
				set_value_prior(
					value_prior_id_out,
					const_value_out,
					offset,
					s_info_vec[smooth_id]
				);
			}
		}
	}

	// get prioirs for measurement covariates
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	// measurement mean covariates for this integrand
		size_t n_cov = pack_object.mulcov_meas_value_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.mulcov_meas_value_info(integrand_id, j);
			size_t offset    = info.offset;
			size_t smooth_id = info.smooth_id;
			set_value_prior(
				value_prior_id_out,
				const_value_out,
				offset,
				s_info_vec[smooth_id]
			);
		}
		// measurement std covariates for this integrand
		n_cov = pack_object.mulcov_meas_std_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.mulcov_meas_std_info(integrand_id, j);
			size_t offset    = info.offset;
			size_t smooth_id = info.smooth_id;
			set_value_prior(
				value_prior_id_out,
				const_value_out,
				offset,
				s_info_vec[smooth_id]
			);
		}
	}

	// done
	return ;
}
/*
------------------------------------------------------------------------------
$begin pack_diff_prior$$
$spell
	diff
	vec
	const
	CppAD
	struct
	dage
	dtime
	var
$$

$section Difference Priors For Packed Variables$$

$head Syntax$$
$icode%diff_prior% = pack_diff_prior(%pack_object%, %s_info_vec%)%$$

$head pack_object$$
This argument has prototype
$codei%
	const pack_info& %pack_object%
%$$
and is the $cref pack_info$$ information corresponding to
the $cref model_variables$$.

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

$head diff_prior$$
The return value has prototype
$codei%
	CppAD::vector<diff_prior_struct> %diff_prior%
%$$
It contains one $code diff_prior_struct$$
for each pair of packed variable indices that have a
$cref/dage/smooth_grid_table/dage_prior_id/$$ or
$cref/dtime/smooth_grid_table/dtime_prior_id/$$ prior.
Let $icode%n_diff_prior% = %diff_prior%.size()%$$.
For $icode%k% = 0 , %...%, %n_diff_prior%-1%$$:
$table
$icode%diff_prior%[%k%].plus_var_id%$$ $cnext
	packed variable index of positive term
$rnext
$icode%diff_prior%[%k%].minus_var_id%$$ $cnext
	packed variable index of negative term
$rnext
$icode%diff_prior%[%k%].prior_id%$$ $cnext
	primary key in prior table for this difference
$tend

$children%
	example/devel/utility/pack_diff_prior_xam.cpp
%$$
$head Example$$
The file $cref pack_diff_prior_xam.cpp$$
contains an example and test that uses this routine.
$end
*/


CppAD::vector<diff_prior_struct> pack_diff_prior(
	const pack_info&                     pack_object  ,
	const CppAD::vector<smooth_info>&    s_info_vec   )
{	pack_info::subvec_info info;

	// initilaize return value as empty vector
	CppAD::vector<diff_prior_struct> ret_val;

	// some parameters
	size_t n_child     = pack_object.child_size();
	size_t n_integrand = pack_object.integrand_size();

	// no difference priors for standard devaition multipliers

	// get priors for rates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	for(size_t j = 0; j <= n_child; j++)
		{	info             = pack_object.rate_info(rate_id, j);
			size_t smooth_id = info.smooth_id;
			if( smooth_id != DISMOD_AT_NULL_SIZE_T )
			{	size_t offset    = info.offset;
				set_diff_prior(ret_val, offset, s_info_vec[smooth_id]);
			}
		}
	}

	// get priors for rate mean covariates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t n_cov = pack_object.mulcov_rate_value_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.mulcov_rate_value_info(rate_id, j);
			size_t offset    = info.offset;
			size_t smooth_id = info.smooth_id;
			set_diff_prior(ret_val, offset, s_info_vec[smooth_id]);
		}
	}

	// get priors for measurement covariates
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	// measurement mean covariates for this integrand
		size_t n_cov = pack_object.mulcov_meas_value_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.mulcov_meas_value_info(integrand_id, j);
			size_t offset    = info.offset;
			size_t smooth_id = info.smooth_id;
			set_diff_prior(ret_val, offset, s_info_vec[smooth_id]);
		}
		// measurement std covariates for this integrand
		n_cov = pack_object.mulcov_meas_std_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.mulcov_meas_std_info(integrand_id, j);
			size_t offset    = info.offset;
			size_t smooth_id = info.smooth_id;
			set_diff_prior(ret_val, offset, s_info_vec[smooth_id]);
		}
	}
# ifndef NDEBUG
	size_t n_var = pack_object.size();
	for(size_t k = 0; k < ret_val.size(); k++)
	{	assert( ret_val[k].plus_var_id < n_var );
		assert( ret_val[k].minus_var_id < n_var );
	}
# endif
	// done
	return ret_val;
}
// ----------------------------------------------------------------------------
namespace {
	void set_prior(
		CppAD::vector<var_prior_struct>&   result  ,
		size_t                             offset  ,
		const dismod_at::smooth_info&      s_info  )
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
				// value prior
				result[var_id].value_prior_id = s_info.value_prior_id(i, j);
				result[var_id].const_value    = s_info.const_value(i, j);
				//
				// dage prior
				result[var_id].dage_prior_id = s_info.dage_prior_id(i, j);
				CPPAD_ASSERT_UNKNOWN( i + 1 < n_age ||
					result[var_id].dage_prior_id == DISMOD_AT_NULL_SIZE_T
				);
				//
				// dtime_prior
				result[var_id].dtime_prior_id = s_info.dtime_prior_id(i, j);
				CPPAD_ASSERT_UNKNOWN( j + 1 < n_time ||
					result[var_id].dtime_prior_id == DISMOD_AT_NULL_SIZE_T
				);
			}
		}
	}
}
/*
-------------------------------------------------------------------------------
$begin pack_var_prior$$
$spell
	var
	vec
	struct
	const
	dage
	dtime
$$

$section Priors in Variable ID Order$$

$head Syntax$$
$icode%var2prior% = pack_var_prior(%pack_object%, %s_info_vec%)
%$$

$head Prototype$$
$srcfile%devel/utility/pack_prior.cpp
	%0%// BEGIN PACK_VAR_PRIOR_PROTOTYPE%// END PACK_VAR_PRIOR_PROTOTYPE%1%
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
For each $cref/var_id/var_table/var_id/$$,
$icode%var2prior%[%var_id%]%$$ contains the corresponding prior information
for the value.

$head var_prior_struct$$
For each $icode var_id$$, the corresponding $code var_prior_struct$$
contains the following information:

$subhead n_time$$
This is the distance between age grid points at the same time.
The distance between time points at the same age is one.

$subhead const_value$$
Either this is nan, $icode value_prior_id$$ is null,
but not both.
If  $icode value_prior_id$$ is null,
$icode const_value$$ is the value at this variable is constrained to.

$subhead value_prior_id$$
Either this is $code null$$, $icode const_value$$ is nan,
but not both.
If  $icode const_value$$ is nan,
$icode value_prior_id$$ identifies the prior for
the value of this variable.

$subhead dage_prior_id$$
If $icode dage_prior_id$$ is null,
this variable corresponding to the maximum age in a smoothing grid.
Otherwise $icode dage_prior_id$$ identifies the  prior for
the difference between the variable with index
$icode var_id$$ and the variable with index $icode%var_id% + %n_time%$$.

$subhead dtime_prior_id$$
If $icode dtime_prior_id$$ is null,
this variable corresponding to the maximum time in a smoothing grid.
Otherwise $icode dtime_prior_id$$ identifies the  prior for
the difference between the variable with index
$icode var_id$$ and the variable with index $icode%var_id% + 1%$$.

$end
-------------------------------------------------------------------------------
*/
// BEGIN PACK_VAR_PRIOR_PROTOTYPE
CppAD::vector<var_prior_struct>  pack_var_prior(
	const pack_info&                   pack_object ,
	const CppAD::vector<smooth_info>&  s_info_vec  )
// END PACK_VAR_PRIOR_PROTOTYPE
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
	// initialize all values as not defined
	CppAD::vector<var_prior_struct> var2prior(n_var);
	for(size_t var_id = 0; var_id < n_var; ++var_id)
	{	var2prior[var_id].const_value    = nan;
		var2prior[var_id].value_prior_id = DISMOD_AT_NULL_SIZE_T;
		var2prior[var_id].dage_prior_id  = DISMOD_AT_NULL_SIZE_T;
		var2prior[var_id].dtime_prior_id = DISMOD_AT_NULL_SIZE_T;
	}
	// -----------------------------------------------------------------------
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
				var2prior[offset].value_prior_id = prior_id;
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
				set_prior(var2prior, offset, s_info_vec[smooth_id]);
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
			set_prior(var2prior, offset, s_info_vec[smooth_id]);
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
			set_prior(var2prior, offset, s_info_vec[smooth_id]);
		}
		// measurement std covariates for this integrand
		n_cov = pack_object.mulcov_meas_std_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info   = pack_object.mulcov_meas_std_info(integrand_id, j);
			size_t offset    = info.offset;
			size_t smooth_id = info.smooth_id;
			set_prior(var2prior, offset, s_info_vec[smooth_id]);
		}
	}
	return var2prior;
}


} // END_DISMOD_AT_NAMESPACE
