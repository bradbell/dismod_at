// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
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
$$

$section Data Model: Constructor$$

$head Syntax$$
$codei%data_model %data_object%(
	%parent_node_id%,
	%n_covariate%,
	%n_age_ode%,
	%n_time_ode%,
	%ode_step_size%,
	%age_table%,
	%time_table%,
	%integrand_table%,
	%node_table%,
	%subset_object%,
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

$head parent_node_id$$
This argument has prototype
$codei%
	size_t %parent_node_id%
%$$
and is the
$cref/parent_node_id/option_table/parent_node_id/$$.
in the fit command.

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
$cref/ode age grid/glossary/Ode Grid/Age, a_i/$$.

$head n_time_ode$$
This argument has prototype
$codei%
	size_t %n_time_ode%
%$$
It is the number of points in the
$cref/ode time grid/glossary/Ode Grid/Time, t_j/$$.

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

$head node_table$$
This argument has prototype
$codei%
	const CppAD::vector<node_struct>& %node_table%
%$$
and is the $cref/node_table/get_node_table/node_table/$$.
Only the $code parent$$ field of this table is used.

$head subset_object$$
This argument has prototype
$codei%
	const CppAD::vector<data_subset_struct>&  %subset_object%
%$$
and is the sub-sampled version of the data table; see
$cref/data_subset_obj/data_subset/data_subset_obj/$$.


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
the $cref/model_variables/model_variable/$$.

$head child_object$$
This argument has prototype
$codei%
	const child_info& %child_object%
%$$
and is the $cref child_info$$ information corresponding to
parent node, node table, and data table.

$end
-----------------------------------------------------------------------------
*/
# include <dismod_at/data_model.hpp>
# include <dismod_at/integrate_1d.hpp>
# include <dismod_at/integrate_2d.hpp>
# include <dismod_at/interp_weight.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/solve_ode.hpp>
# include <dismod_at/residual_density.hpp>
# include <dismod_at/a2_double.hpp>
# include <dismod_at/avgint_subset.hpp>

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
	size_t                                   parent_node_id  ,
	size_t                                   n_covariate     ,
	size_t                                   n_age_ode       ,
	size_t                                   n_time_ode      ,
	double                                   ode_step_size   ,
	const CppAD::vector<double>&             age_table       ,
	const CppAD::vector<double>&             time_table      ,
	const CppAD::vector<integrand_struct>&   integrand_table ,
	const CppAD::vector<node_struct>&        node_table      ,
	const CppAD::vector<SubsetStruct>&       subset_object   ,
	const CppAD::vector<weight_info>&        w_info_vec      ,
	const CppAD::vector<smooth_info>&        s_info_vec      ,
	const pack_info&                         pack_object     ,
	const child_info&                        child_object    )
