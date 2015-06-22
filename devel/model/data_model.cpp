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
	logden
	std
	vec
	const
	enum
	struct
	Cpp
$$

$section Data Model: Constructor$$

$head Syntax$$
$codei%data_model %data_object%(
	%parent_node_id%,
	%n_age_ode%,
	%n_time_ode%,
	%ode_step_size%,
	%age_table%,
	%time_table%,
	%integrand_table%,
	%node_table%,
	%data_table%,
	%w_info_vec%,
	%s_info_vec%,
	%pack_object%
)%$$

$head Reference$$
The $icode data_table$$ is not copied,
instead a reference to is stored in $icode data_object$$.
Thus, $icode data_object$$ should not be used after $icode data_table$$
is deleted.

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
and is the fit table
$cref/parent_node_id/fit_table/parent_node_id/$$.

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
and is the value of $cref/ode_step_size/fit_table/ode_step_size/$$
in the fit table.

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

$head data_table$$
This argument has prototype
$codei%
	const CppAD::vector<data_struct>&  %data_table%
%$$
and is the $cref/data_table/get_data_table/data_table/$$.

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
and is the $cref devel_pack_info$$ information corresponding to
the $cref/model_variables/model_variable/$$.

$end
-----------------------------------------------------------------------------
*/
# include <dismod_at/data_model.hpp>
# include <dismod_at/integrate_1d.hpp>
# include <dismod_at/integrate_2d.hpp>
# include <dismod_at/interp_weight.hpp>
# include <dismod_at/table_error_exit.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/solve_ode.hpp>
# include <dismod_at/residual_density.hpp>
# include <dismod_at/a5_double.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// destructor
data_model::~data_model(void)
{	// must delete the smooth2ode objects pointed to by si2ode_vec_
	size_t n_smooth = si2ode_vec_.size();
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
			delete si2ode_vec_[smooth_id];
}

// consctructor
data_model::data_model(
	size_t                                   parent_node_id  ,
	size_t                                   n_age_ode       ,
	size_t                                   n_time_ode      ,
	double                                   ode_step_size   ,
	const CppAD::vector<double>&             age_table       ,
	const CppAD::vector<double>&             time_table      ,
	const CppAD::vector<integrand_struct>&   integrand_table ,
	const CppAD::vector<node_struct>&        node_table      ,
	const CppAD::vector<data_struct>&        data_table      ,
	const CppAD::vector<data_subset_struct>& data_sample     ,
	const CppAD::vector<weight_info>&        w_info_vec      ,
	const CppAD::vector<smooth_info>&        s_info_vec      ,
	const pack_info&                         pack_object     )
