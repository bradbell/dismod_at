// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin data_model_ctor$$
$spell
	avgint
	logden
	std
	vec
	const
	enum
	struct
	Cpp
	obj
	covariate
	covariates
	cov
	cv
	eigen
$$

$section Data Model: Constructor$$

$head Syntax$$
$codei%data_model %data_object%(
	%bound_random%,
	%n_covariate%,
	%n_age_ode%,
	%n_time_ode%,
	%ode_step_size%,
	%age_table%,
	%time_table%,
	%integrand_table%,
	%subset_object%,
	%subset_cov_value%,
	%w_info_vec%,
	%s_info_vec%,
	%pack_object%,
	%child_object%
)%$$

$head data_object$$
This object has prototype
$codei%
	data_model %data_object%
%$$

$head bound_random$$
This argument has prototype
$codei%
	double %bound_random%
%$$
and is the
$cref/bound_random/option_table/Random Only/bound_random/$$.

$head n_covariate$$
This argument has prototype
$codei%
	size_t %n_covariate%
%$$
It is the number of covariates; i.e., number or rows in
$cref covariate_table$$.

$head n_age_ode$$
This argument has prototype
$codei%
	size_t %n_age_ode%
%$$
It is the number of points in the
$cref/ode age grid/ode_grid/Age, a_i/$$.

$head n_time_ode$$
This argument has prototype
$codei%
	size_t %n_time_ode%
%$$
It is the number of points in the
$cref/ode time grid/ode_grid/Time, t_j/$$.

$head ode_step_size$$
This argument has prototype
$codei%
	double %ode_step_size%
%$$
and is the value of $cref/ode_step_size/option_table/ode_step_size/$$
in the fit command.

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

$head integrand_table$$
This argument has prototype
$codei%
	const CppAD::vector<integrand_struct>&  %integrand_table%
%$$
and is the $cref/integrand_table/get_integrand_table/integrand_table/$$.

$head subset_object$$
This argument has prototype
$codei%
	const CppAD::vector<data_subset_struct>&  %subset_object%
%$$
and is the sub-sampled version of the data or avgint table; see
$cref/data_subset_obj/data_subset/data_subset_obj/$$,
$cref/avgint_subset_obj/avgint_subset/avgint_subset_obj/$$.

$head subset_cov_value$$
This argument has prototype
$codei%
	const CppAD::vector<data_subset_struct>&  %subset_cov_value%
%$$
and is the sub-sampled version of the covariates; see
$cref/data_subset_cov_value/data_subset/data_subset_cov_value/$$,
$cref/avgint_subset_cov_value/avgint_subset/avgint_subset_cov_value/$$.

$head w_info_vec$$
This argument has prototype
$codei%
	const CppAD::vector<weight_info>& %w_info_vec%
%$$
For each $cref/weight_id/weight_table/weight_id/$$,
$codei%
	%w_info_vec%[ %weight_id% ]
%$$
is the corresponding $cref weight_info$$ information.

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
For each $icode%s_info_vec%[%smooth_id%]%$$ object,
only the following functions are used:
$code age_size$$, $code time_size$$, $code age_id$$, $code time_id$$.

$head pack_object$$
This argument has prototype
$codei%
	const pack_info& %pack_object%
%$$
and is the $cref pack_info$$ information corresponding to
the $cref model_variables$$.

$head child_object$$
This argument has prototype
$codei%
	const child_info& %child_object%
%$$
and is the $cref child_info$$ information corresponding to
parent node, node table, and data table.

$head n_covariate_$$
set to $icode n_covariate$$.

$head n_age_ode_$$
set to $icode n_age_ode$$.

$head n_time_ode_$$
set to $icode n_time_ode$$.

$head ode_step_size_$$
set to $icode ode_step_size$$.

$head pack_object_$$
set to $icode pack_object$$.

$head minimum_meas_cv_$$
Set to a vector with the same size as $icode integrand_table$$ and
$codei%
	%minimum_meas_cv_[%integrand_id%] =
		%integrand_table%[%integrand_id%].minimum_meas_cv
%$$

$head eigen_ode2_case_number_$$
set to the default value.

$head replace_like_called_$$
initialize as false.

$head n_child_$$
set to number of children.

$head data_subset_obj_$$
for each $icode subset_id$$, set $codei%data_subset_obj_[%subset_id%]%$$
fields that are command both data_subset and avgint_subset.

$head si2ode_vec_$$
for each $icode smooth_id$$, set $codei%si2ode_vec_[%subset_id%]%$$
to interpolation structure for specified smoothing on the ode grid.

$head child_ran_zero_$$
for each $icode%child% < n_child_%$$,
$codei%child_ran_zero_%[%child%]%$$ is true if all the random effects
for this child are constrained to be zero.

$head data_info_$$
for each $icode subset_id$$, set $codei%data_info_[%subset_id%]%$$
is extra information used to speed up computation of average integrand
for the corresponding data point.


$end
-----------------------------------------------------------------------------
*/
# include <cppad/mixed/exception.hpp>
# include <dismod_at/min_max_vector.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/integrate_1d.hpp>
# include <dismod_at/integrate_2d.hpp>
# include <dismod_at/interp_weight.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/solve_ode.hpp>
# include <dismod_at/residual_density.hpp>
# include <dismod_at/a1_double.hpp>
# include <dismod_at/avgint_subset.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// destructor
data_model::~data_model(void)
{	// must delete the smooth2ode objects pointed to by si2ode_vec_
	size_t n_smooth = si2ode_vec_.size();
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
			delete si2ode_vec_[smooth_id];
}

// consctructor
template <class SubsetStruct>
data_model::data_model(
	double                                   bound_random    ,
	size_t                                   n_covariate     ,
	size_t                                   n_age_ode       ,
	size_t                                   n_time_ode      ,
	double                                   ode_step_size   ,
	const CppAD::vector<double>&             age_table       ,
	const CppAD::vector<double>&             time_table      ,
	const CppAD::vector<integrand_struct>&   integrand_table ,
	const CppAD::vector<prior_struct>&       prior_table     ,
	const CppAD::vector<SubsetStruct>&       subset_object   ,
	const CppAD::vector<double>&             subset_cov_value,
	const CppAD::vector<weight_info>&        w_info_vec      ,
	const CppAD::vector<smooth_info>&        s_info_vec      ,
	const pack_info&                         pack_object     ,
	const child_info&                        child_object    )
:
n_covariate_     (n_covariate)                   ,
n_age_ode_       (n_age_ode)                     ,
n_time_ode_      (n_time_ode)                    ,
ode_step_size_   (ode_step_size)                 ,
pack_object_     (pack_object)                   ,
n_child_         ( child_object.child_size() )
{	assert( bound_random >= 0.0 );
	assert( n_age_ode  > 1 );
	assert( n_time_ode > 1 );
	assert( n_child_ == pack_object.child_size() );
	//
	using std::string;
	//
	// minimum_meas_cv_
	minimum_meas_cv_.resize( integrand_table.size() );
	for(size_t i = 0; i < integrand_table.size(); ++i)
		minimum_meas_cv_[i] = integrand_table[i].minimum_meas_cv;
	//
	// eigen_ode2_case_number_: set default value
	eigen_ode2_case_number_ = 4;
	//
	// replace_like_called_: initialize
	replace_like_called_ = false;
	//
	// -----------------------------------------------------------------------
	// data_subset_obj_
	//
	// only set the fileds that are common to data_subset and avgint_subset
	size_t n_subset = subset_object.size();
	data_subset_obj_.resize(n_subset);
	assert( subset_cov_value.size() == n_covariate * n_subset );
	data_cov_value_.resize(n_subset * n_covariate);
	for(size_t i = 0; i < n_subset; i++)
	{	data_subset_obj_[i].original_id  = subset_object[i].original_id;
		data_subset_obj_[i].integrand_id = subset_object[i].integrand_id;
		data_subset_obj_[i].node_id      = subset_object[i].node_id;
		data_subset_obj_[i].weight_id    = subset_object[i].weight_id;
		data_subset_obj_[i].age_lower    = subset_object[i].age_lower;
		data_subset_obj_[i].age_upper    = subset_object[i].age_upper;
		data_subset_obj_[i].time_lower   = subset_object[i].time_lower;
		data_subset_obj_[i].time_upper   = subset_object[i].time_upper;
		for(size_t j = 0; j < n_covariate_; j++)
			data_cov_value_[i * n_covariate_ + j] =
				subset_cov_value[i * n_covariate + j];
	}
	// -----------------------------------------------------------------------
	// si2ode_vec_
	//
	// has same size as s_info_vec.
	size_t n_smooth = s_info_vec.size();
	si2ode_vec_.resize( n_smooth );
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	si2ode_vec_[smooth_id] = new smooth2ode(
			n_age_ode, n_time_ode, ode_step_size, age_table, time_table,
			s_info_vec[smooth_id]
		);
	}
	// -----------------------------------------------------------------------
	// child_ran_zero_
	double inf = std::numeric_limits<double>::infinity();
	child_ran_zero_.resize(n_child_);
	for(size_t child = 0; child < n_child_; ++child)
	{	child_ran_zero_[child] = true;
		for(size_t rate_id = 0; rate_id < number_rate_enum; ++rate_id)
		{	// check if any random effects for this rate are not constant
			size_t smooth_id = pack_object.rate_info(rate_id, child).smooth_id;
			if( smooth_id != DISMOD_AT_NULL_SIZE_T )
			{	const smooth_info& s_info = s_info_vec[smooth_id];
				size_t             n_age  = s_info.age_size();
				size_t             n_time = s_info.time_size();
				for(size_t i = 0; i < n_age; i++)
				{	for(size_t j = 0; j < n_time; j++)
					{	size_t prior_id    = s_info.value_prior_id(i, j);
						// if prior_id is null then const_value is not null
						if( prior_id != DISMOD_AT_NULL_SIZE_T )
						{
# ifndef NDEBUG
							double lower = prior_table[prior_id].lower;
# endif
							double upper = prior_table[prior_id].upper;
							assert( upper == inf  || upper == lower );
							assert( lower == -inf || upper == lower );
							bool zero    = upper == 0.0;
							zero        |= upper == inf && bound_random == 0.0;
							if( ! zero )
								child_ran_zero_[child] = false;
						}
					}
				}
			}
		}
	}
	// -----------------------------------------------------------------------
	// data_ode_info_
	//
	// has same size as data_subset_obj
	data_info_.resize( n_subset );
	//
	// limits of the ode grid
	double age_min    = min_vector( age_table );
	double time_min   = min_vector( time_table );