:
n_covariate_   (n_covariate)      ,
n_age_ode_     (n_age_ode)        ,
n_time_ode_    (n_time_ode)       ,
ode_step_size_ (ode_step_size)    ,
pack_object_   (pack_object)
{	using std::string;
	size_t i, j, k;
	//
	// only set the fileds that are common to data_subset and avgint_subset
	size_t n_subset = subset_object.size();
	data_subset_obj_.resize(n_subset);
	data_cov_value_.resize(n_subset * n_covariate);
	for(i = 0; i < n_subset; i++)
	{	data_subset_obj_[i].original_id  = subset_object[i].original_id;
		data_subset_obj_[i].integrand_id = subset_object[i].integrand_id;
		data_subset_obj_[i].node_id      = subset_object[i].node_id;
		data_subset_obj_[i].weight_id    = subset_object[i].weight_id;
		data_subset_obj_[i].age_lower    = subset_object[i].age_lower;
		data_subset_obj_[i].age_upper    = subset_object[i].age_upper;
		data_subset_obj_[i].time_lower   = subset_object[i].time_lower;
		data_subset_obj_[i].time_upper   = subset_object[i].time_upper;
		for(j = 0; j < n_covariate_; j++)
			data_cov_value_[i * n_covariate_ + j] = subset_object[i].x[j];
	}
	//
	double eps = std::numeric_limits<double>::epsilon() * 100.0;
	//
	assert( n_age_ode  > 1 );
	assert( n_time_ode > 1 );
	//
	// set default value for eigen_ode2_case_number_
	eigen_ode2_case_number_ = 4;
	//
	// initialize
	replace_like_called_ = false;
	//
	// set n_child_
	n_child_ = child_object.child_size();
	assert( n_child_ == pack_object.child_size() );
	//
	// si2ode_vec_ has same size as s_info_vec
	size_t n_smooth = s_info_vec.size();
	si2ode_vec_.resize( n_smooth );
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	si2ode_vec_[smooth_id] = new smooth2ode(
			n_age_ode, n_time_ode, ode_step_size, age_table, time_table,
			s_info_vec[smooth_id]
		);
	}
	//
	// data_ode_info_ has same size as data_subset_obj
	data_info_.resize( n_subset );
	//
	// limits of the ode grid
	double age_min    = age_table[0];
	double age_max    = age_table[ age_table.size() - 1 ];
	double time_min   = time_table[0];
	double time_max   = time_table[ time_table.size() - 1 ];
	//
	assert( age_max  <= age_min  + n_age_ode * ode_step_size );
	assert( time_max <= time_min + n_time_ode * ode_step_size );
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
		while(age_min + (i_min+1) * ode_step_size <= age_lower )
			i_min++;
		i_min = std::min(i_min, n_age_ode - 2);

		// determine number of ode age grid points
		size_t n_age = 2;
		while( age_min + (i_min + n_age - 1) * ode_step_size < age_upper )
			n_age++;
		assert( i_min + n_age <= n_age_ode );

		// determine minimum ode grid time index
		size_t j_min = 0;
		while(time_min + (j_min+1) * ode_step_size <= time_lower )
			j_min++;
		j_min  = std::min(j_min, n_age_ode - 2);

		// determine number of ode time grid point
		size_t n_time = 2;
		while( time_min + (j_min + n_time - 1) * ode_step_size < time_upper )
			n_time++;
		assert( j_min + n_time <= n_time_ode );

		// initialize coefficient sum for each ode grid point within limits
		CppAD::vector<double> c_sum(n_age * n_time);
		for(k = 0; k < n_age * n_time; k++)
			c_sum[k] = 0.0;

		// weighting for this data point
		size_t weight_id = subset_object[subset_id].weight_id;
		const weight_info& w_info( w_info_vec[weight_id] );

		// indices used to interpolate weighting
		size_t i_wi = 0;
		size_t j_wi = 0;

		// loop over all the ode rectangles that are within limits
		CppAD::vector<double> w(4), c(4);
		std::pair<double, double> w_pair, c_pair;
		for(i = 0; i < n_age-1; i++)
		{	// age ode grid points
			double a1 = age_min + (i_min + i) * ode_step_size;
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
			for(j = 0; j < n_time-1; j++)
			{	// time ode grid points
				double t1 = time_min + (j_min + j) * ode_step_size;
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
		for(k = 0; k < n_age * n_time; k++)
			sum += c_sum[k];
		assert( sum > 0.0 );

		// integrand and eta
		size_t  integrand_id     = subset_object[subset_id].integrand_id;
		integrand_enum integrand = integrand_table[integrand_id].integrand;
		double eta               = integrand_table[integrand_id].eta;

		// child of parent node that this data is associated with
		size_t  child            = child_object.table_id2child(original_id);

		// set the information for this data point
		data_info_[subset_id].integrand = integrand;
		data_info_[subset_id].child     = child;
		data_info_[subset_id].i_min     = i_min;
		data_info_[subset_id].j_min     = j_min;
		data_info_[subset_id].n_age     = n_age;
		data_info_[subset_id].n_time    = n_time;
		data_info_[subset_id].eta       = eta;
		//
		data_info_[subset_id].c_ode.resize(n_age * n_time);
		for(k = 0; k < n_age * n_time; k++)
			data_info_[subset_id].c_ode[k] = c_sum[k] / sum;
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
$icode%data_object%.set_eigen_ode2_case_number(%rate_info%)%$$

$head data_object$$
This object has prototype
$codei%
	data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.

$head rate_info$$
Is the $cref/option_value/option_table/option_value/$$
corresponding to $icode%option_name% = rate_info%$$ in the option table.
$end
*/
void data_model::set_eigen_ode2_case_number(const std::string& rate_info)
{
	if( rate_info == "iota_zero_rho_zero" )
	{	eigen_ode2_case_number_ = 1;
		return;
	}
	if( rate_info == "iota_pos_rho_zero" )
	{	eigen_ode2_case_number_ = 2;
		return;
	}
	if( rate_info == "iota_zero_rho_pos" )
	{	eigen_ode2_case_number_ = 3;
		return;
	}
	if( rate_info == "iota_pos_rho_pos" )
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

$head subset_object$$
We use $icode subset_object$$
for the $cref/subset_object/data_model_ctor/$$ in the $icode data_object$$
constructor.

$head n_subset$$
We use the notation
$codei%
	%n_subset% = %subset_object%.size()
%$$

$head subset_id$$
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

$end
*/
void data_model::replace_like(
		const CppAD::vector<data_subset_struct>&  data_subset_obj )
{	size_t n_subset = data_subset_obj_.size();
	assert( data_subset_obj.size() == n_subset );
	//
	for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
	{	data_subset_obj_[subset_id].density_id =
			data_subset_obj[subset_id].density_id;
		data_subset_obj_[subset_id].hold_out =
			data_subset_obj[subset_id].hold_out;
		data_subset_obj_[subset_id].meas_value =
			data_subset_obj[subset_id].meas_value;
		data_subset_obj_[subset_id].meas_std =
			data_subset_obj[subset_id].meas_std;
		//
		data_info_[subset_id].density = density_enum(
			data_subset_obj[subset_id].density_id
		);
	}
	replace_like_called_ = true;
	return;
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
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, or $cref a2_double$$.

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
$cref pack_info_rate$$ and $cref pack_info_mulcov_rate$$ subvectors of
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
and is all the $cref/model variables/model_variable/$$ in the order
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
{	size_t i, j, k, ell;
	assert( pack_object_.size() == pack_vec.size() );

	// covariate infomation for this data point
	CppAD::vector<double> x(n_covariate_);
	for(j = 0; j < n_covariate_; j++)
			x[j] = data_cov_value_[subset_id * n_covariate_ + j];

	// data_info information for this data point
	integrand_enum integrand           = data_info_[subset_id].integrand;
	size_t i_min                       = data_info_[subset_id].i_min;
	size_t j_min                       = data_info_[subset_id].j_min;
	size_t n_age                       = data_info_[subset_id].n_age;
	size_t n_time                      = data_info_[subset_id].n_time;
	size_t child                       = data_info_[subset_id].child;
	const CppAD::vector<double>& c_ode = data_info_[subset_id].c_ode;

	// check that this data's node is a descendent of the parent node
	assert( child <= n_child_ );

	// ode subgrid that we need integrand at
	size_t n_ode = n_age * n_time;
	CppAD::vector<size_t> ode_index(n_ode);
	for(i = 0; i < n_age; i++)
	{	for(j = 0; j < n_time; j++)
		{	k = i * n_time + j;
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
	for(ell = 0; ell < 2; ell++) if( rate_id[ell] < number_rate_enum )
	{	rate_ode[ell].resize(n_ode);
		//
		// extract subvector information for the parent rate
		pack_info::subvec_info info;
		info             = pack_object_.rate_info(rate_id[ell], n_child_);
		size_t n_var     = info.n_var;
		size_t smooth_id = info.smooth_id;
		//
		CppAD::vector<Float> rate_si(n_var);
		for(k = 0; k < n_var; k++)
			rate_si[k] = pack_vec[info.offset + k];
		//
		// interpolate onto the ode grid
		rate_ode[ell] =
			si2ode_vec_[smooth_id]->interpolate(rate_si, ode_index);
		//
		// initialize sum of effects to zero
		CppAD::vector<Float> effect_ode(n_ode);
		for(k = 0; k < n_ode; k++)
			effect_ode[k] = 0.0;
		//
		// include child random effect
		if( child < n_child_ )
		{	info      = pack_object_.rate_info(rate_id[ell], child);
			n_var     = info.n_var;
			smooth_id = info.smooth_id;
			//
			CppAD::vector<Float> var_si(n_var);
			for(k = 0; k < n_var; k++)
				var_si[k] = pack_vec[info.offset + k];
			//
			CppAD::vector<Float> var_ode =
				si2ode_vec_[smooth_id]->interpolate(var_si, ode_index);
			//
			for(k = 0; k < n_ode; k++)
				effect_ode[k] += var_ode[k];
		}
		// include effect of rate covariates
		size_t n_cov = pack_object_.mulcov_rate_value_n_cov(rate_id[ell]);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object_.mulcov_rate_value_info(rate_id[ell], j);
			n_var      = info.n_var;
			smooth_id  = info.smooth_id;
			double x_j = x[ info.covariate_id ];
			//
			CppAD::vector<Float> var_si(n_var);
			for(k = 0; k < n_var; k++)
				var_si[k] = pack_vec[info.offset + k];
			//
			CppAD::vector<Float> var_ode =
				si2ode_vec_[smooth_id]->interpolate(var_si, ode_index);
			//
			for(k = 0; k < n_ode; k++)
				effect_ode[k] += var_ode[k] * x_j;
		}
		for(k = 0; k < n_ode; k++)
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
		for(k = 0; k < n_ode; k++)
			var_ode[k] = rate_ode[0][k] + rate_ode[0][k];
		break;

		case relrisk_enum:
		for(k = 0; k < n_ode; k++)
		{	var_ode[k]  = 1.0;
			var_ode[k] += rate_ode[0][k] / rate_ode[0][k];
		}
		break;

		default:
		assert( false );
	}
	Float sum = Float(0.0);
	for(k = 0; k < n_ode; k++)
		sum += c_ode[k] * var_ode[k];
	//
	return sum;
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
$$
$section One Average Integrand That Requires the ODE$$

$head Syntax$$
$icode%avg% = %data_object%.avg_yes_ode(%subset_id%, %pack_vec%)%$$


$head data_object$$
This object has prototype
$codei%
	const data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, or $cref a2_double$$.

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
and is all the $cref/model variables/model_variable/$$ in the order
specified by $cref pack_info$$.
Only the following subvectors of $icode pack_vec$$ are used:
$cref pack_info_rate$$,
$cref pack_info_mulcov_rate$$.

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
	size_t                        subset_id ,
	const CppAD::vector<Float>&   pack_vec  ) const
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
# endif
	size_t i, j, k, ell;
	assert( pack_object_.size() == pack_vec.size() );

	// covariate information for this data pont
	CppAD::vector<double> x(n_covariate_);
	for(j = 0; j < n_covariate_; j++)
		x[j] = data_cov_value_[subset_id * n_covariate_ + j];

	// data_info infomation for this data point
	integrand_enum integrand           = data_info_[subset_id].integrand;
	size_t i_min                       = data_info_[subset_id].i_min;
	size_t j_min                       = data_info_[subset_id].j_min;
	size_t n_age_sub                   = data_info_[subset_id].n_age;
	size_t n_time_sub                  = data_info_[subset_id].n_time;
	size_t child                       = data_info_[subset_id].child;
	const CppAD::vector<double>& c_ode = data_info_[subset_id].c_ode;

	// check that this data's node is a descendent of the parent node
	assert( child <= n_child_ );

	CppAD::vector<size_t> ode_index, cohort_start;
	// cohorts that end at maximum age index
	for(j = 0; j < n_time_sub; j++)
	{	size_t i_end, j_end, ik, jk;
		i_end = i_min + n_age_sub - 1; // cohort ends at this age index
		j_end = j_min + j;             // cohort ends at this time index
		assert( i_end < n_age_ode_ );
		assert( j_end < n_time_ode_ );
		k     = ode_index.size();
		cohort_start.push_back(k);
		for(ik = 0; ik <= i_end; ik++)
		{	if( i_end - ik < j_end )
				jk = j_end - (i_end - ik);
			else
				jk = 0;
			ode_index.push_back( ik * n_time_ode_ + jk);
		}
	}
	// cohorts that end at maximum time index
	// (except for the one that also ends at maximum age)
	for(i = 0; i < n_age_sub - 1; i++)
	{	size_t i_end, j_end, ik, jk;
		j_end = j_min + n_time_sub - 1;
		i_end = i_min + i;
		assert( i_end < n_age_ode_ );
		assert( j_end < n_time_ode_ );
		k     = ode_index.size();
		cohort_start.push_back(k);
		// note that this loop is empty when i = 0
		for(ik = 0; ik <= i_end; ik++)
		{	if( i_end - ik < j_end )
				jk = j_end - (i_end - ik);
			else
				jk = 0;
			ode_index.push_back( ik * n_time_ode_ + jk);
		}

	}
	size_t n_index = ode_index.size();

	// value for the rates on the ode subgrid
	CppAD::vector< CppAD::vector<Float> > rate_ode(number_rate_enum);
	pack_info::subvec_info                 info;
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	rate_ode[rate_id].resize(n_index);
		//
		// extract subvector information for the parent rate
		info             = pack_object_.rate_info(rate_id, n_child_);
		size_t n_var     = info.n_var;
		size_t smooth_id = info.smooth_id;
		//
		CppAD::vector<Float> rate_si(n_var);
		for(k = 0; k < n_var; k++)
			rate_si[k] = pack_vec[info.offset + k];
		//
		// interpolate onto the ode grid
		rate_ode[rate_id] =
			si2ode_vec_[smooth_id]->interpolate(rate_si, ode_index);
		//
		// initialize sum of effects to zero
		CppAD::vector<Float> effect_ode(n_index);
		for(k = 0; k < n_index; k++)
			effect_ode[k] = 0.0;
		//
		// include child random effect
		if( child < n_child_ )
		{	info      = pack_object_.rate_info(rate_id, child);
			n_var     = info.n_var;
			smooth_id = info.smooth_id;
			//
			CppAD::vector<Float> var_si(n_var);
			for(k = 0; k < n_var; k++)
				var_si[k] = pack_vec[info.offset + k];
			//
			CppAD::vector<Float> var_ode =
				si2ode_vec_[smooth_id]->interpolate(var_si, ode_index);
			//
			for(k = 0; k < n_index; k++)
				effect_ode[k] += var_ode[k];
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
			CppAD::vector<Float> var_si(n_var);
			for(k = 0; k < n_var; k++)
				var_si[k] = pack_vec[info.offset + k];
			//
			CppAD::vector<Float> var_ode =
				si2ode_vec_[smooth_id]->interpolate(var_si, ode_index);
			//
			for(k = 0; k < n_index; k++)
				effect_ode[k] += var_ode[k] * x_j;
		}
		for(k = 0; k < n_index; k++)
			rate_ode[rate_id][k] *= exp( effect_ode[k] );
	}

	// loop over the cohorts
	size_t n_cohort = cohort_start.size();
	size_t n_ode_sub = n_age_sub * n_time_sub;
	CppAD::vector<Float> iota, rho, chi, omega, S_out, C_out;
	CppAD::vector<Float> integrand_sub( n_ode_sub );
	Float zero = Float(0.0);
	for(k = 0; k < n_ode_sub; k++)
			integrand_sub[k] = CppAD::nan(zero);
	for(ell = 0; ell < n_cohort; ell++)
	{	size_t k_start, nk;
		k_start  = cohort_start[ell];
		if( ell+1 < n_cohort )
			nk = cohort_start[ell+1] - cohort_start[ell] ;
		else
			nk = ode_index.size() - cohort_start[ell];
		iota.resize(nk);
		rho.resize(nk);
		chi.resize(nk);
		omega.resize(nk);
		S_out.resize(0);
		C_out.resize(0);
		Float pini   = Float(rate_ode[pini_enum][k_start]);
		for(k = 0; k < nk; k++)
		{	iota[k]  = rate_ode[iota_enum][k_start + k];
			rho[k]   = rate_ode[rho_enum][k_start + k];
			chi[k]   = rate_ode[chi_enum][k_start + k];
			omega[k] = rate_ode[omega_enum][k_start + k];
		}
		size_t i_max     = ode_index[k_start + nk - 1] / n_time_ode_;
		size_t j_max     = ode_index[k_start + nk - 1] % n_time_ode_;
		Float  step_size = Float(ode_step_size_);
		//
		solve_ode(
			eigen_ode2_case_number_ ,
			i_max, j_max, step_size, pini, iota, rho, chi, omega, S_out, C_out
		);
		//
		// compute integrand on subgrid
		for(k = 0; k < nk; k++)
		{	size_t i = ode_index[k_start + k] / n_time_ode_;
			size_t j = ode_index[k_start + k] % n_time_ode_;
			if( i_min <= i && j_min <= j )
			{	Float P   = C_out[k] / ( S_out[k]  + C_out[k]);
				size_t ij = (i - i_min) * n_time_sub + (j - j_min);
				switch(integrand)
				{
					case prevalence_enum:
					integrand_sub[ij] = P;
					break;

					case Tincidence_enum:
					integrand_sub[ij] = iota[k] * (1.0 - P);
					break;

					case mtspecific_enum:
					integrand_sub[ij] = chi[k] * P;
					break;

					case mtall_enum:
					integrand_sub[ij] = omega[k] + chi[k] * P;
					break;

					case mtstandard_enum:
					integrand_sub[ij] = omega[k] + chi[k];
					integrand_sub[ij] /= omega[k] + chi[k] * P;
					break;

					default:
					assert(false);
				}
			}
		}
	}

	Float sum = Float(0.0);
	for(k = 0; k < n_ode_sub; k++)
	{	assert( ! CppAD::isnan( integrand_sub[k] ) );
		sum += c_ode[k] * integrand_sub[k];
	}
	//
	return sum;
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
$icode%residual% = %data_object%.like_one(%subset_id%, %pack_vec%, %avg%)%$$

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
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, or $cref a2_double$$.

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
and is all the $cref/model variables/model_variable/$$ in the order
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
	Tincidence, prevalence, mtspecific, mtall, mtstandard

$tend

$head residual$$
The return value has prototype
$codei%
	residual_struct<%Float%> %residual%
%$$
see $cref/residual_struct/residual_density/residual_struct/$$.
It contains the weighted residual and the corresponding log-density.

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
	const Float&                  avg       ) const
{	size_t i, j, k;
	assert( pack_object_.size() == pack_vec.size() );
	assert( replace_like_called_ );

	// covariate infomation for this data point
	CppAD::vector<double> x(n_covariate_);
	for(j = 0; j < n_covariate_; j++)
		x[j] = data_cov_value_[subset_id * n_covariate_ + j];
	double sigma                   = data_subset_obj_[subset_id].meas_std;
	size_t integrand_id            = data_subset_obj_[subset_id].integrand_id;
	double meas_value              = data_subset_obj_[subset_id].meas_value;
	assert( sigma > 0.0 );

	// data_info information for this data point
	density_enum   density             = data_info_[subset_id].density;
	double eta                         = data_info_[subset_id].eta;
	size_t i_min                       = data_info_[subset_id].i_min;
	size_t j_min                       = data_info_[subset_id].j_min;
	size_t n_age                       = data_info_[subset_id].n_age;
	size_t n_time                      = data_info_[subset_id].n_time;
	const CppAD::vector<double>& c_ode = data_info_[subset_id].c_ode;

	// ode subgrid that we covariates at
	size_t n_ode = n_age * n_time;
	CppAD::vector<size_t> ode_index(n_ode);
	for(i = 0; i < n_age; i++)
	{	for(j = 0; j < n_time; j++)
		{	k = i * n_time + j;
			ode_index[k] = (i_min + i) * n_time_ode_ + j_min + j;
		}
	}

	// measurement mean covariates effect on the ode subgrid
	CppAD::vector<Float> meas_cov_ode(n_ode);
	for(k = 0; k < n_ode; k++)
		meas_cov_ode[k] = 0.0;
	size_t n_cov = pack_object_.mulcov_meas_value_n_cov(integrand_id);
	//
	pack_info::subvec_info  info;
	for(size_t j = 0; j < n_cov; j++)
	{	info              = pack_object_.mulcov_meas_value_info(integrand_id, j);
		size_t n_var      = info.n_var;
		size_t smooth_id  = info.smooth_id;
		double x_j = x[ info.covariate_id ];
		//
		CppAD::vector<Float> var_si(n_var);
		for(k = 0; k < n_var; k++)
			var_si[k] = pack_vec[info.offset + k];
		//
		CppAD::vector<Float> var_ode =
			si2ode_vec_[smooth_id]->interpolate(var_si, ode_index);
		//
		for(k = 0; k < n_ode; k++)
			meas_cov_ode[k] += var_ode[k] * x_j;
	}
	Float mean_effect = Float(0.0);
	for(k = 0; k < n_ode; k++)
		mean_effect += c_ode[k] * exp( - meas_cov_ode[k] );
	Float adjust  = Float(mean_effect * meas_value);

	// measurement std covariates effect on the ode subgrid
	for(k = 0; k < n_ode; k++)
		meas_cov_ode[k] = 0.0;
	n_cov = pack_object_.mulcov_meas_std_n_cov(integrand_id);
	for(size_t j = 0; j < n_cov; j++)
	{	info              = pack_object_.mulcov_meas_std_info(integrand_id, j);
		size_t n_var      = info.n_var;
		size_t smooth_id  = info.smooth_id;
		double x_j = x[ info.covariate_id ];
		//
		CppAD::vector<Float> var_si(n_var);
		for(k = 0; k < n_var; k++)
			var_si[k] = pack_vec[info.offset + k];
		//
		CppAD::vector<Float> var_ode =
			si2ode_vec_[smooth_id]->interpolate(var_si, ode_index);
		//
		for(k = 0; k < n_ode; k++)
			meas_cov_ode[k] += var_ode[k] * x_j;
	}
	Float std_effect = Float(0.0);
	for(k = 0; k < n_ode; k++)
		std_effect += c_ode[k] * meas_cov_ode[k];
	Float delta  = Float(mean_effect * sigma);
	delta       += std_effect * (adjust + eta);
	//
	return residual_density( density, adjust, avg, delta, Float(eta) );
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
	%hold_out%, %parent%, %pack_vec%
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
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, or $cref a2_double$$.

$head hold_out$$
This argument has prototype
$codei%
	bool %hold_out%
%$$
If it is true, the flagged data will be held out,
otherwise it will not.

$head parent$$
This argument has prototype
$codei%
	bool %parent%
%$$
If it is true, all the
$cref/parent/option_table/parent_node_id/$$ data is returned.
If it is false, all the
$cref/child/node_table/parent/Child Group/$$ data is returned
(including any level of descendant).

$head pack_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %pack_vec%
%$$
and is all the $cref/model variables/model_variable/$$ in the order
specified by $cref pack_info$$; i.e.,
$latex (u , \theta)$$.

$head residual_vec$$
The return value has prototype
$codei%
	CppAD::vector< residual_struct<%Float%> > %residual_vec%
%$$

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
	bool                        hold_out ,
	bool                        parent   ,
	const CppAD::vector<Float>& pack_vec ) const
{	assert( replace_like_called_ );
	CppAD::vector< residual_struct<Float> > residual_vec;

	// loop over the subsampled data
	for(size_t subset_id = 0; subset_id < data_subset_obj_.size(); subset_id++)
	{	bool keep = hold_out == false;
		keep     |= data_subset_obj_[subset_id].hold_out == 0;
		if( parent )
			keep &= data_info_[subset_id].child == n_child_;
		else
			keep &= data_info_[subset_id].child != n_child_;
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

				case prevalence_enum:
				case Tincidence_enum:
				case mtspecific_enum:
				case mtall_enum:
				case mtstandard_enum:
				avg = avg_yes_ode(subset_id, pack_vec);
				break;

				default:
				assert(false);
			}
			// compute its residual and log likelihood
			residual_struct<Float> residual =
				like_one(subset_id, pack_vec, avg);
			residual_vec.push_back( residual );
		}
	}
	return residual_vec;
}

// ------------------------------------------------------------------------
# define DISMOD_AT_INSTANTIATE_DATA_MODEL_CTOR(SubsetStruct)   \
template data_model::data_model(                                \
	size_t                                   parent_node_id  ,  \
	size_t                                   n_covariate     ,  \
	size_t                                   n_age_ode       ,  \
	size_t                                   n_time_ode      ,  \
	double                                   ode_step_size   ,  \
	const CppAD::vector<double>&             age_table       ,  \
	const CppAD::vector<double>&             time_table      ,  \
	const CppAD::vector<integrand_struct>&   integrand_table ,  \
	const CppAD::vector<node_struct>&        node_table      ,  \
	const CppAD::vector<SubsetStruct>&       subset_object   ,  \
	const CppAD::vector<weight_info>&        w_info_vec      ,  \
	const CppAD::vector<smooth_info>&        s_info_vec      ,  \
	const pack_info&                         pack_object     ,  \
	const child_info&                        child_object       \
);
DISMOD_AT_INSTANTIATE_DATA_MODEL_CTOR(data_subset_struct)
DISMOD_AT_INSTANTIATE_DATA_MODEL_CTOR(avgint_subset_struct)
// ------------------------------------------------------------------------
# define DISMOD_AT_INSTANTIATE_DATA_MODEL(Float)            \
	template Float data_model::avg_no_ode(                  \
		size_t                        subset_id,            \
		const CppAD::vector<Float>&   pack_vec              \
	) const;                                                \
	template Float data_model::avg_yes_ode(                 \
		size_t                        subset_id,            \
		const CppAD::vector<Float>&   pack_vec              \
	) const;                                                \
	template residual_struct<Float>                         \
	data_model::like_one(                                   \
		size_t                        subset_id,            \
		const CppAD::vector<Float>&   pack_vec ,            \
		const Float&                  avg                   \
	) const;                                                \
	template CppAD::vector< residual_struct<Float> >        \
	data_model::like_all(                                   \
		bool                          hold_out ,            \
		bool                          parent   ,            \
		const CppAD::vector<Float>&   pack_vec              \
	) const;                                                \

// instantiations
DISMOD_AT_INSTANTIATE_DATA_MODEL(double)
DISMOD_AT_INSTANTIATE_DATA_MODEL( CppAD::AD<double> )
DISMOD_AT_INSTANTIATE_DATA_MODEL( a2_double )


} // END DISMOD_AT_NAMESPACE
