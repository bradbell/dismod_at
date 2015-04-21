// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin fixed_effect$$
$spell
	vec
	CppAD
	var
	cpp
	const
$$

$section Setting and Getting the Fixed Effect Vector$$

$head Syntax$$
$icode%size_fixed% = size_fixed_effect(%pack_object%)
%$$
$codei%put_fixed_effect(%pack_object%, %pack_vec%, %fixed_vec%)
%$$
$codei%get_fixed_effect(%pack_object%, %pack_vec%, %fixed_vec%)
%$$

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, or $cref a5_double$$.

$head Order of Fixed Effects$$
The order of the fixed effects is unspecified, except for the
fact that $code put_fixed_effect$$ and $code get_fixed_effect$$
are inverses of each other; i.e., if you pack the fixed effects using a
$icode fixed_vec$$, and then do a unpack, you will get that
$icode fixed_vec$$ back.

$head pack_object$$
This argument has prototype
$codei%
	const pack_info& %pack_object%
%$$
It is the $cref pack_info$$ information corresponding
to the $cref/model_variables/model_variable/$$.

$head size_fixed$$
This return value has prototype
$codei%
	size_t %size_fixed%
%$$
It is the number of
$cref/fixed effects/model_variable/Fixed Effects, theta/$$ in the model.

$head put_fixed_effect$$
This functions copies information from $icode pack_vec$$
to $icode fixed_vec$$.

$subhead pack_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %pack_vec%
%$$
and its size is $icode%pack_object%.size()%$$.
It specifies the value for all the $cref/model_variables/model_variable/$$
in $cref pack_info$$ format.

$subhead fixed_vec$$
This argument has prototype
$codei%
	CppAD::vector<%Float%>& %fixed_vec%
%$$
and its size is $icode size_fixed$$.
It is a copy of the fixed effects in $icode pack_vec$$
as one contiguous vector in an unspecified order.


$head get_fixed_effect$$
This functions copies information from $icode fixed_vec$$
to $icode pack_vec$$.

$subhead pack_vec$$
This argument has prototype
$codei%
	CppAD::vector<%Float%>& %pack_vec%
%$$
and its size is $icode%pack_object%.size()%$$.
It specifies the value for all the $cref/model_variables/model_variable/$$
in $cref pack_info$$ format.
The input value of its fixed effects does not matter.  Upon return,
the fixed effects correspond to the values in $icode fixed_vec$$.

$subhead fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %fixed_vec%
%$$
and its size is $icode size_fixed$$.
It contains the fixed effects
as one contiguous vector in an unspecified order.

$children%
	example/devel/utility/fixed_effect_xam.cpp
%$$
$head Example$$
The file $cref fixed_effect_xam.cpp$$
contains an example and test that uses this routine.

$end
*/

# include <dismod_at/pack_info.hpp>
# include <dismod_at/random_effect.hpp>
# include <dismod_at/a5_double.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

size_t size_fixed_effect(const pack_info&  pack_object)
{	assert( pack_object.size() > size_random_effect(pack_object) );
	return pack_object.size() - size_random_effect(pack_object);
}

template <class Float>
void get_fixed_effect(
	const pack_info&              pack_object  ,
	const CppAD::vector<Float>&  pack_vec   ,
	CppAD::vector<Float>&        fixed_vec )
{	assert( fixed_vec.size() == size_fixed_effect(pack_object) );
	assert( pack_vec.size()  == pack_object.size() );
	//
	size_t n_integrand = pack_object.integrand_size();
	size_t n_child     = pack_object.child_size();
	size_t n_smooth    = pack_object.smooth_size();


	pack_info::subvec_info info;
	size_t pack_index;
	size_t fixed_index = 0;

	// mulstd
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	pack_index = pack_object.mulstd_offset(smooth_id);
		for(size_t k = 0; k < 3; k++)
			fixed_vec[fixed_index++] = pack_vec[pack_index++];
	}

	// parent rates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	info       = pack_object.rate_info(rate_id, n_child);
		pack_index = info.offset;
		for(size_t k = 0; k < info.n_var; k++)
			fixed_vec[fixed_index++] = pack_vec[pack_index++];
	}

	// measurement mean and standard deviation covariates
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = pack_object.meas_mean_mulcov_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object.meas_mean_mulcov_info(integrand_id, j);
			pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				fixed_vec[fixed_index++] = pack_vec[pack_index++];
		}
		n_cov = pack_object.meas_std_mulcov_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object.meas_std_mulcov_info(integrand_id, j);
			pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				fixed_vec[fixed_index++] = pack_vec[pack_index++];
		}
	}

	// rate mean covariates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t n_cov = pack_object.rate_mean_mulcov_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object.rate_mean_mulcov_info(rate_id, j);
			pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				fixed_vec[fixed_index++] = pack_vec[pack_index++];
		}
	}
	assert( fixed_index == fixed_vec.size() );

	return;
}

template <class Float>
void put_fixed_effect(
	const pack_info&              pack_object  ,
	CppAD::vector<Float>&        pack_vec   ,
	const CppAD::vector<Float>&  fixed_vec  )
{	assert( fixed_vec.size() == size_fixed_effect(pack_object) );
	assert( pack_vec.size()  == pack_object.size() );
	//
	size_t n_integrand = pack_object.integrand_size();
	size_t n_child     = pack_object.child_size();
	size_t n_smooth    = pack_object.smooth_size();

	pack_info::subvec_info info;
	size_t pack_index;
	size_t fixed_index = 0;

	// mulstd
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	pack_index = pack_object.mulstd_offset(smooth_id);
		for(size_t k = 0; k < 3; k++)
			pack_vec[pack_index++] = fixed_vec[fixed_index++];
	}

	// parent rates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	info       = pack_object.rate_info(rate_id, n_child);
		pack_index = info.offset;
		for(size_t k = 0; k < info.n_var; k++)
			pack_vec[pack_index++] = fixed_vec[fixed_index++];
	}

	// measurement mean and standard deviation covariates
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = pack_object.meas_mean_mulcov_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object.meas_mean_mulcov_info(integrand_id, j);
			pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				pack_vec[pack_index++] = fixed_vec[fixed_index++];
		}
		n_cov = pack_object.meas_std_mulcov_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object.meas_std_mulcov_info(integrand_id, j);
			pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				pack_vec[pack_index++] = fixed_vec[fixed_index++];
		}
	}

	// rate mean covariates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t n_cov = pack_object.rate_mean_mulcov_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object.rate_mean_mulcov_info(rate_id, j);
			pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				pack_vec[pack_index++] = fixed_vec[fixed_index++];
		}
	}
	assert( fixed_index == fixed_vec.size() );

	return;
}

# define DISMOD_AT_INSTANTIATE_FIXED_EFFECT(Float)           \
	template void get_fixed_effect(                       \
	const pack_info&              pack_object  ,                \
	const CppAD::vector<Float>&  pack_vec   ,                \
	CppAD::vector<Float>&        fixed_vec                   \
	);                                                       \
	template void put_fixed_effect(                         \
	const pack_info&              pack_object  ,                \
	CppAD::vector<Float>&        pack_vec   ,                \
	const CppAD::vector<Float>&  fixed_vec                   \
	);

// instantiations
DISMOD_AT_INSTANTIATE_FIXED_EFFECT(double)
DISMOD_AT_INSTANTIATE_FIXED_EFFECT( CppAD::AD<double> )
DISMOD_AT_INSTANTIATE_FIXED_EFFECT( a5_double )


} // END DISMOD_AT_NAMESPACE