# ifndef NDEBUG
	double age_max    = max_vector( age_table );
	double time_max   = max_vector( time_table );
# endif
	//
	assert( age_max  <= age_min  + double(n_age_ode) * ode_step_size );
	assert( time_max <= time_min + double(n_time_ode) * ode_step_size );
	//
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	// information for this data point
		size_t original_id = subset_object[subset_id].original_id;

		// age limits
		double age_lower  = subset_object[subset_id].age_lower;
		double age_upper  = subset_object[subset_id].age_upper;
		assert( age_min <= age_lower );
		assert( age_upper <= age_max );
		assert( age_lower <= age_upper);

		// time limits
		double time_lower = subset_object[subset_id].time_lower;
		double time_upper = subset_object[subset_id].time_upper;
		assert( time_min <= time_lower );
		assert( time_upper <= time_max );
		assert( time_lower <= time_upper);

		// determine minimum ode grid age index
		size_t i_min = 0;
		while(age_min + double(i_min+1) * ode_step_size <= age_lower )
			i_min++;
		i_min = std::min(i_min, n_age_ode - 2);

		// determine number of ode age grid points
		size_t n_age = 2;
		while( age_min + double(i_min + n_age - 1) * ode_step_size < age_upper )
			n_age++;
		assert( i_min + n_age <= n_age_ode );

		// determine minimum ode grid time index
		size_t j_min = 0;
		while(time_min + double(j_min+1) * ode_step_size <= time_lower )
			j_min++;
		j_min  = std::min(j_min, n_age_ode - 2);

		// determine number of ode time grid point
		size_t n_time = 2;
		while( time_min + double(j_min + n_time - 1) * ode_step_size < time_upper )
			n_time++;
		assert( j_min + n_time <= n_time_ode );

		// initialize coefficient sum for each ode grid point within limits
		CppAD::vector<double> c_sum(n_age * n_time);
		for(size_t k = 0; k < n_age * n_time; k++)
			c_sum[k] = 0.0;

		// weighting for this data point
		size_t weight_id = subset_object[subset_id].weight_id;
		const weight_info& w_info( w_info_vec[weight_id] );

		// indices used to interpolate weighting
		size_t i_wi = 0;
		size_t j_wi = 0;

		// loop over all the ode rectangles that are within limits
		double eps = std::numeric_limits<double>::epsilon() * 100.0;
		CppAD::vector<double> w(4), c(4);
		std::pair<double, double> w_pair, c_pair;
		for(size_t i = 0; i < n_age-1; i++)
		{	// age ode grid points
			double a1 = age_min + double(i_min + i) * ode_step_size;
			double a2 = a1 + ode_step_size;
			//
			// clip to be within limits for this data point
			double b1 = std::max(a1, age_lower);
			double b2 = std::min(a2, age_upper);
			bool   b1_equal_b2 =
				std::fabs(b1 - b2) <= eps * ode_step_size;
			//
			std::pair<double, double> a_pair(a1, a2);
			std::pair<double, double> b_pair(b1, b2);
			//
			for(size_t j = 0; j < n_time-1; j++)
			{	// time ode grid points
				double t1 = time_min + double(j_min + j) * ode_step_size;
				double t2 = t1 + ode_step_size;
				//
				// clip to be within limits for this data point
				double s1 = std::max(t1, time_lower);
				double s2 = std::min(t2, time_upper);
				bool   s1_equal_s2 =
					std::fabs(s1 - s2) <= eps * ode_step_size;
				//
				// initialize contribution for this rectangle as zero
				c[0] = c[1] = c[2] = c[3] = 0.0;
				//
				std::pair<double, double> t_pair(t1, t2);
				std::pair<double, double> s_pair(s1, s2);
				//
				if( age_lower == age_upper && time_lower == time_upper )
				{	// case with no integration
					double a = b1;
					double t = s1;
					double d = (a2 - a1) * (t2 - t1);

					// coefficients of bilinear interpolator
					c[0]     = (a2 - a)*(t2 - t) / d;   // (a1, t1)
					c[1]     = (a2 - a)*(t - t1) / d;   // (a1, t2)
					c[2]     = (a - a1)*(t2 - t) / d;   // (a2, t1)
					c[3]     = (a - a1)*(t - t1) / d;   // (a2, t2)
				}
				else if( age_lower == age_upper )
				{	// case where only integrate w.r.t time
					if( ! s1_equal_s2 )
					{	double a = age_lower;
						double d = (a2 - a1);

						// weight at time t1
						w_pair.first = interp_weight(
							a, t1, w_info, age_table, time_table, i_wi, j_wi
						);

						// weight at time t2
						w_pair.second = interp_weight(
							a, t2, w_info, age_table, time_table, i_wi, j_wi
						);

						// coefficients for integrating w.r.t time
						c_pair = integrate_1d(t_pair, s_pair, w_pair);

						// coefficients for sourrounding age points
						c[0]   = c_pair.first  * (a2 - a) / d; // (a1, t1)
						c[1]   = c_pair.second * (a2 - a) / d; // (a1, t2)
						c[2]   = c_pair.first  * (a - a1) / d; // (a2, t1)
						c[3]   = c_pair.second * (a - a1) / d; // (a2, t2)
					}
				}
				else if( time_lower == time_upper )
				{	// case where only integrate w.r.t. age
					if( ! b1_equal_b2 )
					{	double t = time_lower;
						double d = (t2 - t1);

						// weight at age a1
						w_pair.first = interp_weight(
							a1, t, w_info, age_table, time_table, i_wi, j_wi
						);

						// weight at age a2
						w_pair.second = interp_weight(
							a2, t, w_info, age_table, time_table, i_wi, j_wi
						);

						// coefficients for integrating w.r.t. age
						c_pair = integrate_1d(a_pair, b_pair, w_pair);

						// coefficients for sourrounding time poins
						c[0]   = c_pair.first  * (t2 - t) / d; // (a1, t1)
						c[1]   = c_pair.first  * (t - t1) / d; // (a1, t2)
						c[2]   = c_pair.second * (t2 - t) / d; // (a2, t1)
						c[3]   = c_pair.second * (t - t1) / d; // (a2, t2)
					}
				}
				else
				{	// case where integrate w.r.t to age and time
					if( ! ( b1_equal_b2 || s1_equal_s2) )
					{	// weight at (a1, t1)
						w[0] = interp_weight(
							a1, t1, w_info, age_table, time_table, i_wi, j_wi);

						// weight at (a1, t2)
						w[1] = interp_weight(
							a1, t2, w_info, age_table, time_table, i_wi, j_wi);

						// weight at (a2, t1)
						w[2] = interp_weight(
							a2, t1, w_info, age_table, time_table, i_wi, j_wi);

						// weight at (a2, t2)
						w[3] = interp_weight(
							a2, t2, w_info, age_table, time_table, i_wi, j_wi);
						//
						c = integrate_2d(a_pair, t_pair, b_pair, s_pair, w);
					}
				}
				// add this rectangle contribution
				c_sum[     i * n_time + j   ]  += c[0];
				c_sum[     i * n_time + j+1 ]  += c[1];
				c_sum[ (i+1) * n_time + j   ]  += c[2];
				c_sum[ (i+1) * n_time + j+1 ]  += c[3];
			}
		}
		// compute normalization factor
		double sum = 0.0;
		for(size_t k = 0; k < n_age * n_time; k++)
			sum += c_sum[k];
		assert( sum > 0.0 );

		// integrand as an enum instead of integrand_id
		size_t  integrand_id     = subset_object[subset_id].integrand_id;
		integrand_enum integrand = integrand_table[integrand_id].integrand;

		// child of parent node that this data is associated with
		size_t  child            = child_object.table_id2child(original_id);

		// set the information for this data point
		data_info_[subset_id].integrand = integrand;
		data_info_[subset_id].child     = child;
		data_info_[subset_id].i_min     = i_min;
		data_info_[subset_id].j_min     = j_min;
		data_info_[subset_id].n_age     = n_age;
		data_info_[subset_id].n_time    = n_time;
		//
		data_info_[subset_id].c_ode.resize(n_age * n_time);
		for(size_t k = 0; k < n_age * n_time; k++)
			data_info_[subset_id].c_ode[k] = c_sum[k] / sum;

		// Does this data point depend on the random effects
		// that do not have equal bounds
		bool bound_ran_neq = false;
		if( child < n_child_ && bound_random > 0.0 )
		{	CppAD::vector<size_t> rate_id;
			switch( integrand )
			{	case Sincidence_enum:
				rate_id.push_back( size_t(iota_enum) );
				break;

				case remission_enum:
				rate_id.push_back( size_t(rho_enum) );
				break;

				case mtexcess_enum:
				rate_id.push_back( size_t(chi_enum) );
				break;

				case mtother_enum:
				rate_id.push_back( size_t(omega_enum) );
				break;

				case mtwith_enum:
				case relrisk_enum:
				rate_id.push_back( size_t(chi_enum) );
				rate_id.push_back( size_t(omega_enum) );
				break;

				default:
				for(size_t ell = 0; ell < number_rate_enum; ell++)
					rate_id.push_back( ell );
				break;
			}
			for(size_t ell = 0; ell < rate_id.size(); ell++)
			{	// check if any random effects for this rate are not constant
				size_t smooth_id =
					pack_object.rate_info(rate_id[ell], child).smooth_id;
				if( smooth_id != DISMOD_AT_NULL_SIZE_T )
				{	const smooth_info& s_info = s_info_vec[smooth_id];
					size_t             n_a  = s_info.age_size();
					size_t             n_t = s_info.time_size();
					for(size_t i = 0; i < n_a; i++)
					{	for(size_t j = 0; j < n_t; j++)
						{	size_t prior_id    = s_info.value_prior_id(i, j);
							// if prior_id is null then const_value is not null
							if( prior_id != DISMOD_AT_NULL_SIZE_T )
							{	double lower = prior_table[prior_id].lower;
								double upper = prior_table[prior_id].upper;
								if( lower != upper )
									bound_ran_neq = true;
							}
						}
					}
				}
			}
		}
		data_info_[subset_id].bound_ran_neq = bound_ran_neq;
	}
}
/*
$begin set_eigne_ode2_case_number$$
$spell
	avgint
	Dismod
	eigen
	arg
	const
$$

$section Sets the Method Used to Solve the Dismod_at ODE$$

$head Syntax$$
$icode%data_object%.set_eigen_ode2_case_number(%rate_case%)%$$

$head data_object$$
This object has prototype
$codei%
	data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.

$head rate_case$$
Is the $cref/option_value/option_table/Conventions/option_value/$$
corresponding to $icode%option_name% = rate_case%$$ in the option table.
$end
*/
void data_model::set_eigen_ode2_case_number(const std::string& rate_case)
{	/*
	b[0] = - ( iota + omega )
	b[1] = + rho
	b[2] = + iota
	b[3] = - ( rho + chi + omega );
	*/
	if( rate_case == "iota_zero_rho_zero" )
	{	eigen_ode2_case_number_ = 1;
		return;
	}
	if( rate_case == "iota_zero_rho_pos" )
	{	eigen_ode2_case_number_ = 2;
		return;
	}
	if( rate_case == "iota_pos_rho_zero" )
	{	eigen_ode2_case_number_ = 3;
		return;
	}
	if( rate_case == "iota_pos_rho_pos" )
	{	eigen_ode2_case_number_ = 4;
		return;
	}
	assert(false);
}
/*
$begin data_model_replace_like$$
$spell
	avgint
	const
	CppAD
	struct
	std
	obj
	cv
$$

$section Set Value Necessary for Likelihood (not for Average Integrand)$$

$head Syntax$$
$icode%data_object%.replace_like(%data_subset_obj%)%$$

$head Purpose$$
The values
$cref/density_id/data_table/density_id/$$,
$cref/hold_out/data_table/hold_out/$$,
$cref/meas_value/data_table/meas_value/$$,
$cref/meas_std/data_table/meas_std/$$ are not necessary to calculate
$cref/data_object.avg_no_ode/data_model_avg_no_ode/$$ and
$cref/data_object.avg_yes_ode/data_model_avg_yes_ode/$$.
However, the are necessary to use the functions
$cref/data_object.like_one/data_model_like_one/$$ and
$cref/data_object.like_all/data_model_like_all/$$.

$head data_object$$
This object has prototype
$codei%
	data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.

$subhead subset_object$$
We use $icode subset_object$$
for the $cref/subset_object/data_model_ctor/$$ in the $icode data_object$$
constructor.

$subhead n_subset$$
We use the notation
$codei%
	%n_subset% = %subset_object%.size()
%$$

$subhead subset_id$$
This an index between zero and $icode%n_subset% - 1%$$.
It is used to refer to the corresponding element of
$icode subset_object$$.

$head data_subset_obj$$
This argument has prototype
$codei%
	const CppAD::vector<data_subset_struct>& %data_subset_obj%
%$$
and has size $icode n_subset$$.
For each $icode subset_id$$,
and for $icode%field% = density_id%,% hold_out%,% meas_value%,% meas_std%$$,
$icode%data_subset_obj%[%subset_id%].%field%$$,
is used as a replacement for
$icode%subset_object[%subset_id%]%.%field%$$.

$head Laplace Density$$
The Laplace density is only allowed if the model for the
$cref avg_integrand$$ does not depend on the value of the random effects.
If this assumption is violated, an error messages is generated and the
program exits.

$end
*/
void data_model::replace_like(
		const CppAD::vector<data_subset_struct>&  data_subset_obj )
{
	// n_subset
	size_t n_subset = data_subset_obj_.size();
	assert( data_subset_obj.size() == n_subset );
	//
	// replace density_id, hold_out, meas_value, meas_std, eta, nu
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	data_subset_obj_[subset_id].density_id =
			data_subset_obj[subset_id].density_id;
		data_subset_obj_[subset_id].hold_out =
			data_subset_obj[subset_id].hold_out;
		data_subset_obj_[subset_id].meas_value =
			data_subset_obj[subset_id].meas_value;
		data_subset_obj_[subset_id].meas_std =
			data_subset_obj[subset_id].meas_std;
		data_subset_obj_[subset_id].eta =
			data_subset_obj[subset_id].eta;
		data_subset_obj_[subset_id].nu =
			data_subset_obj[subset_id].nu;
		//
		data_info_[subset_id].density = density_enum(
			data_subset_obj[subset_id].density_id
		);
		//
		bool laplace = data_info_[subset_id].density == laplace_enum;
		laplace     |= data_info_[subset_id].density == log_laplace_enum;
		if( laplace && data_info_[subset_id].bound_ran_neq )
		{	std::string msg, table_name;
			size_t data_id = data_subset_obj_[subset_id].original_id;
			table_name = "data";
			msg  = "density_id corresponds to laplace or log_laplace and\n";
			msg += "model depends on random effects that are not constrained";
			error_exit(msg, table_name, data_id);
		}
	}
	replace_like_called_ = true;
	return;
}
/*
-------------------------------------------------------------------------------
$begin data_model_reference_ode$$
$spell
	vec
	const
	CppAD
	Covariate
	covariates
$$

$section Solve for S and C Corresponding to Reference Covariate Values$$

$head Syntax$$
$icode%reference_sc% = %data_object%.reference_sc(%pack_vec%, %parent_only%)%$$

$head Prototype$$
$srcfile%devel/model/data_model.cpp%0
	%// BEGIN REFERENCE_SC_PROTOTYPE%// END REFERENCE_SC_PROTOTYPE%
1%$$

$head data_object$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head pack_vec$$
is all the $cref model_variables$$ in the order
specified by $cref pack_info$$.

$head parent_only$$
if true, only the parent values in $icode reference_sc$$ are calculated
($icode%c% = n_child_%$$).
The other elements in $icode reference_sc$$ are not defined.

$head reference_sc$$
The return value has prototype
$codei%
	const CppAD::vector<%Float%>& %reference_sc%
%$$
It's size is given by
$codei%
	%reference_sc%.size() = 2 * (n_child_ + 1) * %n_ode%
%$$
Let $icode%n_ode% = n_age_ode_ * n_time_ode_%$$.
For $icode%c% = 0, %...%, n_child_%$$,
for $icode%i% = 0, %...%, n_age_ode_-1%$$,
for $icode%j% = 0, %...%, n_time_ode_-1%$$,
$codei%
	%reference_sc%[ 2*(%child% * %n_ode% + %i% * n_time_ode_ + %j%) + 0]
%$$
is the susceptible fraction S and
$codei%
	%reference_sc%[ 2*(%child% * %n_ode% + %i% * n_time_ode_ + %j%) + 1]
%$$
is the with condition fraction C corresponding to the specified child
$icode c$$ and reference value for the rate covariates.
The child $icode%c% = n_child_%$$ corresponds to the parent.
If $icode%c% < n_child_%$$ and $code%child_ran_zero_[%c%]%$$ is true,
the corresponding values are not calculated because $icode%c% = n_child_%$$
can be used instead.

$end
*/

