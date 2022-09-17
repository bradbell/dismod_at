// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# include <cppad/mixed/exception.hpp>
# include <dismod_at/avg_noise_effect.hpp>
# include <dismod_at/grid2line.hpp>
# include <dismod_at/null_int.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

/*
------------------------------------------------------------------------------
$begin avg_noise_effect_ctor$$
$spell
	std
	avg_noise_obj
	vec
	const
	CppAD
	Integrands
$$

$section Constructing An Average Noise Effect Object$$

$head Syntax$$
$codei%avg_noise_effect %avg_noise_obj%(
	%ode_step_size%,
	%age_avg_grid%,
	%age_table%,
	%time_table%,
	%subgroup_table%,
	%integrand_table%,
	%w_info_vec%,
	%s_info_vec%,
	%pack_object%
);
%$$

$head Prototype$$
$srcthisfile%
	0%// BEGIN_AVG_NOISE_EFFECT_PROTOTYPE%// END_AVG_NOISE_EFFECT_PROTOTYPE%1
%$$

$head ode_step_size$$
This is the value of
$cref/
	ode_step_size/
	option_table/
	Age Average Grid/
	ode_step_size
/$$ in the option table.

$head age_avg_grid$$
This is the $cref age_avg_grid$$.
A reference to $icode age_avg_grid$$ is used by $icode avg_noise_obj$$
($icode age_avg_grid$$ must no be deleted for as long as
$icode avg_noise_obj$$ is used).

$head age_table$$
This argument is the $cref age_table$$.
A reference to $icode age_table$$ is used by $icode avg_noise_obj$$
($icode age_table$$ must no be deleted for as long as
$icode avg_noise_obj$$ is used).

$head time_table$$
This argument is the $cref time_table$$.
A reference to $icode time_table$$ is used by $icode avg_noise_obj$$.

$head subgroup_table$$
This argument is the $cref subgroup_table$$.
A reference to $icode subgroup_table$$ is used by $icode avg_noise_obj$$.

$head integrand_table$$
This argument is the $cref integrand_table$$.
A reference to $icode integrand_table$$ is used by $icode avg_noise_obj$$.

$head w_info_vec$$
For each $cref/weight_id/weight_table/weight_id/$$,
$codei%
	%w_info_vec%[ %weight_id% ]
%$$
is the corresponding $cref weight_info$$ information.
In addition, the constant weight is included at the end of the vector; i.e.,
at index $icode%w_info_vec%.size()-1%$$.

$head s_info_vec$$
For each $cref/smooth_id/smooth_table/smooth_id/$$,
$codei%
	%s_info_vec%[ %smooth_id% ]
%$$
is the corresponding $cref smooth_info$$ information.
None of the prior information in $icode s_info_vec$$ is used.
A reference to $icode s_info_vec$$ is used by $icode avg_noise_obj$$.

$head pack_object$$
This is the $cref pack_info$$ information corresponding to
the $cref model_variables$$.

$head Example$$
The file $cref avg_noise_effect_xam.cpp$$ contains an example and test
of using this routine.

$end
*/
// BEGIN_AVG_NOISE_EFFECT_PROTOTYPE
avg_noise_effect::avg_noise_effect(
		double                                    ode_step_size    ,
		const CppAD::vector<double>&              age_avg_grid     ,
		const CppAD::vector<double>&              age_table        ,
		const CppAD::vector<double>&              time_table       ,
		const CppAD::vector<subgroup_struct>&     subgroup_table   ,
		const CppAD::vector<integrand_struct>&    integrand_table  ,
		const CppAD::vector<weight_info>&         w_info_vec       ,
		const CppAD::vector<smooth_info>&         s_info_vec       ,
		const pack_info&                          pack_object      )
