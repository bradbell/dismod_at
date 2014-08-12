// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin data_mean_ctor$$
$spell
	vec
	const
	enum
	struct
	Cpp
$$

$section Data Mean: Constructor$$

$head Syntax$$
$codei%data_mean %avg_integrand%(
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
	%s_info_vec
)%$$

$head Notation$$
We use $icode a_min$$ for the minimum age in $cref age_table$$
and $icode t_min$$ for the minimum time in $cref time_table$$.

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code CppAD::AD<double>$$

$head parent_node_id$$
This argument has prototype
$codei%
	size_t %parent_node_id%
%$$
and is the run table
$cref/parent_node_id/run_table/parent_node_id/$$.

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
and is the value of $cref/ode_step_size/run_table/ode_step_size/$$
in the run table.

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
	const CppAD::vector<integrand_enum>&  %integrand_table%
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
Only the following field of each $code data_struct$$ are used
by this routine:
$code integrand_id$$,
$code node_id$$,
$code weight_id$$,
$code age_lower$$,
$code age_upper$$,
$code time_lower$$, and
$code time_upper$$.

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

$end
-----------------------------------------------------------------------------
*/
# include <dismod_at/include/data_mean.hpp>
# include <dismod_at/include/integrate_1d.hpp>
# include <dismod_at/include/integrate_2d.hpp>
# include <dismod_at/include/interp_weight.hpp>
# include <dismod_at/include/table_error_exit.hpp>
# include <dismod_at/include/child_data.hpp>
# include <dismod_at/include/get_rate_table.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// destructor
data_mean::~data_mean(void)
{	// must delete the smooth2ode objects pointed to by si2ode_vec_
	size_t n_smooth = si2ode_vec_.size();
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
			delete si2ode_vec_[smooth_id];
}

// consctructor
data_mean::data_mean(
	size_t                               parent_node_id  ,
	size_t                               n_age_ode       ,
	size_t                               n_time_ode      ,
	double                               ode_step_size   ,
	const CppAD::vector<double>&         age_table       ,
	const CppAD::vector<double>&         time_table      ,
	const CppAD::vector<integrand_enum>& integrand_table ,
	const CppAD::vector<node_struct>&    node_table      ,
	const CppAD::vector<data_struct>&    data_table      ,
	const CppAD::vector<weight_info>&    w_info_vec      ,
	const CppAD::vector<smooth_info>&    s_info_vec      )