:
n_age_ode_     (n_age_ode)        ,
n_time_ode_    (n_time_ode)       ,
ode_step_size_ (ode_step_size)    ,
data_table_    (data_table)       ,
data_sample_   (data_sample)      ,
pack_object_   (pack_object)
{	using std::string;
	size_t i, j, k;
	//
	double eps = std::numeric_limits<double>::epsilon() * 100.0;
	//
	assert( n_age_ode  > 1 );
	assert( n_time_ode > 1 );
	//
	// set n_child_
	child_info child_object(parent_node_id, node_table, data_table);
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
	// data_ode_info_ has same size as data_table
	size_t n_data = data_table.size();
	data_info_.resize( n_data );
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
	for(size_t data_id = 0; data_id < n_data; data_id++)
	{	// information for this data point

		// age limits
		double age_lower  = data_table[data_id].age_lower;
		if( age_lower < age_min )
		{	string msg = "age_lower is less than minimum age in age table";
			table_error_exit("data", data_id, msg);
		}
		double age_upper  = data_table[data_id].age_upper;
		if( age_upper < age_lower )
		{	string msg = "age_upper is less than age_lower";
			table_error_exit("data", data_id, msg);
		}
		if( age_max < age_upper )
		{	string msg = "age_upper is greater than maximum age in age table";
			table_error_exit("data", data_id, msg);
		}

		// time limits
		double time_lower  = data_table[data_id].time_lower;
		if( time_lower < time_min )
		{	string msg = "time_lower is less than minimum time in time table";
			table_error_exit("data", data_id, msg);
		}
		double time_upper  = data_table[data_id].time_upper;
		if( time_upper < time_lower )
		{	string msg = "time_upper is less than time_lower";
			table_error_exit("data", data_id, msg);
		}
		if( time_max < time_upper )
		{	string msg =
				"time_upper is greater than maximum time in time table";
			table_error_exit("data", data_id, msg);
		}

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
		size_t weight_id = data_table[data_id].weight_id;
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
		size_t  integrand_id     = data_table[data_id].integrand_id;
		integrand_enum integrand = integrand_table[integrand_id].integrand;
		double eta               = integrand_table[integrand_id].eta;

		// child of parent node that this data is associated with
		size_t  child            = child_object.data_id2child(data_id);

		// density for this data point
		size_t density_id    = data_table[data_id].density_id;
		density_enum density = density_enum(density_id);

		// set the information for this data point
		data_info_[data_id].integrand = integrand;
		data_info_[data_id].density   = density;
		data_info_[data_id].child     = child;
		data_info_[data_id].i_min     = i_min;
		data_info_[data_id].j_min     = j_min;
		data_info_[data_id].n_age     = n_age;
		data_info_[data_id].n_time    = n_time;
		data_info_[data_id].eta       = eta;
		//
		data_info_[data_id].c_ode.resize(n_age * n_time);
		for(k = 0; k < n_age * n_time; k++)
			data_info_[data_id].c_ode[k] = c_sum[k] / sum;
	}
}
/*
-----------------------------------------------------------------------------
$begin data_model_avg_no_ode$$

$spell
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
$icode%avg% = %data_object%.avg_no_ode(%sample_id%, %pack_vec%)%$$

$head data_object$$
This object has prototype
$codei%
	const data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.


$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, or $cref a5_double$$.

$head sample_id$$
This argument has prototype
$codei%
	size_t %sample_id%
%$$
and is the $cref/sample_id/data_subset/data_sample/sample_id/$$
we are computing the average integrand for.

$head pack_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %pack_vec%
%$$
and is all the $cref/model variables/model_variable/$$ in the order
specified by $cref devel_pack_info$$.

$subhead Integrand and Rates$$
The $cref/integrand_id/data_table/integrand_id/$$ corresponding to this
$icode data_id$$ must be one of those listed in the table below.
In addition, depending on the integrand, only the corresponding
$cref devel_pack_info_rate$$ and $cref devel_pack_info_rate_mulcov$$ subvectors of
$icode pack_vec$$ are used:
$table
Integrand               $cnext Rates               $rnext
$code incidence_enum$$  $cnext $code iota_enum$$   $rnext
$code remission_enum$$  $cnext $code rho_enum$$    $rnext
$code mtexcess_enum$$   $cnext $code chi_enum$$    $rnext
$code mtother_enum$$    $cnext $code omega_enum$$  $rnext
$code mtwith_enum$$     $cnext $code chi_enum$$, $code omega_enum$$ $rnext
$code relrisk_enum$$    $cnext $code chi_enum$$, $code omega_enum$$
$tend

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
	size_t                        sample_id ,
	const CppAD::vector<Float>&   pack_vec  ) const
{	size_t i, j, k, ell;
	assert( pack_object_.size() == pack_vec.size() );

	// data table infomation for this data point
	const CppAD::vector<double>& x = data_sample_[ sample_id ].x;

	// data_info information for this data point
	size_t data_id = data_sample_[sample_id].data_id;
	integrand_enum integrand           = data_info_[ data_id].integrand;
	size_t i_min                       = data_info_[data_id ].i_min;
	size_t j_min                       = data_info_[data_id ].j_min;
	size_t n_age                       = data_info_[data_id ].n_age;
	size_t n_time                      = data_info_[data_id ].n_time;
	size_t child                       = data_info_[data_id ].child;
	const CppAD::vector<double>& c_ode = data_info_[data_id ].c_ode;

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
		case incidence_enum:
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
		size_t n_cov = pack_object_.rate_mean_mulcov_n_cov(rate_id[ell]);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object_.rate_mean_mulcov_info(rate_id[ell], j);
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
		case incidence_enum:
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
$icode%avg% = %data_object%.avg_yes_ode(%data_id%, %pack_vec%)%$$


$head data_object$$
This object has prototype
$codei%
	const data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, or $cref a5_double$$.

$head data_id$$
This argument has prototype
$codei%
	size_t %data_id%
%$$
and is the $cref/data_id/data_table/data_id/$$ for we are computing
the average integrand for.

$subhead Node$$
The $icode data_id$$ must correspond to a
$cref/node_id/data_table/node_id/$$ that is a descendant of the
$cref/parent_node_id/data_model_ctor/parent_node_id/$$; i.e.,
the function $code data_id2child$$ returns a
$cref/child/child_info/data_id2child/child/$$ value
less than or equal
$cref/n_child/child_info/child_size/n_child/$$.

$subhead Integrand$$
The $cref/integrand_id/data_table/integrand_id/$$ corresponding to this
$icode data_id$$ must be one of the following:
$code prevalence_enum$$,
$code mtspecific_enum$$,
$code mtall_enum$$,
$code mtstandard_enum$$.

$head pack_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %pack_vec%
%$$
and is all the $cref/model variables/model_variable/$$ in the order
specified by $cref devel_pack_info$$.
Only the following subvectors of $icode pack_vec$$ are used:
$cref devel_pack_info_rate$$,
$cref devel_pack_info_rate_mulcov$$.

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
	size_t                        data_id  ,
	const CppAD::vector<Float>&   pack_vec ) const
{
# ifndef NDEBUG
	switch( data_info_[data_id].integrand )
	{
		case incidence_enum:
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

	// data table information for this data pont
	const CppAD::vector<double>& x     = data_table_[ data_id ].x;

	// data_info infomation for this data point
	integrand_enum integrand           = data_info_[ data_id].integrand;
	size_t i_min                       = data_info_[data_id ].i_min;
	size_t j_min                       = data_info_[data_id ].j_min;
	size_t n_age_sub                   = data_info_[data_id ].n_age;
	size_t n_time_sub                  = data_info_[data_id ].n_time;
	size_t child                       = data_info_[data_id ].child;
	const CppAD::vector<double>& c_ode = data_info_[data_id ].c_ode;

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
		size_t n_cov = pack_object_.rate_mean_mulcov_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = pack_object_.rate_mean_mulcov_info(rate_id, j);
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
$icode%residual% = %data_object%.like_one(%data_id%, %pack_vec%, %avg%)%$$

$head Log-likelihood$$
We use $cref/y_i/data_like/Data Table Notation/y_i/$$ to denote the
$cref/meas_value/data_table/meas_value/$$ corresponding
to this $cref/data_id/data_table/data_id/$$.
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
$code double$$, $code AD<double>$$, or $cref a5_double$$.

$head data_id$$
This argument has prototype
$codei%
	size_t %data_id%
%$$
and is the $cref/data_id/data_table/data_id/$$ for we are computing
the weighted residual and log-likelihood for.

$subhead Node$$
The $icode data_id$$ must correspond to a
$cref/node_id/data_table/node_id/$$ that is a descendant of the
$cref/parent_node_id/data_model_ctor/parent_node_id/$$; i.e.,
the function $code data_id2child$$ returns a
$cref/child/child_info/data_id2child/child/$$ value
less than or equal
$cref/n_child/child_info/child_size/n_child/$$.

$head pack_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %pack_vec%
%$$
and is all the $cref/model variables/model_variable/$$ in the order
specified by $cref devel_pack_info$$; i.e.,
$latex (u , \theta)$$.
Only the $cref devel_pack_info_meas_mulcov$$ subvectors of $icode pack_vec$$ are used
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
routine                   $cnext integrand for this $icode data_id$$
$rnext
$cref data_model_avg_no_ode$$ $cnext
	incidence, remission, mtexcess, mtother, mtwith, relrisk
$rnext
$cref data_model_avg_yes_ode$$ $cnext
	prevalence, mtspecific, mtall, mtstandard

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
	size_t                        data_id  ,
	const CppAD::vector<Float>&   pack_vec ,
	const Float&                  avg      ) const
{	size_t i, j, k;
	assert( pack_object_.size() == pack_vec.size() );

	// data table infomation for this data point
	const CppAD::vector<double>& x = data_table_[ data_id ].x;
	double sigma                   = data_table_[ data_id ].meas_std;
	size_t integrand_id            = data_table_[ data_id ].integrand_id;
	double meas_value              = data_table_[ data_id ].meas_value;
	assert( sigma > 0.0 );

	// data_info information for this data point
	density_enum   density             = data_info_[data_id].density;
	double eta                         = data_info_[data_id].eta;
	size_t i_min                       = data_info_[data_id].i_min;
	size_t j_min                       = data_info_[data_id].j_min;
	size_t n_age                       = data_info_[data_id].n_age;
	size_t n_time                      = data_info_[data_id].n_time;
	const CppAD::vector<double>& c_ode = data_info_[data_id].c_ode;

	// ode subgrid that we covariates at
	size_t n_ode = n_age * n_time;
	CppAD::vector<size_t> ode_index(n_ode);
	for(i = 0; i < n_age; i++)
	{	for(j = 0; j < n_time; j++)
		{	k = i * n_time + j;
			ode_index[k] = (i_min + i) * n_time_ode_ + j_min + j;
		}
	}

	// measurement mean covaraites effect on the ode subgrid
	CppAD::vector<Float> meas_cov_ode(n_ode);
	for(k = 0; k < n_ode; k++)
		meas_cov_ode[k] = 0.0;
	size_t n_cov = pack_object_.meas_mean_mulcov_n_cov(integrand_id);
	//
	pack_info::subvec_info  info;
	for(size_t j = 0; j < n_cov; j++)
	{	info              = pack_object_.meas_mean_mulcov_info(integrand_id, j);
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

	// measurement std covaraites effect on the ode subgrid
	for(k = 0; k < n_ode; k++)
		meas_cov_ode[k] = 0.0;
	n_cov = pack_object_.meas_std_mulcov_n_cov(integrand_id);
	for(size_t j = 0; j < n_cov; j++)
	{	info              = pack_object_.meas_std_mulcov_info(integrand_id, j);
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
	enum
	vec
	CppAD
	const
	struct
$$

$section All the Weighted Residuals and Log-Likelihoods$$

$head Syntax$$
$icode%residual_vec% = %data_object%.like_all(%pack_vec%)%$$

$head Data Table Notation$$

$head data_object$$
This object has prototype
$codei%
	const data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.

$subhead i$$
We use $icode i$$ to
denote the $cref/data_id/data_table/data_id/$$
for a row in the data table.

$subhead y_i$$
We use $icode y_i$$ to denote the corresponding
$cref/meas_value/data_table/meas_value/$$.

$subhead n_i$$
We use $icode n_i$$ to denote the corresponding
$cref/node_id/data_table/node_id/$$.

$subhead I$$
We use $icode I$$ to denote the set of $icode data_id$$
such that $icode n_i$$ is equal to
$cref/parent node/fit_table/parent_node_id/$$,
or is a descendent of the parent node; i.e.,
$codei%
	data_id2child(%data_id%) <= %n_child%
%$$
see $cref/data_id2child/child_info/data_id2child/$$.


$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code AD<double>$$, or $cref a5_double$$.

$head pack_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %pack_vec%
%$$
and is all the $cref/model variables/model_variable/$$ in the order
specified by $cref devel_pack_info$$; i.e.,
$latex (u , \theta)$$.

$head residual_vec$$
The return value has prototype
$codei%
	CppAD::vector< residual_struct<%Float%> > %residual_vec%
%$$
The size of $icode residual$$ is equal the number of elements in $icode I$$.
The order of the residuals is unspecified (at this time).
The log of the density
$latex \B{p}( y | u , \theta )$$,
is the sum of the log of the densities corresponding to all the
$cref/residuals/residual_density/$$ in $icode residual_vec$$.

$end
*/
template <class Float>
CppAD::vector< residual_struct<Float> > data_model::like_all(
	const CppAD::vector<Float>& pack_vec
) const
{	CppAD::vector< residual_struct<Float> > residual_vec;

	// loop over the subsampled data
	for(size_t sample_id = 0; sample_id < data_sample_.size(); sample_id++)
	{	size_t data_id = data_sample_[sample_id].data_id;

		// compute avgerage of integrand for this data
		Float avg;
		integrand_enum integrand  = data_info_[data_id].integrand;
		switch( integrand )
		{	case incidence_enum:
			case remission_enum:
			case mtexcess_enum:
			case mtother_enum:
			case mtwith_enum:
			case relrisk_enum:
			avg = avg_no_ode(sample_id, pack_vec);
			break;

			case prevalence_enum:
			case mtspecific_enum:
			case mtall_enum:
			case mtstandard_enum:
			avg = avg_yes_ode(data_id, pack_vec);
			break;

			default:
			assert(false);
		}
		// compute its residual and log likelihood
		residual_struct<Float> residual = like_one(data_id, pack_vec, avg);
		residual_vec.push_back( residual );
	}
	return residual_vec;
}

// ------------------------------------------------------------------------
# define DISMOD_AT_INSTANTIATE_DATA_MODEL(Float)            \
	template Float data_model::avg_no_ode(                  \
		size_t                        data_id  ,            \
		const CppAD::vector<Float>&   pack_vec              \
	) const;                                                \
	template Float data_model::avg_yes_ode(                 \
		size_t                        data_id  ,            \
		const CppAD::vector<Float>&   pack_vec              \
	) const;                                                \
	template residual_struct<Float>                         \
	data_model::like_one(                                   \
		size_t                        data_id  ,            \
		const CppAD::vector<Float>&   pack_vec ,            \
		const Float&                  avg                   \
	) const;                                                \
	template CppAD::vector< residual_struct<Float> >        \
	data_model::like_all(                                   \
		const CppAD::vector<Float>&   pack_vec              \
	) const;                                                \

// instantiations
DISMOD_AT_INSTANTIATE_DATA_MODEL(double)
DISMOD_AT_INSTANTIATE_DATA_MODEL( CppAD::AD<double> )
DISMOD_AT_INSTANTIATE_DATA_MODEL( a5_double )


} // END DISMOD_AT_NAMESPACE
