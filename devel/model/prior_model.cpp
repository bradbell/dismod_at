// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin prior_model_ctor$$
$spell
	pack_vec
	CppAD
	const
	subvectors
	std
	struct
	fabs
	var
$$

$section Construct Fixed Negative Log-Likelihood Object$$

$head Syntax$$
$codei%prior_model %prior_object%(
	%pack_object%, %age_table%, %time_table%, %prior_table%, %s_info_vec%
)%$$

$head prior_object$$
This object has prototype
$codei%
	prior_model %prior_object%
%$$

$head pack_object$$
This argument has prototype
$codei%
	const pack_info& %pack_object%
%$$
and is the $cref pack_info$$ information corresponding to the
$cref model_variables$$.

$head age_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>&  %age_table%
%$$
and is the $cref/age_table/get_age_table/age_table/$$.

$head time_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>&  %time_table%
%$$
and is the $cref/time_table/get_time_table/time_table/$$.

$head prior_table$$
This argument has prototype
$codei%
	const CppAD::vector<prior_struct>&  %prior_table%
%$$
and is the $cref/prior_table/get_prior_table/prior_table/$$.
Only to following fields are used
$cref/density_id/prior_table/density_id/$$,
$cref/mean/prior_table/mean/$$,
$cref/std/prior_table/std/$$,
$cref/eta/prior_table/eta/$$,
$cref/nu/prior_table/nu/$$.

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

$end
*/

# include <utility>
# include <dismod_at/prior_model.hpp>
# include <dismod_at/residual_density.hpp>
# include <dismod_at/prior_model.hpp>
# include <dismod_at/smooth_info.hpp>
# include <dismod_at/a2_double.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

prior_model::prior_model(
	const pack_info&                       pack_object     ,
	const CppAD::vector<double>&           age_table       ,
	const CppAD::vector<double>&           time_table      ,
	const CppAD::vector<prior_struct>&     prior_table     ,
	const CppAD::vector<smooth_info>&      s_info_vec      )
:
pack_object_(pack_object)  ,
age_table_(age_table)      ,
time_table_(time_table)    ,
prior_table_(prior_table)  ,
s_info_vec_(s_info_vec)
{ }

// private
template <class Float>
residual_struct<Float> prior_model::log_prior(
	const prior_struct&   prior      ,  // prior for this residual
	const Float&          mulstd     ,  // multiplies prior std
	const Float&          z          ,  // first random variable
	const Float&          y          ,  // second random variable
	size_t                index      ,  // the index for this residual
	bool                  difference ) const // is this a difference residual
{	assert ( 0 <= prior.density_id  );
	assert ( prior.density_id < number_density_enum );

	density_enum density = density_enum(prior.density_id);
	Float        mu      = Float(prior.mean);
	Float        delta   = mulstd * Float(prior.std);
	Float        eta     = Float(prior.eta);
	Float        nu      = Float(prior.nu);
	return residual_density(
		z, y, mu, delta, density, eta, nu, index, difference
	);
}