:
n_age_ode_     (n_age_ode)        ,
n_time_ode_    (n_time_ode)       ,
ode_step_size_ (ode_step_size)    ,
data_table_    (data_table)
{	using std::string;
	size_t i, j, k;
	//
	double eps = std::numeric_limits<double>::epsilon() * 100.0;
	//
	assert( n_age_ode  > 1 );
	assert( n_time_ode > 1 );
	//
	// set n_child_
	child_data cd(parent_node_id, node_table, data_table);
	n_child_ = cd.child_size();
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

		// integrand
		size_t  integrand_id     = data_table[data_id].integrand_id;
		integrand_enum integrand = integrand_table[integrand_id];

		// child of parent node that this data is associated with
		size_t  child            = cd.data_id2child(data_id);

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
		while(age_min + i_min * ode_step_size < age_lower )
			i_min++;
		i_min = std::min(i_min, n_age_ode - 2);

		// determine number of ode age grid points
		size_t n_age = 0;
		while( age_min + (i_min + n_age) * ode_step_size < age_upper )
			n_age++;
		n_age = std::max(n_age, size_t(2));
		assert( i_min + n_age <= n_age_ode );

		// determine minimum ode grid time index 
		size_t j_min = 0;
		while(time_min + j_min * ode_step_size < time_lower )
			j_min++;
		j_min  = std::min(j_min, n_age_ode - 2);

		// determine number of ode time grid point
		size_t n_time = 0;
		while( time_min + (j_min + n_time) * ode_step_size < time_upper )
			n_time++;
		n_time = std::max(n_time, size_t(2));
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
				std::pair<double, double> t_pair(t1, t2);
				std::pair<double, double> s_pair(s1, s2);
				//
				if( b1_equal_b2 && s1_equal_s2 )
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
				else if( b1_equal_b2 )
				{	// case where only integrate w.r.t time
					double a = age_lower;
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

					// translate to coefficients for sourrounding age points
					c[0]   = c_pair.first  * (a2 - a) / d; // (a1, t1)
					c[1]   = c_pair.second * (a2 - a) / d; // (a1, t2)
					c[2]   = c_pair.first  * (a - a1) / d; // (a2, t1)
					c[3]   = c_pair.second * (a - a1) / d; // (a2, t2)
				}
				else if( s1_equal_s2 )
				{	// case where only integrate w.r.t. age
					double t = time_lower;
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

					// translate to coefficient for sourrounding time poins
					c[0]   = c_pair.first  * (t2 - t) / d; // (a1, t1)
					c[1]   = c_pair.first  * (t - t1) / d; // (a1, t2)
					c[2]   = c_pair.second * (t2 - t) / d; // (a2, t1)
					c[3]   = c_pair.second * (t - t1) / d; // (a2, t2)
				}
				else 
				{	// case where integrate w.r.t to age and time

					// weight at (a1, t1)
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

		// set the information for this data point
		data_info_[data_id].integrand = integrand;
		data_info_[data_id].child     = child;
		data_info_[data_id].i_min     = i_min;
		data_info_[data_id].j_min     = j_min;
		data_info_[data_id].n_age     = n_age; 
		data_info_[data_id].n_time    = n_time; 
		//
		data_info_[data_id].c_ode.resize(n_age * n_time);
		for(k = 0; k < n_age * n_time; k++)
			data_info_[data_id].c_ode[k] = c_sum[k] / sum;
	}
}
/*
-----------------------------------------------------------------------------
$begin data_mean_no_ode$$

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
$$
$section Data Mean for Integrands That Don't Require the ODE$$

$head Syntax$$
$icode%avg% = %avg_integrand%.no_ode(%data_id%, %var_info%, %var_vec%)%$$


$head avg_integrand$$
This object has prototype
$codei%
	const data_mean %avg_integrand%
%$$

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code CppD::AD<double>$$.

$head data_id$$
This argument has prototype
$codei%
	size_t %data_id%
%$$
and is the $cref/data_id/data_table/data_id/$$ for we are computing
the data mean for.

$subhead Node$$
The $icode data_id$$ must correspond to a 
$cref/node_id/data_table/node_id/$$ that is a descendant of the
$cref/parent_node_id/data_mean_ctor/parent_node_id/$$; i.e.,
the function $code data_id2child$$ returns a
$cref/child/child_data/data_id2child/child/$$ value
less than or equal 
$cref/n_child/child_data/child_size/n_child/$$. 

$head var_info$$
This argument has prototype
$codei%
	const pack_var& %var_info%
%$$
and is the $cref pack_var$$ information corresponding to 
$icode var_vec$$.

$head var_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %var_vec%
%$$
and is a vector of values for all of the model variables.

$subhead Integrand and Rates$$
The $cref/integrand_id/data_table/integrand_id/$$ corresponding to this
$icode data_id$$ must be one of those listed in the table below.
In addition, depending on the integrand, only the corresponding 
$cref pack_var_rate$$ and $cref pack_var_rate_mulcov$$ subvectors of 
$icode var_vec$$ are used:
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
This is the 
$cref/average integrand/model_data_mean/Average Integrand/$$ 
for the specified data point.
$end
*/