// BEGIN REFERENCE_SC_PROTOTYPE
template <class Float>
CppAD::vector<Float> data_model::reference_ode(
	const CppAD::vector<Float>& pack_vec     ,
	bool                        parent_only
) const
// END REFERENCE_SC_PROTOTYPE
{
	size_t n_ode = n_age_ode_ * n_time_ode_;
	CppAD::vector<Float> reference_sc( (n_child_+1) * n_ode * 2 );
	for(size_t k = 0; k < reference_sc.size(); ++k)
		reference_sc[k] = CppAD::numeric_limits<Float>::quiet_NaN();
	//
	// will not use integrand result
	integrand_enum integrand = susceptible_enum;
	//
	// x value correspnding to covariate reference
	CppAD::vector<double> x(n_covariate_);
	for(size_t j = 0; j < n_covariate_; ++j)
		x[j] = 0.0;
	//
	// computing new reference_sc, so do not use this argument
	CppAD::vector<Float> not_used(0);
	//
	for(size_t child = 0; child <= n_child_; ++child)
	{	bool skip = false;
		if( child < n_child_ )
			skip = parent_only || child_ran_zero_[child];
		if( ! skip )
		{
			// limits corresponding to entire ode grid
			size_t i_min      = 0;
			size_t j_min      = 0;
			size_t n_age_sub  = n_age_ode_;
			size_t n_time_sub = n_time_ode_;
			//
			// compute values for this child
			CppAD::vector<Float> sci_sub = sci_ode(
				integrand,
				i_min,
				j_min,
				n_age_sub,
				n_time_sub,
				child,
				x,
				pack_vec,
				not_used
			);
			//
			// copy to child_ode_grid_sc
			for(size_t i = 0; i < n_age_ode_; ++i)
			{	for(size_t j = 0; j < n_time_ode_; ++j)
				{	size_t k = 2 * (child * n_ode + i * n_time_ode_ + j);
					reference_sc[k + 0] = sci_sub[(i * n_time_ode_ + j)*3 + 0];
					reference_sc[k + 1] = sci_sub[(i * n_time_ode_ + j)*3 + 1];
				}
			}
		}
	}
	return reference_sc;
}
/*
-----------------------------------------------------------------------------
$begin data_model_sci_ode$$
$spell
	vec
	const
	CppAD
	subvectors
	std
	xam
	enum
	Tincidence
	mtspecific
	mtall
	mtstandard
	covariate
	covariates
$$
$section Use the ODE to Integrand for one Measurement$$

$head Syntax$$
$icode%sci_sub% = data_object%.sci_ode(
	%integrand%,
	%i_min%,
	%j_min%,
	%n_age_sub%,
	%n_time_sub%,
	%child%,
	%pack_vec%,
	%reference_sc%
)%$$

$head Prototype$$
$srcfile%devel/model/data_model.cpp%0
	%// BEGIN SCI_ODE_PROTOTYPE%// END SCI_ODE_PROTOTYPE%
1%$$

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head data_object$$
This object has prototype
$codei%
	const data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.

$head integrand$$
Is the integrand we are computing and must be one of the following:
$code susceptible_enum$$,
$code withC_enum$$,
$code prevalence_enum$$,
$code Tincidence_enum$$,
$code mtspecific_enum$$,
$code mtall_enum$$,
$code mtstandard_enum$$.
Note that when $icode integrand$$ is
$code susceptible_enum$$ or $code withC_enum$$
each integrand value appears twice in $icode sci_sub$$.

$head i_min$$
is the age index, in the $cref ode_grid$$, where the sub-grid starts.

$head j_min$$
is the time index, in the $cref ode_grid$$, where the sub-grid starts.

$head n_age_sub$$
is the number of age points in the sub-grid
$codei
	%i_min% + %n_age_sub% <= n_age_ode_
%$$

$head n_time_sub$$
is the number of time points in the sub-grid
$codei
	%j_min% + %n_time_sub% <= n_time_ode_
%$$

$head child$$
Is the child index for this computation.
If $icode%child% = n_child_%$$, this is computation is for the parent.

$head x$$
Is the value of the covariates for this computation,
$icode%x%.size() == n_covariate_%$$.

$head pack_vec$$
is all the $cref model_variables$$ in the order
specified by $cref pack_info$$.
Only the following subvectors of $icode pack_vec$$ are used:
$cref pack_info_rate_info$$,
$cref pack_info_mulcov_rate$$.

$head reference_sc$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %reference_sc%
%$$
If the size of this vector is zero, it is not used.
It's intended use is to avoid repeated solving of the ODE's corresponding
to the reference value for all of the covariates.
Let $icode%n_ode% = n_age_ode_ * n_time_ode_%$$.
If it's size is not zero,be
$codei%2 * (n_child_ + 1) * %n_ode%$$.
In this case,
for $icode%c% = 0, %...%, n_child_%$$,
for $icode%i% = 0, %...%, n_age_ode_-1%$$,
for $icode%j% = 0, %...%, n_time_ode_-1%$$,
$codei%
	%reference_sc%[ 2*(%child% * %n_ode% + %i% * n_time_ode_ + %j%) + 0]
%$$
is the susceptible fraction S and
$codei%
	%reference_sc%[ 2*(%child% * %n_ode% + %i% * n_time_ode_ + %j%) + 1]
%$$
is the with condition fraction C corresponding to the specified child
$icode c$$ and reference value for the rate covariates.
The child $icode%c% = n_child_%$$ corresponds to the parent.

$head sci_sub$$
The return value has size $icode%n_age_sub%*%n_time_sub%*3%$$.
For $icode%i%=0,...,%n_age_sub%-1%$$,
$icode%j%=0,...,%n_time_sub%-1%$$,
The corresponding ode grid index is
$codei%
	( %i_min% + %i% , %j_min% + %j% )
%$$.
The value
$codei%
	%sci_sub%[(%i% * %n_time_sub% + %j%)*3 + 0]
%$$
is the susceptible fraction $latex S(a, t)$$
at sub-grid pair $codei%(%i%, %j%)%$$.
The value
$codei%
	%sci_sub%[(%i% * %n_time_sub% + %j%)*3 + 1]
%$$
is the with condition fraction $latex C(a, t)$$
at sub-grid pair $codei%(%i%, %j%)%$$.
The value
$codei%
	%sci_sub%[(%i% * %n_time_sub% + %j%)*3 + 2]
%$$
is the value of the integrand
at sub-grid pair $codei%(%i%, %j%)%$$.

$children%example/devel/model/sci_ode_xam.cpp
%$$
$head Example$$
The file $cref sci_ode_xam.cpp$$ contains an example and test
of using this routine.

$end
*/
// BEGIN SCI_ODE_PROTOTYPE
template <class Float>
CppAD::vector<Float> data_model::sci_ode(
	integrand_enum                integrand     ,
	size_t                        i_min         ,
	size_t                        j_min         ,
	size_t                        n_age_sub     ,
	size_t                        n_time_sub    ,
	size_t                        child         ,
	const CppAD::vector<double>&  x             ,
	const CppAD::vector<Float>&   pack_vec      ,
	const CppAD::vector<Float>&   reference_sc
) const
// END SCI_ODE_PROTOTYPE
{	assert( pack_object_.size() == pack_vec.size() );
	assert( i_min + n_age_sub <= n_age_ode_ );
	assert( j_min + n_time_sub <= n_time_ode_ );
	assert( child <= n_child_ );
	assert( x.size() == n_covariate_ );

	/* -----------------------------------------------------------------------
	cohort_start, ode_index:
	chort_start[k] = index in ode_index for start of k-th cohort.
	For ell = ode_start[k], ... oed_start[k+1] - 1,
		ode_index[ell] = i * n_time_ode + j
	where (i, j) are age, time indices for ell-th grid point in k-th cohort.
	The first (i,j) pair for each cohort has i = 0. The last (i,j) pair for
	each cohort has i = i_min + n_age_sub - 1 or j = j_min + n_time_sub - 1.
	The cohorts include all the ODE grid points in range for this measurement.
	------------------------------------------------------------------------ */
	// information that maps cohorts to indices in the ODE grid
	CppAD::vector<size_t> cohort_start, ode_index;

	// maximum age index
	assert( n_age_sub > 0);
	size_t i_max = i_min + n_age_sub - 1;

	// maximum time index plus one
	assert( n_time_sub > 0 );
	size_t j_max = j_min + n_time_sub - 1;

	// cohorts that end at maximum age index i_max
	// j_end is the time index corresponding to end of cohort
	for(size_t j_end = j_min; j_end <= j_max; ++j_end)
	{	// k is index in ode_index at which this cohort integration starts
		size_t k = ode_index.size();
		cohort_start.push_back(k);
		//
		// age index for each (i,j) pair in this cohort
		for(size_t i = 0; i <= i_max; ++i)
		{	// time index for this (i,j) pair
			// j - i = j_end - i_max
			size_t j;
			if( j_end > i_max - i )
				j = j_end - (i_max - i);
			else
				j = 0;
			//
			// pack pair (i, j) into one number and put in this cohort
			ode_index.push_back(i * n_time_ode_ + j);
		}
	}

	// cohorts that end at maximum time index j_max
	// i_end is the time index corresponding to end of cohort
	// (case where i_end == i_max is included above)
	for(size_t i_end = i_min; i_end <= i_max; ++i_end)
	{	// k is index in ode_index at which this cohort integration starts
		size_t k = ode_index.size();
		cohort_start.push_back(k);
		//
		// age index for each (i,j) pair in this cohort
		for(size_t i = 0; i <= i_end; ++i)
		{	// time index for this (i,j) pair
			// j - i = j_max - i_end
			size_t j;
			if( j_max > i_end - i )
				j = j_max - (i_end - i);
			else
				j = 0;
			//
			// pack pair (i, j) into one number and put in this cohort
			ode_index.push_back(i * n_time_ode_ + j);
		}
	}

	// total numer of cohort indices
	size_t n_index = ode_index.size();

	/* --------------------------------------------------------------------
	rate_ode, use_reference_sc:
	For rate_id = 0, ..., n_rate-1, ell = 0, ..., n_index - 1
		rate_ode[rate_id][ell]
	is value of the specified rate at the (i, j) grid point specified by
		ode_index[ell] = i * n_time_ode + j
	---------------------------------------------------------------------- */
	CppAD::vector< CppAD::vector<Float> > rate_ode(number_rate_enum);
	pack_info::subvec_info                info;
	bool use_reference_sc = reference_sc.size() != 0;
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	rate_ode[rate_id].resize(n_index);
		//
		// parent information for this rate
		info             = pack_object_.rate_info(rate_id, n_child_);
		size_t smooth_id = info.smooth_id;
		size_t n_var;
		if( smooth_id == DISMOD_AT_NULL_SIZE_T )
		{	// in this case the parent rate is zero
			for(size_t k = 0; k < n_index; k++)
				rate_ode[rate_id][k] = 0.0;
		}
		else
		{	n_var     = info.n_var;
			//
			CppAD::vector<Float> rate_si(n_var);
			for(size_t k = 0; k < n_var; k++)
				rate_si[k] = pack_vec[info.offset + k];
			//
			// interpolate parent rate onto the ode grid
			rate_ode[rate_id] =
				si2ode_vec_[smooth_id]->interpolate(rate_si, ode_index);
		}
		//
		// initialize sum of effects to zero
		CppAD::vector<Float> effect_ode(n_index);
		for(size_t k = 0; k < n_index; k++)
			effect_ode[k] = 0.0;
		//
		// include child random effect for this rate
		if( child < n_child_ )
		{	info      = pack_object_.rate_info(rate_id, child);
			smooth_id = info.smooth_id;
			if( smooth_id != DISMOD_AT_NULL_SIZE_T )
			{	n_var     = info.n_var;
				//
				CppAD::vector<Float> var_si(n_var);
				for(size_t k = 0; k < n_var; k++)
					var_si[k] = pack_vec[info.offset + k];
				//
				// interpolate child rate effect onto the ode grid
				CppAD::vector<Float> var_ode =
					si2ode_vec_[smooth_id]->interpolate(var_si, ode_index);
				//
				for(size_t k = 0; k < n_index; k++)
					effect_ode[k] += var_ode[k];
			}
		}
		//
		// include effect of rate covariates
		size_t n_cov = pack_object_.mulcov_rate_value_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object_.mulcov_rate_value_info(rate_id, j);
			n_var      = info.n_var;
			smooth_id  = info.smooth_id;
			double x_j = x[ info.covariate_id ];
			//
			// x_j is relative to the reference covariate value
			use_reference_sc &= x_j == 0.0;
			//
			CppAD::vector<Float> var_si(n_var);
			for(size_t k = 0; k < n_var; k++)
				var_si[k] = pack_vec[info.offset + k];
			//
			// interpolate covariate multiplier onto the ode grid
			CppAD::vector<Float> var_ode =
				si2ode_vec_[smooth_id]->interpolate(var_si, ode_index);
			//
			// effect for this covariate
			for(size_t k = 0; k < n_index; k++)
				effect_ode[k] += var_ode[k] * x_j;
		}
		// multiply parent rate by exponential of total effect
		for(size_t k = 0; k < n_index; k++)
			rate_ode[rate_id][k] *= exp( effect_ode[k] );
	}

	/* -----------------------------------------------------------------------
	sci_ode
	---------------------------------------------------------------------- */
	size_t n_cohort  = cohort_start.size();
	size_t n_ode     = n_age_ode_ * n_time_ode_;
	size_t n_ode_sub = n_age_sub * n_time_sub;
	CppAD::vector<Float> iota, rho, chi, omega, S_out, C_out;
	Float zero = Float(0.0);
	CppAD::vector<Float> sci_sub(n_ode_sub * 3);
	//
	// initialize the output values as nan
	for(size_t k = 0; k < n_ode_sub * 3; k++)
		sci_sub[k] = CppAD::nan(zero);
	//
	// for each cohort
	for(size_t ell = 0; ell < n_cohort; ell++)
	{	// starting index for this cohort
		size_t k_start  = cohort_start[ell];
		// number of indices for this cohort
		size_t nk;
		if( ell+1 < n_cohort )
			nk = cohort_start[ell+1] - cohort_start[ell] ;
		else
			nk = ode_index.size() - cohort_start[ell];
		//
		// set iota, rho, chi, omega for this cohort.
		iota.resize(nk);
		rho.resize(nk);
		chi.resize(nk);
		omega.resize(nk);
		for(size_t k = 0; k < nk; k++)
		{	iota[k]  = rate_ode[iota_enum][k_start + k];
			rho[k]   = rate_ode[rho_enum][k_start + k];
			chi[k]   = rate_ode[chi_enum][k_start + k];
			omega[k] = rate_ode[omega_enum][k_start + k];
		}
		//
		if( use_reference_sc )
		{	S_out.resize(nk);
			C_out.resize(nk);
			for(size_t k = 0; k < nk; ++k)
			{	size_t l = 2 * (child * n_ode + ode_index[k_start + k]);
				if( child < n_child_ && child_ran_zero_[child] )
					l = 2 * (n_child_ * n_ode + ode_index[k_start + k]);
				S_out[k]   = reference_sc[l + 0];
				C_out[k]   = reference_sc[l + 1];
			}
		}
		else
		{
			// pini, iota, rho, cho, omega for this cohort
			Float pini   = Float(rate_ode[pini_enum][k_start]);
			//
			// S and C for this cohort
			S_out.resize(0);
			C_out.resize(0);
			i_max     = ode_index[k_start + nk - 1] / n_time_ode_;
			j_max     = ode_index[k_start + nk - 1] % n_time_ode_;
			Float  step_size = Float(ode_step_size_);
			//
			solve_ode(eigen_ode2_case_number_ ,
			i_max, j_max, step_size, pini, iota, rho, chi, omega, S_out, C_out
			);
		}
		//
		// extract part of cohort that is in the rectangle for this measurement
		Float infinity  = Float( std::numeric_limits<double>::infinity() );
		for(size_t k = 0; k < nk; k++)
		{	size_t i = ode_index[k_start + k] / n_time_ode_;
			size_t j = ode_index[k_start + k] % n_time_ode_;
			if( i_min <= i && j_min <= j )
			{	Float P   = C_out[k] / ( S_out[k]  + C_out[k]);
				bool ok = zero <= P && P < infinity;
				ok     |= integrand == susceptible_enum;
				ok     |= integrand == withC_enum;
				if( ! ok )
				{	std::string message = "Numerical integration error.\n"
					"Prevalence is negative or infinite or Nan.";
					throw CppAD::mixed::exception(
						"sci_ode", message
					);
				}
				size_t ij = (i - i_min) * n_time_sub + (j - j_min);
				sci_sub[ ij * 3 + 0 ] = S_out[k];
				sci_sub[ ij * 3 + 1 ] = C_out[k];
				switch(integrand)
				{	case susceptible_enum:
					sci_sub[ ij * 3 + 2 ] = S_out[k];
					break;

					case withC_enum:
					sci_sub[ ij * 3 + 2 ] = C_out[k];
					break;

					case prevalence_enum:
					sci_sub[ ij * 3 + 2 ] = P;
					break;

					case Tincidence_enum:
					sci_sub[ ij * 3 + 2 ] = iota[k] * (1.0 - P);
					break;

					case mtspecific_enum:
					sci_sub[ ij * 3 + 2 ] = chi[k] * P;
					break;

					case mtall_enum:
					sci_sub[ ij * 3 + 2 ] = omega[k] + chi[k] * P;
					break;

					case mtstandard_enum:
					sci_sub[ ij * 3 + 2 ] = omega[k] + chi[k];
					sci_sub[ ij * 3 + 2 ] /= omega[k] + chi[k] * P;
					break;

					// This integrand does not requre the ode; i.e., S or C
					default:
					assert(false);
				}
			}
		}
	}
	return sci_sub;
}