// END_AVG_NOISE_EFFECT_PROTOTYPE
:
ode_step_size_             ( ode_step_size )   ,
age_table_                 ( age_table )       ,
time_table_                ( time_table )      ,
subgroup_table_            ( subgroup_table )  ,
integrand_table_           ( integrand_table ) ,
w_info_vec_                ( w_info_vec )      ,
s_info_vec_                ( s_info_vec )      ,
pack_object_               ( pack_object )     ,
double_time_line_object_   ( age_avg_grid )    ,
a1_double_time_line_object_( age_avg_grid )
{ }
/*
------------------------------------------------------------------------------
$begin avg_noise_effect_rectangle$$
$spell
	avg_noise_obj
	vec
	const
	CppAD
	Integrands
	covariates
$$

$section Computing One Average Integrand$$

$head Syntax$$
$icode%avg% = %avg_noise_obj%.rectangle(
	%age_lower%,
	%age_upper%,
	%time_lower%,
	%time_upper%,
	%weight_id%,
	%subgroup_id%,
	%integrand_id%,
	%x%,
	%pack_vec%
)%$$

$head Prototype$$
$srcthisfile%
	0%// BEGIN_RECTANGLE_PROTOTYPE%// END_RECTANGLE_PROTOTYPE%1
%$$

$head age_lower$$
the lower age in the rectangle.

$head age_upper$$
the upper age in the rectangle; $icode%age_lower% <= %age_upper%$$.

$head time_lower$$
the lower time in the rectangle.

$head time_upper$$
the upper time in the rectangle; $icode%time_lower% <= %time_upper%$$.

$head weight_id$$
This is the $cref/weight_id/weight_table/weight_id/$$
in the weight table corresponding to this average integrand.

$head subgroup_id$$
This is the $cref/subgroup_id/subgroup_table/subgroup_id/$$
in the subgroup table corresponding to this average integrand.

$head integrand_id$$
This is the $cref/integrand_id/integrand_table/integrand_id/$$
in the integrand table corresponding to this average integrand.

$head x$$
This is the vector of covariates for this average.

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head pack_vec$$
is all the $cref model_variables$$ in the order
specified by $icode pack_object$$.

$head avg$$
The return value $icode avg$$ is the average of the integrand
using the specified weighting over the specified rectangle

$children%example/devel/model/avg_noise_effect_xam.cpp
%$$

$head Example$$
The file $cref avg_noise_effect_xam.cpp$$ contains an example and test
of using this routine.

$end
*/

// BEGIN_RECTANGLE_PROTOTYPE
template <class Float>
Float avg_noise_effect::rectangle(
	double                           age_lower        ,
	double                           age_upper        ,
	double                           time_lower       ,
	double                           time_upper       ,
	size_t                           weight_id        ,
	size_t                           subgroup_id      ,
	size_t                           integrand_id     ,
	const CppAD::vector<double>&     x                ,
	const CppAD::vector<Float>&      pack_vec         ,
// END_RECTANGLE_PROTOTYPE
	time_line_vec<Float>&            time_line_object ,
	CppAD::vector<Float>&            effect           )
{	using CppAD::vector;
	typedef typename time_line_vec<Float>::time_point  time_point;

	// numerical precision
	double eps99 = 99.0 * std::numeric_limits<double>::epsilon();

	// weight information for this average
	// constant weighting is at the end of w_info_vec_
	size_t weight_index = w_info_vec_.size() - 1;
	if( weight_id != DISMOD_AT_NULL_SIZE_T )
	{	assert( weight_id < weight_index );
		weight_index = weight_id;
	}
	const weight_info& w_info( w_info_vec_[weight_index] );

	// number of ages and time in the weight grid
	size_t n_age  = w_info.age_size();
	size_t n_time = w_info.time_size();

	// weight_grid_
	weight_grid_.resize(n_age * n_time);
	for(size_t i = 0; i < n_age; i++)
	{	for(size_t j = 0; j < n_time; ++j)
			weight_grid_[i * n_time + j] = w_info.weight(i, j);
	}


	// specialize the time_line object for this rectangle
	time_line_object.specialize(
		age_lower, age_upper, time_lower, time_upper
	);

	// The extended age grid
	const vector<double>& extend_grid = time_line_object.extend_grid();
	size_t                sub_lower   = time_line_object.sub_lower();
	size_t                sub_upper   = time_line_object.sub_upper();

	// age_lower == extend_grid[sub_lower]
	assert(time_line_vec<Float>::near_equal(extend_grid[sub_lower],age_lower));

	// age_upper == extend_grid[sub_upper]
	assert(time_line_vec<Float>::near_equal(extend_grid[sub_upper],age_upper));

	// n_age: number of ages (time line for each time line)
	n_age = sub_upper - sub_lower + 1;

	// one_time
	bool one_time = time_line_vec<double>::near_equal(time_lower, time_upper);

	// n_time: number times in each time line
	// d_time: spacing between time points
	n_time        = 1;
	double d_time = 0.0;
	if( ! one_time )
	{	n_time = size_t(
			2.0 - eps99 + (time_upper - time_lower) / ode_step_size_
		);
		d_time = (time_upper - time_lower) / double(n_time - 1);
	}

	// n_line: total number of age, time points
	size_t n_line = n_age * n_time;

	// resize temporaris
	line_age_.resize(n_line);
	line_time_.resize(n_line);

	// line_age_
	// line_time_
	for(size_t i = 0; i < n_age; ++i)
	{	for(size_t j = 0; j < n_time; ++j)
		{	size_t k =  i * n_time + j;
			size_t age_index = sub_lower + i;
			line_age_[k]     = extend_grid[age_index];
			line_time_[k]    = time_lower + double(j) * d_time;
		}
	}

	// -----------------------------------------------------------------------
	// effect
	// -----------------------------------------------------------------------
	//
	// some temporaries
	pack_info::subvec_info info;
	vector<Float> temp(n_line), smooth_value;
	//
	// initialize effect as zero
	effect.resize(n_line);
	for(size_t k = 0; k < n_line; ++k)
		effect[k] = 0.0;
	//
	// sum over the noise covariates for this integrand
	size_t n_cov = pack_object_.group_meas_noise_n_cov(integrand_id);
	for(size_t j = 0; j < n_cov; ++j)
	{	info  = pack_object_.group_meas_noise_info(integrand_id, j);
		size_t group_id  = info.group_id;
		size_t smooth_id = info.smooth_id;
		double x_j       = x[ info.covariate_id ];
		if( group_id == size_t( subgroup_table_[subgroup_id].group_id ) )
		{	// interpolate from smoothing grid to cohort
			smooth_value.resize(info.n_var);
			for(size_t k = 0; k < info.n_var; ++k)
				smooth_value[k] = pack_vec[info.offset + k];
			const smooth_info& s_info = s_info_vec_[smooth_id];
			temp = grid2line(
				line_age_,
				line_time_,
				age_table_,
				time_table_,
				s_info,
				smooth_value
			);
			// add in this multiplier times covariate effect
			for(size_t k = 0; k < n_line; ++k)
				effect[k] += temp[k] * x_j;
		}
	}
	// -----------------------------------------------------------------------
	// line_weight_
	line_weight_.resize(n_line);
	line_weight_ = grid2line(
		line_age_,
		line_time_,
		age_table_,
		time_table_,
		w_info,
		weight_grid_
	);
	for(size_t i = 0; i < n_age; ++i)
	{	for(size_t j = 0; j < n_time; ++j)
		{	time_point point;
			size_t k         = i * n_time + j;
			size_t age_index = sub_lower + i;
			point.time       = line_time_[k];
			point.weight     = line_weight_[k];
			point.value      = effect[k];
			time_line_object.add_point(age_index, point);
		}
	}
	Float avg = time_line_object.age_time_avg();
	return avg;
}