template <class Float>
Float data_mean::no_ode(
		size_t                        data_id  ,
		const pack_var&               var_info ,
		const CppAD::vector<Float>&   var_vec
	) const
{	size_t i, j, k, ell;
	assert( var_info.size() == var_vec.size() );

	// data table infomation for this data point
	integrand_enum integrand       = data_info_[ data_id].integrand;
	const CppAD::vector<double>& x = data_table_[ data_id ] .x; 

	// exrtra information for this data point
	size_t i_min                       = data_info_[data_id ].i_min;
	size_t j_min                       = data_info_[data_id ].j_min;
	size_t n_age                       = data_info_[data_id ].n_age;
	size_t n_time                      = data_info_[data_id ].n_time;
	size_t child                       = data_info_[data_id ].child;
	const CppAD::vector<double>& c_ode = data_info_[data_id ].c_ode;

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
		rate_id[0] = size_t (mtother_enum);
		break;

		case mtwith_enum:
		case relrisk_enum:
		rate_id[0] = size_t (chi_enum);
		rate_id[1] = size_t (mtother_enum);
		break;

		default:
		// use compute_yes_ode for these cases
		assert( false );
	}


	// value for the rate on the ode subgrid
	CppAD::vector< CppAD::vector<Float> > rate_ode(2);
	for(ell = 0; ell < 2; ell++) if( rate_id[ell] < number_rate_enum )
	{	rate_ode[ell].resize(n_ode);
		//
		// extract subvector infromation for this rate
		pack_var::subvec_info info;
		info             = var_info.rate_info(rate_id[ell], child);	
		size_t n_var     = info.n_var;
		size_t smooth_id = info.smooth_id;
		//
		CppAD::vector<Float> rate_si(n_var);
		if( child < n_child_ || n_child_ == 0 )
		{	// rate is for a child or only the parent rate is being estimated
			for(k = 0; k < n_var; k++)
				rate_si[k] = var_vec[info.offset + k]; 
		}
		else
		{	// rate is for parent and there are multiple children
			assert( n_child_ > 1 );
			assert( child  == n_child_ );
			for(k = 0; k < n_var; k++)
				rate_si[k] = Float(0);
			for(size_t child_id = 0; child_id < n_child_; child_id++)
			{	info = var_info.rate_info(rate_id[ell],child_id);	
				for(k = 0; k < n_var; k++)
					rate_si[k] += var_vec[info.offset + k];
			}
			double den = double(n_child_);
			for(k = 0; k < n_var; k++)
				rate_si[k] /= den;
		}
		//
		// interpolate onto the ode grid
		rate_ode[ell] = 
			si2ode_vec_[smooth_id]->interpolate(rate_si, ode_index);
		//
		// include effect of rate covariates
		size_t n_cov = var_info.rate_mean_mulcov_n_cov(rate_id[ell]);
		for(size_t j = 0; j < n_cov; j++)
		{	info       = var_info.rate_mean_mulcov_info(rate_id[ell], j);
			n_var      = info.n_var;
			smooth_id  = info.smooth_id;
			double x_j = x[ info.covariate_id ];
			//
			CppAD::vector<Float> var_si(n_var);
			for(k = 0; k < n_var; k++)
				var_si[k] += var_vec[info.offset + k];
			//
			CppAD::vector<Float> var_ode = 
				si2ode_vec_[smooth_id]->interpolate(var_si, ode_index);
			//
			for(k = 0; k < n_ode; k++)
				rate_ode[ell][k] *= exp( var_ode[k] * x_j );
		}
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
	Float sum = 0.0;
	for(k = 0; k < n_ode; k++)
		sum += c_ode[k] * var_ode[k];
	//
	return sum;
}

# define DISMOD_AT_INSTANTIATE_DATA_MEAN_NO_ODE(Float)      \
	template Float data_mean::no_ode(                       \
		size_t                        data_id  ,            \
		const pack_var&               var_info ,            \
		const CppAD::vector<Float>&   var_vec               \
	) const;

// instantiations
DISMOD_AT_INSTANTIATE_DATA_MEAN_NO_ODE(double)
DISMOD_AT_INSTANTIATE_DATA_MEAN_NO_ODE( CppAD::AD<double> )



} // END DISMOD_AT_NAMESPACE