/*
-----------------------------------------------------------------------------
$begin data_model_avg_no_ode$$

$spell
	avgint
	Sincidence
	subvectors
	enum
	integrands
	mtexcess
	mtother
	mtwith
	relrisk
	var
	vec
	CppAD
	const
	Doesn't
$$
$section One Average Integrand That Doesn't Require the ODE$$

$head Syntax$$
$icode%avg% = %data_object%.avg_no_ode(%subset_id%, %pack_vec%)%$$

$head data_object$$
This object has prototype
$codei%
	const data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.


$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head subset_id$$
This argument has prototype
$codei%
	size_t %subset_id%
%$$
and is the $cref/subset_id/data_subset/data_subset_obj/subset_id/$$
we are computing the average integrand for.

$subhead Integrand and Rates$$
The $cref/integrand_id/data_table/integrand_id/$$ corresponding to this
$cref/subset_id/data_subset/data_subset_obj/subset_id/$$
must be one of those listed in the table below.
In addition, depending on the integrand, only the corresponding
$cref pack_info_rate_info$$ and $cref pack_info_mulcov_rate$$ subvectors of
$icode pack_vec$$ are used:
$table
Integrand               $cnext Rates               $rnext
$code Sincidence_enum$$ $cnext $code iota_enum$$   $rnext
$code remission_enum$$  $cnext $code rho_enum$$    $rnext
$code mtexcess_enum$$   $cnext $code chi_enum$$    $rnext
$code mtother_enum$$    $cnext $code omega_enum$$  $rnext
$code mtwith_enum$$     $cnext $code chi_enum$$, $code omega_enum$$ $rnext
$code relrisk_enum$$    $cnext $code chi_enum$$, $code omega_enum$$
$tend

$head pack_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %pack_vec%
%$$
and is all the $cref model_variables$$ in the order
specified by $cref pack_info$$.

$head avg$$
The return value has prototype
$codei%
	%Float% avg
%$$
This is the
$cref/average integrand/avg_integrand/Average Integrand, A_i/$$
for the specified data point.

$children%example/devel/model/avg_no_ode_xam.cpp
%$$
$head Example$$
The file $cref avg_no_ode_xam.cpp$$ contains an example and test
of using this routine.

$end
*/
template <class Float>
Float data_model::avg_no_ode(
	size_t                        subset_id ,
	const CppAD::vector<Float>&   pack_vec  ) const
{
	assert( pack_object_.size() == pack_vec.size() );

	// integrand_id
	size_t integrand_id = data_subset_obj_[subset_id].integrand_id;

	// covariate information for this data point
	CppAD::vector<double> x(n_covariate_);
	for(size_t j = 0; j < n_covariate_; j++)
			x[j] = data_cov_value_[subset_id * n_covariate_ + j];

	// data_info information for this data point
	integrand_enum integrand           = data_info_[subset_id].integrand;
	size_t i_min                       = data_info_[subset_id].i_min;
	size_t j_min                       = data_info_[subset_id].j_min;
	size_t n_age                       = data_info_[subset_id].n_age;
	size_t n_time                      = data_info_[subset_id].n_time;
	size_t child                       = data_info_[subset_id].child;
	const CppAD::vector<double>& c_ode = data_info_[subset_id].c_ode;

	// check that this data's node is for parent or one of its descendents
	assert( child <= n_child_ );

	// ode subgrid that we need integrand at
	size_t n_ode = n_age * n_time;
	CppAD::vector<size_t> ode_index(n_ode);
	for(size_t i = 0; i < n_age; i++)
	{	for(size_t j = 0; j < n_time; j++)
		{	size_t k = i * n_time + j;
			ode_index[k] = (i_min + i) * n_time_ode_ + j_min + j;
		}
	}

	// which rates are necessary for this integrand
	CppAD::vector<size_t> rate_id(2);
	rate_id[0] = number_rate_enum;
	rate_id[1] = number_rate_enum;
	switch(integrand)
	{
		case Sincidence_enum:
		rate_id[0] = size_t (iota_enum);
		break;

		case remission_enum:
		rate_id[0] = size_t (rho_enum);
		break;

		case mtexcess_enum:
		rate_id[0] = size_t (chi_enum);
		break;

		case mtother_enum:
		rate_id[0] = size_t (omega_enum);
		break;

		case mtwith_enum:
		case relrisk_enum:
		rate_id[0] = size_t (chi_enum);
		rate_id[1] = size_t (omega_enum);
		break;

		default:
		// use compute_avg_yes_ode for these cases
		assert( false );
	}


	// value for the rate on the ode subgrid
	CppAD::vector< CppAD::vector<Float> > rate_ode(2);
	for(size_t ell = 0; ell < 2; ell++) if( rate_id[ell] < number_rate_enum )
	{	rate_ode[ell].resize(n_ode);
		//
		// extract subvector information for the parent rate
		pack_info::subvec_info info;
		info             = pack_object_.rate_info(rate_id[ell], n_child_);
		size_t smooth_id = info.smooth_id;
		size_t n_var;
		if( smooth_id == DISMOD_AT_NULL_SIZE_T )
		{	for(size_t k = 0; k < n_ode; k++)
				rate_ode[ell][k] = 0.0;
		}
		else
		{	n_var     = info.n_var;
			//
			CppAD::vector<Float> rate_si(n_var);
			for(size_t k = 0; k < n_var; k++)
				rate_si[k] = pack_vec[info.offset + k];
			//
			// interpolate onto the ode grid
			rate_ode[ell] =
				si2ode_vec_[smooth_id]->interpolate(rate_si, ode_index);
		}
		//
		// initialize sum of effects to zero
		CppAD::vector<Float> effect_ode(n_ode);
		for(size_t k = 0; k < n_ode; k++)
			effect_ode[k] = 0.0;
		//
		// include child random effect
		if( child < n_child_ )
		{	info      = pack_object_.rate_info(rate_id[ell], child);
			smooth_id = info.smooth_id;
			if( smooth_id != DISMOD_AT_NULL_SIZE_T )
			{	n_var     = info.n_var;
				//
				CppAD::vector<Float> var_si(n_var);
				for(size_t k = 0; k < n_var; k++)
					var_si[k] = pack_vec[info.offset + k];
				//
				CppAD::vector<Float> var_ode =
					si2ode_vec_[smooth_id]->interpolate(var_si, ode_index);
				//
				for(size_t k = 0; k < n_ode; k++)
					effect_ode[k] += var_ode[k];
			}
		}
		//
		// include effect of rate covariates
		size_t n_cov = pack_object_.mulcov_rate_value_n_cov(rate_id[ell]);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object_.mulcov_rate_value_info(rate_id[ell], j);
			n_var      = info.n_var;
			smooth_id  = info.smooth_id;
			double x_j = x[ info.covariate_id ];
			//
			CppAD::vector<Float> var_si(n_var);
			for(size_t k = 0; k < n_var; k++)
				var_si[k] = pack_vec[info.offset + k];
			//
			CppAD::vector<Float> var_ode =
				si2ode_vec_[smooth_id]->interpolate(var_si, ode_index);
			//
			for(size_t k = 0; k < n_ode; k++)
				effect_ode[k] += var_ode[k] * x_j;
		}
		//
		// include effect of measurement covariates
		n_cov = pack_object_.mulcov_meas_value_n_cov(integrand_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object_.mulcov_meas_value_info(integrand_id, j);
			n_var      = info.n_var;
			smooth_id  = info.smooth_id;
			double x_j = x[ info.covariate_id ];
			//
			CppAD::vector<Float> var_si(n_var);
			for(size_t k = 0; k < n_var; k++)
				var_si[k] = pack_vec[info.offset + k];
			//
			CppAD::vector<Float> var_ode =
				si2ode_vec_[smooth_id]->interpolate(var_si, ode_index);
			//
			for(size_t k = 0; k < n_ode; k++)
				effect_ode[k] += var_ode[k] * x_j;
		}
		//
		// apply the total effect
		for(size_t k = 0; k < n_ode; k++)
			rate_ode[ell][k] *= exp( effect_ode[k] );
	}
	CppAD::vector<Float> var_ode(n_ode);
	switch(integrand)
	{
		case Sincidence_enum:
		var_ode = rate_ode[0];
		break;

		case remission_enum:
		var_ode =  rate_ode[0];
		break;

		case mtexcess_enum:
		var_ode = rate_ode[0];
		break;

		case mtother_enum:
		var_ode = rate_ode[0];
		break;

		case mtwith_enum:
		// chi = rate_ode[0], omega = rate_ode[1]
		for(size_t k = 0; k < n_ode; k++)
			var_ode[k] = rate_ode[0][k] + rate_ode[1][k];
		break;

		case relrisk_enum:
		// chi = rate_ode[0], omega = rate_ode[1]
		for(size_t k = 0; k < n_ode; k++)
			var_ode[k]  = 1.0 + rate_ode[0][k] / rate_ode[1][k];
		break;

		default:
		assert( false );
	}
	Float avg = Float(0.0);
	for(size_t k = 0; k < n_ode; k++)
		avg += c_ode[k] * var_ode[k];
	//
	return avg;
}
/*
-----------------------------------------------------------------------------
$begin data_model_avg_yes_ode$$

$spell
	avgint
	Tincidence
	mtspecific
	mtall
	mtstandard
	subvectors
	enum
	integrands
	var
	vec
	CppAD
	const
	odegrid
	covariates
$$
$section One Average Integrand That Requires the ODE$$

$head Syntax$$
$icode%avg% = %data_object%.avg_yes_ode(
	%subset_id%, %pack_vec%, %reference_sc%
)%$$


$head data_object$$
This object has prototype
$codei%
	const data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head subset_id$$
This argument has prototype
$codei%
	size_t %subset_id%
%$$
and is the $cref/subset_id/data_subset/data_subset_obj/subset_id/$$
we are computing the average integrand for.

$subhead Integrand$$
The $cref/integrand_id/data_table/integrand_id/$$ corresponding to this
$cref/subset_id/data_subset/data_subset_obj/subset_id/$$
must be one of the following:
$code susceptible_enum$$,
$code withC_enum$$,
$code prevalence_enum$$,
$code Tincidence_enum$$,
$code mtspecific_enum$$,
$code mtall_enum$$,
$code mtstandard_enum$$.

$head pack_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %pack_vec%
%$$
and is all the $cref model_variables$$ in the order
specified by $cref pack_info$$.
Only the following subvectors of $icode pack_vec$$ are used:
$cref pack_info_rate_info$$,
$cref pack_info_mulcov_rate$$.

$head reference_sc$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %reference_sc%
%$$
If the size of this vector is zero, it is not used.
It's intended use is to avoid repeated solving of the ODE's corresponding
to the reference value for all of the covariates.
Let $icode%n_ode% = n_age_ode_ * n_time_ode_%$$.
If it's size is not zero,be
$codei%2 * (n_child_ + 1) * %n_ode%$$.
In this case,
for $icode%c% = 0, %...%, n_child_%$$,
for $icode%i% = 0, %...%, n_age_ode_-1%$$,
for $icode%j% = 0, %...%, n_time_ode_-1%$$,
$codei%
	%reference_sc%[ 2*(%child% * %n_ode% + %i% * n_time_ode_ + %j%) + 0]
%$$
is the susceptible fraction and
$codei%
	%reference_sc%[ 2*(%child% * %n_ode% + %i% * n_time_ode_ + %j%) + 1]
%$$
is the with condition fraction corresponding to the specified child
$icode c$$ and reference value for the rate covariates.
The child $icode%c% = n_child_%$$ corresponds to the parent.

$head avg$$
The return value has prototype
$codei%
	%Float% avg
%$$
This is the
$cref/average integrand/avg_integrand/Average Integrand, A_i/$$
for the specified data point.

$children%example/devel/model/avg_yes_ode_xam.cpp
%$$
$head Example$$
The file $cref avg_yes_ode_xam.cpp$$ contains an example and test
of using this routine.

$end
*/