# define DISMOD_AT_INSTANTIATE_AVG_NOISE_EFFECT_RECTANGLE(Float) \
	template                                                   \
	Float avg_noise_effect::rectangle(                           \
		double                           age_lower        ,    \
		double                           age_upper        ,    \
		double                           time_lower       ,    \
		double                           time_upper       ,    \
		size_t                           weight_id        ,    \
		size_t                           subgroup_id      ,    \
		size_t                           integrand_id     ,    \
		const CppAD::vector<double>&     x                ,    \
		const CppAD::vector<Float>&      pack_vec         ,    \
		time_line_vec<Float>&            time_line_object ,    \
		CppAD::vector<Float>&            line_adj              \
	);                                                         \
\
	Float avg_noise_effect::rectangle(                           \
		double                           age_lower        ,    \
		double                           age_upper        ,    \
		double                           time_lower       ,    \
		double                           time_upper       ,    \
		size_t                           weight_id        ,    \
		size_t                           subgroup_id      ,    \
		size_t                           integrand_id     ,    \
		const CppAD::vector<double>&     x                ,    \
		const CppAD::vector<Float>&      pack_vec         )    \
	{	return rectangle(                                      \
			age_lower,                                         \
			age_upper,                                         \
			time_lower,                                        \
			time_upper,                                        \
			weight_id,                                         \
			subgroup_id,                                       \
			integrand_id,                                      \
			x,                                                 \
			pack_vec,                                          \
			Float ## _time_line_object_,                       \
			Float ## _effect_                                  \
		);                                                     \
	}

// instantiations
DISMOD_AT_INSTANTIATE_AVG_NOISE_EFFECT_RECTANGLE( double )
DISMOD_AT_INSTANTIATE_AVG_NOISE_EFFECT_RECTANGLE( a1_double )

} // END_DISMOD_AT_NAMESPACE