// private
template <class Float>
void prior_model::log_prior_on_grid(
	CppAD::vector< residual_struct<Float> >& residual_vec    ,
	size_t                                   offset          ,
	const CppAD::vector<Float>&              pack_vec        ,
	const CppAD::vector<Float>&              mulstd_vec      ,
	const smooth_info&                       s_info          ) const
{	size_t n_age  = s_info.age_size();
	size_t n_time = s_info.time_size();

	// used to get results from log_prior
	residual_struct<Float> residual;

	// value smoothing
	bool difference = false;
	Float not_used  = CppAD::numeric_limits<Float>::quiet_NaN();
	for(size_t i = 0; i < n_age; i++)
	{	for(size_t j = 0; j < n_time; j++)
		{	size_t var_id              = offset + i * n_time + j;
			Float  var                 = Float(pack_vec[var_id]);
			size_t prior_id            = s_info.value_prior_id(i, j);
			//
			// const_value priors correspond to uniform and have no residual
			if( prior_id != DISMOD_AT_NULL_SIZE_T )
			{	const prior_struct&  prior = prior_table_[prior_id];
				// use 3 * var_id + 0 for value priors
				size_t index               = 3 * var_id + 0;
				residual  = log_prior(
					prior, mulstd_vec[0], not_used, var, index, difference
				);
				// residuals for uniform densities are always zero
				if( residual.density != uniform_enum )
					residual_vec.push_back(residual);
			}
		}
	}
	// age difference smoothing
	difference = true;
	for(size_t i = 0; i < (n_age-1); i++)
	{
# ifndef NDEBUG
		double a0 = age_table_[ s_info.age_id(i) ];
		double a1 = age_table_[ s_info.age_id(i+1) ];
		assert( a1 > a0 );
# endif
		for(size_t j = 0; j < n_time; j++)
		{	size_t var_id   = offset + i * n_time + j;
			Float  v0       = pack_vec[var_id];
			Float  v1       = pack_vec[var_id + n_time];
			size_t prior_id           = s_info.dage_prior_id(i, j);
			const prior_struct& prior = prior_table_[prior_id];
			// use 3 * var_id + 1 for dage priors
			size_t index               = 3 * var_id + 1;
			residual  = log_prior(
				prior, mulstd_vec[1], v1,  v0, index, difference
			);
			if( residual.density != uniform_enum )
				residual_vec.push_back(residual);
		}
	}
	// time difference smoothing
	difference = true;
	for(size_t j = 0; j < (n_time-1); j++)
	{
# ifndef NDEBUG
		double t0 = time_table_[ s_info.time_id(j) ];
		double t1 = time_table_[ s_info.time_id(j+1) ];
		assert( t1 > t0 );
# endif
		for(size_t i = 0; i < n_age; i++)
		{	size_t var_id   = offset + i * n_time + j;
			Float  v0       = pack_vec[var_id];
			Float  v1       = pack_vec[var_id + 1];
			size_t prior_id           = s_info.dtime_prior_id(i, j);
			const prior_struct& prior = prior_table_[prior_id];
			// use 3 * var_id + 2 for dtime priors
			size_t index               = 3 * var_id + 2;
			residual  = log_prior(
				prior, mulstd_vec[2], v1, v0, index, difference
			);
			if( residual.density != uniform_enum )
				residual_vec.push_back(residual);
		}
	}
	return;
}
/*
------------------------------------------------------------------------------
$begin prior_fixed_effect$$
$spell
	vec
	Cpp
	struct
	const
$$

$section Evaluate Fixed Negative Log-Likelihood for the Fixed Effects$$

$head Syntax$$
$icode%residual_vec% = %prior_object%.fixed(%pack_vec%)%$$

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, or $cref a2_double$$.

$head prior_object$$
This object has prototype
$codei%
	const prior_model %prior_object%
%$$
see $cref/prior_object constructor/prior_model_ctor/prior_object/$$.

$head pack_vec$$
This vector has prototype
$codei%
	const CppAD::vector<%Float%>& %pack_vec%
%$$
and is all the $cref model_variables$$ in the order
specified by $cref pack_info$$.

$head residual_vec$$
The return value has prototype
$codei%
	CppAD::vector< residual_struct<%Float%> > %residual_vec%
%$$
The size of $icode residual$$ is not equal to the number of fixed effects
because there are priors on smoothing differences as well as values.
The order of the residuals is unspecified (at this time).

$head p(theta)$$
The log of the fixed negative log-likelihood for the
$cref/fixed effects/model_variables/Fixed Effects, theta/$$,
$latex \log \; \B{p}( \theta )$$,
is the sum of all the log densities corresponding to the
$cref/residuals/residual_density/$$.


$children%example/devel/model/prior_fixed_xam.cpp
%$$
$head Example$$
The file $cref prior_fixed_xam.cpp$$ contains an example and test
of using this routine.

$end
*/
template <class Float>
CppAD::vector< residual_struct<Float> > prior_model::fixed(
	const CppAD::vector<Float>&            pack_vec        ) const
{
	// initialize the log of the fixed negative log-likelihood as zero
	CppAD::vector< residual_struct<Float> > residual_vec;
	assert( residual_vec.size() == 0 );

	// used to get results from log_prior
	residual_struct<Float> residual;

	// number of smoothings
	size_t n_smooth = s_info_vec_.size();

	// standard deviation multipliers
	Float not_used;
	bool difference = false;
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	for(size_t k = 0; k < 3; k++)
		{	size_t offset = pack_object_.mulstd_offset(smooth_id, k);
			if( offset != DISMOD_AT_NULL_SIZE_T )
			{
				// value of the multiplier
				Float mulstd = Float(pack_vec[offset]);

				// prior_id for this multiplier
				size_t prior_id;
				switch(k)
				{	case 0:
					prior_id = s_info_vec_[smooth_id].mulstd_value();
					break;

					case 1:
					prior_id = s_info_vec_[smooth_id].mulstd_dage();
					break;

					case 2:
					prior_id = s_info_vec_[smooth_id].mulstd_dtime();
					break;

					default:
					assert(false);
				}

				// prior for this multilier
				const prior_struct& prior = prior_table_[prior_id];

				// use 3 * var_id + 0 for value priors
				size_t index = 3 * offset + 0;

				// add fixed negative log-likelihood for this multiplier
				residual  = log_prior(
					prior, Float(1.0), not_used, mulstd, index, difference
				);
				if( residual.density != uniform_enum )
					residual_vec.push_back(residual);
			}
		}
	}
	// hold mulstd_vec values for one smooting
	CppAD::vector<Float> mulstd_vec(3);

	// rates
	pack_info::subvec_info info;
	size_t n_child = pack_object_.child_size();
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	// child value that corresponds to the parent
		size_t child = n_child;

		info = pack_object_.rate_info(rate_id, child);
		size_t smooth_id          = info.smooth_id;
		if( smooth_id != DISMOD_AT_NULL_SIZE_T )
		{
			const smooth_info& s_info = s_info_vec_[smooth_id];
			for(size_t k = 0; k < 3; k++)
			{	size_t offset = pack_object_.mulstd_offset(smooth_id, k);
				if( offset == DISMOD_AT_NULL_SIZE_T )
					mulstd_vec[k] = 1.0;
				else
					mulstd_vec[k] = pack_vec[offset];
			}
			log_prior_on_grid(
				residual_vec,
				info.offset ,
				pack_vec    ,
				mulstd_vec  ,
				s_info
			);
		}
	}

	// rate covariate multipliers
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t n_cov = pack_object_.mulcov_rate_value_n_cov( rate_id );
		for(size_t cov = 0; cov < n_cov; cov++)
		{	info = pack_object_.mulcov_rate_value_info(rate_id, cov);
			size_t smooth_id          = info.smooth_id;
			const smooth_info& s_info = s_info_vec_[smooth_id];
			for(size_t k = 0; k < 3; k++)
			{	size_t offset = pack_object_.mulstd_offset(smooth_id, k);
				if( offset == DISMOD_AT_NULL_SIZE_T )
					mulstd_vec[k] = 1.0;
				else
					mulstd_vec[k] = pack_vec[offset];
			}
			log_prior_on_grid(
				residual_vec,
				info.offset ,
				pack_vec    ,
				mulstd_vec  ,
				s_info
			);
		}
	}

	// measurement covariate multipliers
	size_t n_integrand = pack_object_.integrand_size();
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = pack_object_.mulcov_meas_value_n_cov( integrand_id );
		for(size_t cov = 0; cov < n_cov; cov++)
		{	info = pack_object_.mulcov_meas_value_info(integrand_id, cov);
			size_t smooth_id          = info.smooth_id;
			const smooth_info& s_info = s_info_vec_[smooth_id];
			for(size_t k = 0; k < 3; k++)
			{	size_t offset = pack_object_.mulstd_offset(smooth_id, k);
				if( offset == DISMOD_AT_NULL_SIZE_T )
					mulstd_vec[k] = 1.0;
				else
					mulstd_vec[k] = pack_vec[offset];
			}
			log_prior_on_grid(
				residual_vec,
				info.offset ,
				pack_vec    ,
				mulstd_vec  ,
				s_info
			);
		}
		n_cov = pack_object_.mulcov_meas_std_n_cov( integrand_id );
		for(size_t cov = 0; cov < n_cov; cov++)
		{	info = pack_object_.mulcov_meas_std_info(integrand_id, cov);
			size_t smooth_id          = info.smooth_id;
			const smooth_info& s_info = s_info_vec_[smooth_id];
			for(size_t k = 0; k < 3; k++)
			{	size_t offset = pack_object_.mulstd_offset(smooth_id, k);
				if( offset == DISMOD_AT_NULL_SIZE_T )
					mulstd_vec[k] = 1.0;
				else
					mulstd_vec[k] = pack_vec[offset];
			}
			log_prior_on_grid(
				residual_vec,
				info.offset ,
				pack_vec    ,
				mulstd_vec  ,
				s_info
			);
		}
	}
	return residual_vec;
}
/*
------------------------------------------------------------------------------
$begin prior_random_effect$$
$spell
	vec
	Cpp
	struct
	const
$$

$section Evaluate Fixed Negative Log-Likelihood for the Random Effects$$

$head Syntax$$
$icode%residual_vec% = %prior_object%.random(%pack_vec%)%$$

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, or $cref a2_double$$.

$head prior_object$$
This object has prototype
$codei%
	const prior_model %prior_object%
%$$
see $cref/prior_object constructor/prior_model_ctor/prior_object/$$.

$head pack_vec$$
This vector has prototype
$codei%
	const CppAD::vector<%Float%>& %pack_vec%
%$$
and is all the $cref model_variables$$ in the order
specified by $cref pack_info$$.

$head residual_vec$$
The return value has prototype
$codei%
	CppAD::vector< residual_struct<%Float%> > %residual_vec%
%$$
The size of $icode residual$$ is not equal to the number of random effects
because there are priors on smoothing differences as well as values.
The order of the residuals is unspecified (at this time).

$head p(u|theta)$$
The log of the fixed negative log-likelihood for the
and $cref/random effects/model_variables/Random Effects, u/$$
given the fixed effects,
$latex \log \; \B{p}( u | \theta )$$,
is the sum of the all the log densities corresponding to the
$cref/residuals/residual_density/$$.


$children%example/devel/model/prior_random_xam.cpp
%$$
$head Example$$
The file $cref prior_random_xam.cpp$$ contains an example and test
of using this routine.

$end
*/
template <class Float>
CppAD::vector< residual_struct<Float> > prior_model::random(
	const CppAD::vector<Float>&            pack_vec        ) const
{
	// initialize the log of the fixed negative log-likelihood as zero
	CppAD::vector< residual_struct<Float> > residual_vec;
	assert( residual_vec.size() == 0 );

	// standard deviations multipliers for one smoothing
	CppAD::vector<Float> mulstd_vec(3);

	// rates
	pack_info::subvec_info info;
	size_t n_child = pack_object_.child_size();
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	// for all children and parent
		for(size_t child = 0; child < n_child; child++)
		{	info = pack_object_.rate_info(rate_id, child);
			size_t smooth_id          = info.smooth_id;
			if( smooth_id != DISMOD_AT_NULL_SIZE_T )
			{
				const smooth_info& s_info = s_info_vec_[smooth_id];
				for(size_t k = 0; k < 3; k++)
				{	size_t offset = pack_object_.mulstd_offset(smooth_id, k);
					if( offset == DISMOD_AT_NULL_SIZE_T )
						mulstd_vec[k] = 1.0;
					else
						mulstd_vec[k] = pack_vec[offset];
				}
				log_prior_on_grid(
					residual_vec,
					info.offset ,
					pack_vec    ,
					mulstd_vec  ,
					s_info
				);
			}
		}
	}
	return residual_vec;
}

# define DISMOD_AT_INSTANTIATE_PRIOR_DENSITY(Float)                       \
	template                                                              \
	CppAD::vector< residual_struct<Float> > prior_model::fixed<Float>(  \
		const CppAD::vector<Float>&   pack_vec                            \
	) const;                                                              \
	template                                                              \
	CppAD::vector< residual_struct<Float> > prior_model::random<Float>( \
		const CppAD::vector<Float>&   pack_vec                            \
	) const;

// instantiations
DISMOD_AT_INSTANTIATE_PRIOR_DENSITY(double)
DISMOD_AT_INSTANTIATE_PRIOR_DENSITY( CppAD::AD<double> )
DISMOD_AT_INSTANTIATE_PRIOR_DENSITY( a2_double )

} // BEGIN_DISMOD_AT_NAMESPACE