template <class Float>
Float data_model::avg_yes_ode(
	size_t                        subset_id    ,
	const CppAD::vector<Float>&   pack_vec     ,
	const CppAD::vector<Float>&   reference_sc ) const
{
# ifndef NDEBUG
	switch( data_info_[subset_id].integrand )
	{
		case Sincidence_enum:
		case remission_enum:
		case mtexcess_enum:
		case mtother_enum:
		case mtwith_enum:
		case relrisk_enum:
		assert(false);
		break;

		default:
		break;
	}
	size_t n_ode = n_age_ode_ * n_time_ode_;
	if( reference_sc.size() != 0 )
		assert( reference_sc.size() == 2 * (n_child_ + 1) * n_ode );
# endif
	assert( pack_object_.size() == pack_vec.size() );

	// covariate information for this data pont
	CppAD::vector<double> x(n_covariate_);
	for(size_t j = 0; j < n_covariate_; j++)
		x[j] = data_cov_value_[subset_id * n_covariate_ + j];

	// data_info information for this data point
	integrand_enum integrand           = data_info_[subset_id].integrand;
	size_t i_min                       = data_info_[subset_id].i_min;
	size_t j_min                       = data_info_[subset_id].j_min;
	size_t n_age_sub                   = data_info_[subset_id].n_age;
	size_t n_time_sub                  = data_info_[subset_id].n_time;
	size_t child                       = data_info_[subset_id].child;
	const CppAD::vector<double>& c_ode = data_info_[subset_id].c_ode;

	// sci_sub
	CppAD::vector<Float> sci_sub = sci_ode(
		integrand,
		i_min,
		j_min,
		n_age_sub,
		n_time_sub,
		child,
		x,
		pack_vec,
		reference_sc
	);

	/* -----------------------------------------------------------------------
	ode_index:
	We call (i_min+i, j_min+j) for i=0,...,n_age_sub-1, j=0,...,n_time_sub
	the ODE subgrid for this measurement. For k = 0, ..., n_ode_sub-1
		ode_index[k] = (i_min + i) * n_time_ode_ + j_min + j
	where each point in the ODE subgrid has been represented.
	------------------------------------------------------------------------ */
	size_t n_ode_sub = n_age_sub * n_time_sub;
	CppAD::vector<size_t> ode_index(n_ode_sub);
	for(size_t i = 0; i < n_age_sub; i++)
	{	for(size_t j = 0; j < n_time_sub; j++)
		{	size_t k = i * n_time_sub + j;
			ode_index[k] = (i_min + i) * n_time_ode_ + j_min + j;
		}
	}
	/* ------------------------------------------------------------------------
	meas_cov_ode:
	for k = 0,...,n_ode_sub-1, meas_cov_ode[k] is the measurement covariate
	effect at the gird point correspoinding to ode_index[k].
	------------------------------------------------------------------------ */
	size_t integrand_id = data_subset_obj_[subset_id].integrand_id;
	CppAD::vector<Float> meas_cov_ode(n_ode_sub);
	for(size_t k = 0; k < n_ode_sub; k++)
		meas_cov_ode[k] = 0.0;
	size_t n_cov = pack_object_.mulcov_meas_value_n_cov(integrand_id);
	pack_info::subvec_info                info;
	for(size_t j = 0; j < n_cov; j++)
	{	info  = pack_object_.mulcov_meas_value_info(integrand_id, j);
		size_t n_var      = info.n_var;
		size_t smooth_id  = info.smooth_id;
		double x_j        = x[ info.covariate_id ];
		//
		CppAD::vector<Float> var_si(n_var);
		for(size_t k = 0; k < n_var; k++)
			var_si[k] = pack_vec[info.offset + k];
		//
		CppAD::vector<Float> var_ode =
			si2ode_vec_[smooth_id]->interpolate(var_si, ode_index);
		//
		for(size_t k = 0; k < n_ode_sub; k++)
			meas_cov_ode[k] += var_ode[k] * x_j;
	}
	/* ------------------------------------------------------------------------
	avg:
	c_ode Weighted average of the integrand times exponential of measurement
	covariate effect
	------------------------------------------------------------------------ */
	// compute the average integrand
	Float avg = Float(0.0);
	for(size_t k = 0; k < n_ode_sub; k++)
	{	assert( ! CppAD::isnan( sci_sub[k * 3 + 2] ) );
		avg += c_ode[k] * sci_sub[k * 3 + 2] * exp( meas_cov_ode[k] );
	}
	//
	return avg;
}
/*
-----------------------------------------------------------------------------
$begin data_model_like_one$$

$spell
	avgint
	Sincidence
	Tincidence
	struct
	logden
	fabs
	logden
	std
	Integrands
	wres
	var
	vec
	const
	CppAD
	subvectors
	mtspecific
	mtall
	mtstandard
	mtexcess
	mtwith
	mtother
	relrisk
	xam
$$
$section One Weighted Residual and Log-Likelihood for any Integrands$$

$head Syntax$$
$icode%residual% = %data_object%.like_one(
	%subset_id%, %pack_vec%, %avg%, %delta_out%
)%$$

$head Requirement$$
One must call $cref/replace_like/data_model_replace_like/$$
before calling this function.

$head Log-likelihood$$
We use $cref/y_i/data_like/Data Table Notation/y_i/$$ to denote the
$cref/meas_value/data_table/meas_value/$$ corresponding
to this $cref/subset_id/data_subset/data_subset_obj/subset_id/$$.
The log-likelihood computed by $code like_one$$ is the mapping
$latex \[
	\ell (u, \theta) = C + \log [ \B{p} ( y_i | u , \theta ) ]
\] $$
where $latex u$$ are the random effects,
$latex \theta$$ are the fixed effects, and
$latex C$$ is a constant that does
not depend on $latex ( u , \theta )$$.

$head data_object$$
This object has prototype
$codei%
	const data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head subset_id$$
This argument has prototype
$codei%
	size_t %subset_id%
%$$
and is the $cref/subset_id/data_subset/data_subset_obj/subset_id/$$
we are computing the weighted residual and log-likelihood for.

$head pack_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %pack_vec%
%$$
and is all the $cref model_variables$$ in the order
specified by $cref pack_info$$; i.e.,
$latex (u , \theta)$$.
Only the $cref pack_info_mulcov_meas$$ subvectors of $icode pack_vec$$ are used
by $code like_one$$ (note that other components of $latex (u, \theta )$$
are used to compute $icode avg$$ documented below).

$head avg$$
This argument has prototype
$codei%
	const %Float%& %avg%
%$$
and is the
$cref/average integrand/avg_integrand/Average Integrand, A_i/$$,
$latex A_i ( u , \theta )$$, for the specified data point.
This can be calculated using:
$table
routine                   $cnext integrand for this $icode subset_id$$
$rnext
$cref data_model_avg_no_ode$$ $cnext
	Sincidence, remission, mtexcess, mtother, mtwith, relrisk
$rnext
$cref data_model_avg_yes_ode$$ $cnext
	susceptible, with, Tincidence, prevalence, mtspecific, mtall, mtstandard

$tend

$head delta_out$$
The input value of $icode delta_out$$ does not matter.
Upon return it is the adjusted standard deviation
$cref/delta/data_like/Adjusted Standard Deviation, delta_i/$$
for this data point.

$head residual$$
The return value has prototype
$codei%
	residual_struct<%Float%> %residual%
%$$
see $cref/residual_struct/residual_density/residual/residual_struct/$$.
It contains the weighted residual and the corresponding log-density.

$subhead index$$
Note that the $cref/index/residual_density/index/$$ field is set to
the value $icode subset_id$$.

$children%example/devel/model/like_one_xam.cpp
%$$
$head Example$$
The file $cref like_one_xam.cpp$$ contains an example and test
of using this routine.

$end
*/
template <class Float>
residual_struct<Float> data_model::like_one(
	size_t                        subset_id ,
	const CppAD::vector<Float>&   pack_vec  ,
	const Float&                  avg       ,
	Float&                        delta_out ) const
{
	assert( pack_object_.size() == pack_vec.size() );
	assert( replace_like_called_ );

	// covariate information for this data point
	CppAD::vector<double> x(n_covariate_);
	for(size_t j = 0; j < n_covariate_; j++)
		x[j] = data_cov_value_[subset_id * n_covariate_ + j];
	double eta           = data_subset_obj_[subset_id].eta;
	double nu            = data_subset_obj_[subset_id].nu;
	size_t integrand_id  = data_subset_obj_[subset_id].integrand_id;
	double meas_value    = data_subset_obj_[subset_id].meas_value;
	double meas_std      = data_subset_obj_[subset_id].meas_std;
	double meas_cv       = minimum_meas_cv_[integrand_id];
	//
	assert( meas_std  > 0.0 );
	double Delta = std::max(meas_std, meas_cv * std::fabs(meas_value) );

	// data_info information for this data point
	density_enum   density             = data_info_[subset_id].density;
	size_t i_min                       = data_info_[subset_id].i_min;
	size_t j_min                       = data_info_[subset_id].j_min;
	size_t n_age                       = data_info_[subset_id].n_age;
	size_t n_time                      = data_info_[subset_id].n_time;
	const CppAD::vector<double>& c_ode = data_info_[subset_id].c_ode;

	// ode subgrid that we covariates at
	size_t n_ode = n_age * n_time;
	CppAD::vector<size_t> ode_index(n_ode);
	for(size_t i = 0; i < n_age; i++)
	{	for(size_t j = 0; j < n_time; j++)
		{	size_t k = i * n_time + j;
			ode_index[k] = (i_min + i) * n_time_ode_ + j_min + j;
		}
	}

	// measurement std covariates effect on the ode subgrid
	CppAD::vector<Float> meas_cov_ode(n_ode);
	for(size_t k = 0; k < n_ode; k++)
		meas_cov_ode[k] = 0.0;
	size_t n_cov = pack_object_.mulcov_meas_std_n_cov(integrand_id);
	pack_info::subvec_info  info;
	for(size_t j = 0; j < n_cov; j++)
	{	info              = pack_object_.mulcov_meas_std_info(integrand_id, j);
		size_t n_var      = info.n_var;
		size_t smooth_id  = info.smooth_id;
		double x_j = x[ info.covariate_id ];
		//
		CppAD::vector<Float> var_si(n_var);
		for(size_t k = 0; k < n_var; k++)
			var_si[k] = pack_vec[info.offset + k];
		//
		CppAD::vector<Float> var_ode =
			si2ode_vec_[smooth_id]->interpolate(var_si, ode_index);
		//
		for(size_t k = 0; k < n_ode; k++)
			meas_cov_ode[k] += var_ode[k] * x_j;
	}
	Float std_effect = Float(0.0);
	for(size_t k = 0; k < n_ode; k++)
		std_effect += c_ode[k] * meas_cov_ode[k];
	//
	// Compute the adusted standard deviation
	if( density == log_gaussian_enum
	||  density == log_laplace_enum
	||  density == log_students_enum
	)
		delta_out  = Delta * (1.0  + std_effect);
	else
		delta_out  = Delta + std_effect;
	//
	Float not_used;
	bool difference = false;
	struct residual_struct<Float> residual = residual_density(
		not_used,
		avg,
		Float(meas_value),
		delta_out,
		density,
		Float(eta),
		Float(nu),
		subset_id,
		difference
	);
	//
	// switch the sign of the residual because it is computed as
	// model minus data (to avoid having model scale the variance)
	residual.wres = - residual.wres;
	//
	return residual;
}
/*
-------------------------------------------------------------------------------
$begin data_model_like_all$$
$spell
	avgint
	bool
	enum
	vec
	CppAD
	const
	struct
$$

$section All the Weighted Residuals and Log-Likelihoods$$

$head Syntax$$
$icode%residual_vec% = %data_object%.like_all(
	%hold_out%, %random_depend%, %pack_vec%
)%$$

$head Requirement$$
One must call $cref/replace_like/data_model_replace_like/$$
before calling this function.

$head data_object$$
This object has prototype
$codei%
	const data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head hold_out$$
This argument has prototype
$codei%
	bool %hold_out%
%$$
If it is true, the flagged data will be held out,
otherwise it will not.

$head random_depend$$
This argument has prototype $codei%
	bool %random_depend%
%$$
If it is true (false), only residuals that depend on the random effects
that have non-equal (equal) bounds, are included.

$head pack_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %pack_vec%
%$$
and is all the $cref model_variables$$ in the order
specified by $cref pack_info$$; i.e., $latex (u , \theta)$$.
If $icode random_depend$$ is false,
only the components of $icode u$$ that have equal lower and upper
bounds are defined (the other components are unspecified).

$head residual_vec$$
The return value has prototype
$codei%
	CppAD::vector< residual_struct<%Float%> > %residual_vec%
%$$

$subhead index$$
For each element of $icode residual_vec$$,
the residual $cref/index/residual_density/index/$$
is set to the $cref/data_subset_id/data_subset_table/data_subset_id/$$
for the corresponding residual.

$subhead Include Hold Outs$$
If $icode hold_out$$ is false,
the size of $icode residual$$ is equal the number of subset data values
$cref/n_subset/data_subset/data_subset_obj/n_subset/$$.
The order of the residuals is the same as in
$cref/subset_object/data_model_ctor/subset_object/$$ in the
$icode data_object$$ constructor.

$subhead No Hold Outs$$
If $icode hold_out$$ is true,
the size of $icode residual$$ is equal the number of subset data values
$cref/n_subset/data_subset/data_subset_obj/n_subset/$$,
minus the number for which
$cref/hold_out/data_table/hold_out/$$ is one.
The order of the residuals is not specified in this case.

$head Log Density$$
The log of the density
$latex \B{p}( y | u , \theta )$$,
for the data that is included,
is the sum of the log of the densities corresponding to all the
$cref/residuals/residual_density/$$ in $icode residual_vec$$.

$end
*/
template <class Float>
CppAD::vector< residual_struct<Float> > data_model::like_all(
	bool                        hold_out      ,
	bool                        random_depend ,
	const CppAD::vector<Float>& pack_vec      ) const
{	assert( replace_like_called_ );
	//
	bool parent_only = ! random_depend;
	CppAD::vector<Float> reference_sc = reference_ode(pack_vec, parent_only);
	//
	// loop over the subsampled data
	CppAD::vector< residual_struct<Float> > residual_vec;
	for(size_t subset_id = 0; subset_id < data_subset_obj_.size(); subset_id++)
	{	bool keep = hold_out == false;
		keep     |= data_subset_obj_[subset_id].hold_out == 0;
		if( random_depend )
			keep &= data_info_[subset_id].bound_ran_neq == true;
		else
			keep &= data_info_[subset_id].bound_ran_neq == false;
		assert( data_info_[subset_id].child <= n_child_ );
		if( keep )
		{	// compute avgerage of integrand for this data
			Float avg;
			integrand_enum integrand  = data_info_[subset_id].integrand;
			switch( integrand )
			{	case Sincidence_enum:
				case remission_enum:
				case mtexcess_enum:
				case mtother_enum:
				case mtwith_enum:
				case relrisk_enum:
				avg = avg_no_ode(subset_id, pack_vec);
				break;

				case susceptible_enum:
				case withC_enum:
				case prevalence_enum:
				case Tincidence_enum:
				case mtspecific_enum:
				case mtall_enum:
				case mtstandard_enum:
				avg = avg_yes_ode(subset_id, pack_vec, reference_sc);
				break;

				default:
				assert(false);
			}
			// compute its residual and log likelihood
			Float not_used;
			residual_struct<Float> residual =
				like_one(subset_id, pack_vec, avg, not_used);
			residual_vec.push_back( residual );
		}
	}
	return residual_vec;
}

