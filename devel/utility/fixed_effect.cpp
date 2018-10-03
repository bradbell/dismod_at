// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
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
$icode%n_fixed% = number_fixed(%pack_object%)
%$$
$icode%pack_index% = fixed2var_id(%pack_object%)
%$$
$codei%pack_fixed(%pack_object%, %pack_vec%, %fixed_vec%)
%$$
$codei%unpack_fixed(%pack_object%, %pack_vec%, %fixed_vec%)
%$$

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head Order of Fixed Effects$$
The order of the fixed effects is unspecified, except for the
fact that $code pack_fixed$$ and $code unpack_fixed$$
are inverses of each other; i.e., if you pack the fixed effects using a
$icode fixed_vec$$, and then do a unpack, you will get that
$icode fixed_vec$$ back.

$head pack_object$$
This argument has prototype
$codei%
	const pack_info& %pack_object%
%$$
It is the $cref pack_info$$ information corresponding
to the $cref model_variables$$.

$head n_fixed$$
This return value has prototype
$codei%
	size_t %n_fixed%
%$$
It is the number of
$cref/fixed effects/model_variables/Fixed Effects, theta/$$ in the model.

$head pack_index$$
This return value has prototype
$codei%
	CppAD::vector<size_t> %pack_index%
%$$
It size is equal to $icode n_fixed$$; i.e., the number of
$cref/fixed effects/model_variables/Fixed Effects, theta/$$ in the model.
For each fixed effect index
$icode%j%  = 0%,..., n_fixed%-1%$$,
the value $icode%pack_index%[%j%]%$$ is the corresponding
index in a packed vector (with both fixed and random effects).

$head unpack_fixed$$
This functions copies information from $icode pack_vec$$
to $icode fixed_vec$$.

$subhead pack_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %pack_vec%
%$$
and its size is $icode%pack_object%.size()%$$.
It specifies the value for all the $cref model_variables$$
in $cref pack_info$$ format.

$subhead fixed_vec$$
This argument has prototype
$codei%
	CppAD::vector<%Float%>& %fixed_vec%
%$$
and its size is $icode n_fixed$$.
It is a copy of the fixed effects in $icode pack_vec$$
as one contiguous vector in an unspecified order.


$head pack_fixed$$
This functions copies information from $icode fixed_vec$$
to $icode pack_vec$$.

$subhead pack_vec$$
This argument has prototype
$codei%
	CppAD::vector<%Float%>& %pack_vec%
%$$
and its size is $icode%pack_object%.size()%$$.
It specifies the value for all the $cref model_variables$$
in $cref pack_info$$ format.
The input value of its fixed effects does not matter.  Upon return,
the fixed effects correspond to the values in $icode fixed_vec$$.

