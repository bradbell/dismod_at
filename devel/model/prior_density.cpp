// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin prior_density$$
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

$section Construct Prior Density Object$$

$head Syntax$$
$codei%prior_density %prior_object%(
	%pack_object%, %age_table%, %time_table%, %prior_table%, %s_info_vec%
)%$$

$head pack_object$$
This argument has prototype
$codei%
	const pack_info& %pack_object%
%$$
and is the $cref pack_info$$ information corresponding to
$icode pack_vec$$.

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
$cref/eta/prior_table/eta/$$.

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
# include <dismod_at/prior_density.hpp>
# include <dismod_at/residual_density.hpp>
# include <dismod_at/prior_density.hpp>
# include <dismod_at/smooth_info.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

prior_density::prior_density(
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
residual_struct<Float> prior_density::log_prior_density(
	residual_type_enum    type     ,
	const prior_struct&   prior    ,
	const Float&          variable ) const
{	assert ( 0 <= prior.density_id  );
	assert ( prior.density_id < number_density_enum );

	density_enum density = density_enum(prior.density_id);
	Float        mean    = Float(prior.mean);
	Float        std     = Float(prior.std);
	Float        eta     = Float(prior.eta);
	return residual_density(type, density, variable, mean, std, eta );
}

// private
template <class Float>
void prior_density::log_prior_density_on_grid(
	CppAD::vector< residual_struct<Float> >& residual_vec    ,
	residual_type_enum                       type            ,
	size_t                                   offset          ,
	const CppAD::vector<Float>&              pack_vec        ,
	const smooth_info&                       s_info          ) const
{	size_t n_age  = s_info.age_size();
	size_t n_time = s_info.time_size();

	// used to get results from log_prior_density
	residual_struct<Float> residual;

	// value smoothing
	for(size_t i = 0; i < n_age; i++)
	{	for(size_t j = 0; j < n_time; j++)
		{	Float  var      = Float(pack_vec[offset + i * n_time + j]);
			size_t prior_id           = s_info.value_prior_id(i, j);
			const prior_struct& prior = prior_table_[prior_id];
			residual                  = log_prior_density(type, prior, var);
			residual_vec.push_back(residual);
		}
	}
	// age difference smoothing
	for(size_t i = 0; i < (n_age-1); i++)
	{	double a0 = age_table_[ s_info.age_id(i) ];
		double a1 = age_table_[ s_info.age_id(i+1) ];
		assert( a1 > a0 );
		for(size_t j = 0; j < n_time; j++)
		{	Float  v0       = pack_vec[offset + i * n_time + j];
			Float  v1       = pack_vec[offset + (i+1) * n_time + j];
			Float  dv_da    = Float((v1 - v0) / (a1 - a0));
			size_t prior_id = s_info.dage_prior_id(i, j);
			const prior_struct& prior = prior_table_[prior_id];
			residual                  = log_prior_density(type, prior, dv_da);
			residual_vec.push_back(residual);
		}
	}
	// time difference smoothing
	for(size_t j = 0; j < (n_time-1); j++)
	{	double t0 = time_table_[ s_info.time_id(j) ];
		double t1 = time_table_[ s_info.time_id(j+1) ];
		assert( t1 > t0 );
		for(size_t i = 0; i < n_age; i++)
		{
			Float  v0       = pack_vec[offset + i * n_time + j];
			Float  v1       = pack_vec[offset + i * n_time + j + 1];
			Float  dv_dt    = Float((v1 - v0) / (t1 - t0));
			size_t prior_id = s_info.dtime_prior_id(i, j);
			const prior_struct& prior = prior_table_[prior_id];
			residual                  = log_prior_density(type, prior, dv_dt);
			residual_vec.push_back(residual);
		}
	}
	return;
}
/*
------------------------------------------------------------------------------
$begin prior_fixed_effect$$
$spell
$$

$section Evaluate Prior Density for the Fixed Effects$$

$head Syntax$$
$icode%residual_vec% = prior_object%.fixed(%pack_vec%)%$$

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, $code AD< AD<double> >$$,
where $code AD$$ is $code CppAD::AD$$.

$head residual_vec$$
The return value has prototype
$codei%
	CppAD::vector< residual_struct<%Float%> > %residual_vec%
%$$
The size of $icode residual$$ is not equal to the number of fixed effects
because there are priors on smoothing differences as well as values.
The order of the residuals is unspecified (at this time).

$head p(theta)$$
The log of the prior density for the
$cref/fixed/model_variable/Fixed Effects, theta/$$
$latex \B{p}( \theta )$$,
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
CppAD::vector< residual_struct<Float> > prior_density::fixed(
	const CppAD::vector<Float>&            pack_vec        ) const
{
	// initialize the log of the prior density as zero
	CppAD::vector< residual_struct<Float> > residual_vec;
	assert( residual_vec.size() == 0 );

	// used to get results from log_prior_density
	residual_type_enum     type  = fixed_prior_enum;
	residual_struct<Float> residual;

	// number of smoothings
	size_t n_smooth = s_info_vec_.size();

	// smoothing multipliers
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	// offset for this smoothing
		size_t offset = pack_object_.mulstd_offset(smooth_id);

		// multiplier for value smoothing
		Float mulstd = Float(pack_vec[offset + 0]);

		// prior index for this multilier
		size_t prior_id           = s_info_vec_[smooth_id].mulstd_value();
		const prior_struct* prior = &prior_table_[prior_id];

		// add prior density for this multipliers value
		residual            = log_prior_density(type, *prior, mulstd);
		residual_vec.push_back(residual);

		// add multiplier for age difference smoothing
		mulstd              = pack_vec[offset + 1];
		prior_id            = s_info_vec_[smooth_id].mulstd_dage();
		prior               = &prior_table_[prior_id];
		residual            = log_prior_density(type, *prior, mulstd);
		residual_vec.push_back(residual);

		// multiplier for time difference smoothing
		mulstd              = pack_vec[offset + 2];
		prior_id            = s_info_vec_[smooth_id].mulstd_dtime();
		prior               = &prior_table_[prior_id];
		residual            = log_prior_density(type, *prior, mulstd);
		residual_vec.push_back(residual);
	}

	// rates
	pack_info::subvec_info info;
	size_t n_child = pack_object_.child_size();
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	// child value that corresponds to the parent
		size_t child = n_child;

		info = pack_object_.rate_info(rate_id, child);
		size_t smooth_id          = info.smooth_id;
		size_t offset             = info.offset;
		const smooth_info& s_info = s_info_vec_[smooth_id];

		log_prior_density_on_grid(
			residual_vec,
			type        ,
			offset      ,
			pack_vec    ,
			s_info
		);
	}

	// rate covariate multipliers
	type = fixed_prior_enum;
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t n_cov = pack_object_.rate_mean_mulcov_n_cov( rate_id );
		for(size_t cov = 0; cov < n_cov; cov++)
		{	info = pack_object_.rate_mean_mulcov_info(rate_id, cov);
			size_t smooth_id          = info.smooth_id;
			size_t offset             = info.offset;
			const smooth_info& s_info = s_info_vec_[smooth_id];

			log_prior_density_on_grid(
				residual_vec,
				type        ,
				offset      ,
				pack_vec    ,
				s_info
			);
		}
	}

	// measurement covariate multipliers
	type = fixed_prior_enum;
	size_t n_integrand = pack_object_.integrand_size();
	for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
	{	size_t n_cov = pack_object_.meas_mean_mulcov_n_cov( integrand_id );
		for(size_t cov = 0; cov < n_cov; cov++)
		{	info = pack_object_.meas_mean_mulcov_info(integrand_id, cov);
			size_t smooth_id          = info.smooth_id;
			size_t offset             = info.offset;
			const smooth_info& s_info = s_info_vec_[smooth_id];

			log_prior_density_on_grid(
				residual_vec,
				type        ,
				offset      ,
				pack_vec    ,
				s_info
			);
		}
		n_cov = pack_object_.meas_std_mulcov_n_cov( integrand_id );
		for(size_t cov = 0; cov < n_cov; cov++)
		{	info = pack_object_.meas_std_mulcov_info(integrand_id, cov);
			size_t smooth_id          = info.smooth_id;
			size_t offset             = info.offset;
			const smooth_info& s_info = s_info_vec_[smooth_id];

			log_prior_density_on_grid(
				residual_vec,
				type        ,
				offset      ,
				pack_vec    ,
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
	enum
	eval
	pack_vec
	CppAD
	const
	subvectors
	std
	struct
	fabs
	var
$$

$section Evaluate Prior Density for the Random Effects$$

$head Syntax$$
$icode%residual_vec% = prior_object%.random(%pack_vec%)%$$

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, $code AD< AD<double> >$$,
where $code AD$$ is $code CppAD::AD$$.

$head residual_vec$$
The return value has prototype
$codei%
	CppAD::vector< residual_struct<%Float%> > %residual_vec%
%$$
The size of $icode residual$$ is not equal to the number of random effects
because there are priors on smoothing differences as well as values.
The order of the residuals is unspecified (at this time).

$head p(u|theta)$$
The log of the prior density for the
and $cref/random/model_variable/Random Effects, u/$$ effects
given the fixed effect,
$latex \B{p}( u | \theta )$$,
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
CppAD::vector< residual_struct<Float> > prior_density::random(
	const CppAD::vector<Float>&            pack_vec        ) const
{
	// initialize the log of the prior density as zero
	CppAD::vector< residual_struct<Float> > residual_vec;
	assert( residual_vec.size() == 0 );

	// used to get results from log_prior_density
	residual_type_enum     type = random_prior_enum;

	// rates
	pack_info::subvec_info info;
	size_t n_child = pack_object_.child_size();
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	// for all children and parent
		for(size_t child = 0; child < n_child; child++)
		{	info = pack_object_.rate_info(rate_id, child);
			size_t smooth_id          = info.smooth_id;
			size_t offset             = info.offset;
			const smooth_info& s_info = s_info_vec_[smooth_id];

			log_prior_density_on_grid(
				residual_vec,
				type        ,
				offset      ,
				pack_vec    ,
				s_info
			);
		}
	}
	return residual_vec;
}

# define DISMOD_AT_INSTANTIATE_PRIOR_DENSITY(Float)                       \
	template                                                              \
	CppAD::vector< residual_struct<Float> > prior_density::fixed<Float>(  \
		const CppAD::vector<Float>&   pack_vec                            \
	) const;                                                              \
	template                                                              \
	CppAD::vector< residual_struct<Float> > prior_density::random<Float>( \
		const CppAD::vector<Float>&   pack_vec                            \
	) const;

// instantiations
DISMOD_AT_INSTANTIATE_PRIOR_DENSITY(double)
DISMOD_AT_INSTANTIATE_PRIOR_DENSITY( CppAD::AD<double> )
DISMOD_AT_INSTANTIATE_PRIOR_DENSITY( CppAD::AD< CppAD::AD<double> > )

} // BEGIN_DISMOD_AT_NAMESPACE