// ------------------------------------------------------------------------
# define DISMOD_AT_INSTANTIATE_DATA_MODEL_CTOR(SubsetStruct)   \
template data_model::data_model(                                \
	double                                   bound_random    ,  \
	size_t                                   n_covariate     ,  \
	size_t                                   n_age_ode       ,  \
	size_t                                   n_time_ode      ,  \
	double                                   ode_step_size   ,  \
	const CppAD::vector<double>&             age_table       ,  \
	const CppAD::vector<double>&             time_table      ,  \
	const CppAD::vector<integrand_struct>&   integrand_table ,  \
	const CppAD::vector<prior_struct>&       prior_table     ,  \
	const CppAD::vector<SubsetStruct>&       subset_object   ,  \
	const CppAD::vector<double>&             subset_cov_value,  \
	const CppAD::vector<weight_info>&        w_info_vec      ,  \
	const CppAD::vector<smooth_info>&        s_info_vec      ,  \
	const pack_info&                         pack_object     ,  \
	const child_info&                        child_object       \
);
DISMOD_AT_INSTANTIATE_DATA_MODEL_CTOR(data_subset_struct)
DISMOD_AT_INSTANTIATE_DATA_MODEL_CTOR(avgint_subset_struct)
// ------------------------------------------------------------------------
# define DISMOD_AT_INSTANTIATE_DATA_MODEL(Float)            \
	template CppAD::vector<Float>                           \
	data_model::sci_ode(                                    \
		integrand_enum                integrand    ,        \
		size_t                        i_min        ,        \
		size_t                        j_min        ,        \
		size_t                        n_age_sub    ,        \
		size_t                        n_time_sub   ,        \
		size_t                        child        ,        \
		const CppAD::vector<double>&  x            ,        \
		const CppAD::vector<Float>&   pack_vec     ,        \
		const CppAD::vector<Float>&   reference_sc          \
	) const;                                                \
	template Float data_model::avg_no_ode(                  \
		size_t                        subset_id,            \
		const CppAD::vector<Float>&   pack_vec              \
	) const;                                                \
	template Float data_model::avg_yes_ode(                 \
		size_t                        subset_id        ,    \
		const CppAD::vector<Float>&   pack_vec         ,    \
		const CppAD::vector<Float>&   reference_sc          \
	) const;                                                \
	template residual_struct<Float>                         \
	data_model::like_one(                                   \
		size_t                        subset_id,            \
		const CppAD::vector<Float>&   pack_vec ,            \
		const Float&                  avg      ,            \
		Float&                        delta_out             \
	) const;                                                \
	template CppAD::vector< residual_struct<Float> >        \
	data_model::like_all(                                   \
		bool                          hold_out ,            \
		bool                          parent   ,            \
		const CppAD::vector<Float>&   pack_vec              \
	) const;                                                \

// instantiations
DISMOD_AT_INSTANTIATE_DATA_MODEL( double )
DISMOD_AT_INSTANTIATE_DATA_MODEL( a1_double )


} // END DISMOD_AT_NAMESPACE