$subhead fixed_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %fixed_vec%
%$$
and its size is $icode n_fixed$$.
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
# include <dismod_at/a1_double.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// ---------------------------------------------------------------------------
// number fixed
size_t number_fixed(const pack_info&  pack_object)
{	assert( pack_object.size() > pack_object.random_size() );
	return pack_object.size() - pack_object.random_size();
}
// ---------------------------------------------------------------------------
// fixed2var_id
CppAD::vector<size_t> fixed2var_id(const pack_info& pack_object )
{
	//
	size_t n_integrand = pack_object.integrand_size();
	size_t n_child     = pack_object.child_size();
	size_t n_smooth    = pack_object.smooth_size();

	pack_info::subvec_info info;
	size_t pack_index;
	size_t fixed_index = 0;
	size_t n_fixed     = number_fixed(pack_object);
	CppAD::vector<size_t> ret_val(n_fixed);

	// mulstd
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	for(size_t k = 0; k < 3; k++)
		{	pack_index = pack_object.mulstd_offset(smooth_id, k);
			if( pack_index != DISMOD_AT_NULL_SIZE_T )
				ret_val[fixed_index++] = pack_index;
		}
	}

	// parent rates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	info       = pack_object.rate_info(rate_id, n_child);
		if( info.smooth_id != DISMOD_AT_NULL_SIZE_T )
		{	pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				ret_val[fixed_index++] = pack_index++;
		}
	}

	// measurement mean and standard deviation covariates
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = pack_object.mulcov_meas_value_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object.mulcov_meas_value_info(integrand_id, j);
			pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				ret_val[fixed_index++] = pack_index++;
		}
		n_cov = pack_object.mulcov_meas_std_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object.mulcov_meas_std_info(integrand_id, j);
			pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				ret_val[fixed_index++] = pack_index++;
		}
	}

	// rate mean covariates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t n_cov = pack_object.mulcov_rate_value_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object.mulcov_rate_value_info(rate_id, j);
			pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				ret_val[fixed_index++] = pack_index++;
		}
	}
	assert( fixed_index == n_fixed );
	//
	return ret_val;
}
// ---------------------------------------------------------------------------
// unpack_fixed
template <class Float>
void unpack_fixed(
	const pack_info&             pack_object  ,
	const CppAD::vector<Float>&  pack_vec     ,
	CppAD::vector<Float>&        fixed_vec    )
{	assert( fixed_vec.size() == number_fixed(pack_object) );
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
	{	for(size_t k = 0; k < 3; k++)
		{	pack_index = pack_object.mulstd_offset(smooth_id, k);
			if( pack_index != DISMOD_AT_NULL_SIZE_T )
				fixed_vec[fixed_index++] = pack_vec[pack_index];
		}
	}

	// parent rates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	info       = pack_object.rate_info(rate_id, n_child);
		if( info.smooth_id != DISMOD_AT_NULL_SIZE_T )
		{	pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				fixed_vec[fixed_index++] = pack_vec[pack_index++];
		}
	}

	// measurement mean and standard deviation covariates
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = pack_object.mulcov_meas_value_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object.mulcov_meas_value_info(integrand_id, j);
			pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				fixed_vec[fixed_index++] = pack_vec[pack_index++];
		}
		n_cov = pack_object.mulcov_meas_std_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object.mulcov_meas_std_info(integrand_id, j);
			pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				fixed_vec[fixed_index++] = pack_vec[pack_index++];
		}
	}

	// rate mean covariates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t n_cov = pack_object.mulcov_rate_value_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object.mulcov_rate_value_info(rate_id, j);
			pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				fixed_vec[fixed_index++] = pack_vec[pack_index++];
		}
	}
	assert( fixed_index == fixed_vec.size() );

	return;
}
// ---------------------------------------------------------------------------
// pack_fixed
template <class Float>
void pack_fixed(
	const pack_info&             pack_object  ,
	CppAD::vector<Float>&        pack_vec     ,
	const CppAD::vector<Float>&  fixed_vec    )
{	assert( fixed_vec.size() == number_fixed(pack_object) );
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
	{	for(size_t k = 0; k < 3; k++)
		{	pack_index = pack_object.mulstd_offset(smooth_id, k);
			if( pack_index != DISMOD_AT_NULL_SIZE_T )
				pack_vec[pack_index] = fixed_vec[fixed_index++];
		}
	}

	// parent rates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	info       = pack_object.rate_info(rate_id, n_child);
		if( info.smooth_id != DISMOD_AT_NULL_SIZE_T )
		{	pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				pack_vec[pack_index++] = fixed_vec[fixed_index++];
		}
	}

	// measurement mean and standard deviation covariates
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = pack_object.mulcov_meas_value_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object.mulcov_meas_value_info(integrand_id, j);
			pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				pack_vec[pack_index++] = fixed_vec[fixed_index++];
		}
		n_cov = pack_object.mulcov_meas_std_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object.mulcov_meas_std_info(integrand_id, j);
			pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				pack_vec[pack_index++] = fixed_vec[fixed_index++];
		}
	}

	// rate mean covariates
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t n_cov = pack_object.mulcov_rate_value_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object.mulcov_rate_value_info(rate_id, j);
			pack_index = info.offset;
			for(size_t k = 0; k < info.n_var; k++)
				pack_vec[pack_index++] = fixed_vec[fixed_index++];
		}
	}
	assert( fixed_index == fixed_vec.size() );

	return;
}
// ---------------------------------------------------------------------------

# define DISMOD_AT_INSTANTIATE_FIXED_EFFECT(Float)        \
	template void unpack_fixed(                       \
	const pack_info&             pack_object  ,           \
	const CppAD::vector<Float>&  pack_vec     ,           \
	CppAD::vector<Float>&        fixed_vec                \
	);                                                    \
	template void pack_fixed(                       \
	const pack_info&             pack_object  ,           \
	CppAD::vector<Float>&        pack_vec     ,           \
	const CppAD::vector<Float>&  fixed_vec                \
	);

// instantiations
DISMOD_AT_INSTANTIATE_FIXED_EFFECT( double )
DISMOD_AT_INSTANTIATE_FIXED_EFFECT( a1_double )


} // END DISMOD_AT_NAMESPACE
