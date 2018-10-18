/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <cppad/mixed/exception.hpp>
# include <dismod_at/avg_integrand.hpp>
# include <dismod_at/grid2line.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

/*
------------------------------------------------------------------------------
$begin avg_integrand_ctor$$
$spell
	vec
	const
	CppAD
	Integrands
$$

$section Constructing An  Average Integrand Object$$

$head Syntax$$
$codei%avg_integrand %avg_object%(
	%ode_step_size%,
	%rate_case%,
	%ode_age_grid%,
	%age_table%,
	%time_table%,
	%integrand_table%,
	%w_info_vec%,
	%s_info_vec%,
	%pack_object%
);
%$$

$head Prototype$$
$srcfile%devel/model/avg_integrand.cpp%
	0%// BEGIN_AVG_INTEGRAND_PROTOTYPE%// END_AVG_INTEGRAND_PROTOTYPE%1
%$$

$head ode_step_size$$
This is the value of
$cref/ode_step_size/option_table/ode_step_size/$$ in the option table.

$head rate_case$$
This is the value of
$cref/rate_case/option_table/rate_case/$$ in the option table.

$head ode_age_grid$$
This is the $cref ode_age_grid$$.
A reference to $icode ode_age_grid$$ is used by $icode avg_object$$
($icode ode_age_grid$$ must no be deleted for as long as
$icode avg_object$$ is used).

$head age_table$$
This argument is the $cref age_table$$.
A reference to $icode age_table$$ is used by $icode avg_object$$
($icode age_table$$ must no be deleted for as long as
$icode avg_object$$ is used).

$head time_table$$
This argument is the $cref time_table$$.
A reference to $icode time_table$$ is used by $icode avg_object$$.

$head integrand_table$$
This argument is the $cref integrand_table$$.
A reference to $icode integrand_table$$ is used by $icode avg_object$$.

$head w_info_vec$$
For each $cref/weight_id/weight_table/weight_id/$$,
$codei%
	%w_info_vec%[ %weight_id% ]
%$$
is the corresponding $cref weight_info$$ information.

$head s_info_vec$$
For each $cref/smooth_id/smooth_table/smooth_id/$$,
$codei%
	%s_info_vec%[ %smooth_id% ]
%$$
is the corresponding $cref smooth_info$$ information.
None of the prior information in $icode s_info_vec$$ is used.
A reference to $icode s_info_vec$$ is used by $icode avg_object$$.

$head pack_object$$
This is the $cref pack_info$$ information corresponding to
the $cref model_variables$$.

$head Example$$
The file $cref avg_integrand_xam.cpp$$ contains an example and test
of using this routine.

$end
*/


// BEGIN_AVG_INTEGRAND_PROTOTYPE
avg_integrand::avg_integrand(
		double                                    ode_step_size    ,
		const std::string&                        rate_case        ,
		const CppAD::vector<double>&              ode_age_grid     ,
		const CppAD::vector<double>&              age_table        ,
		const CppAD::vector<double>&              time_table       ,
		const CppAD::vector<integrand_struct>&    integrand_table  ,
		const CppAD::vector<weight_info>&         w_info_vec       ,
		const CppAD::vector<smooth_info>&         s_info_vec       ,
		const pack_info&                          pack_object      )
// END_AVG_INTEGRAND_PROTOTYPE
:
ode_step_size_             ( ode_step_size )   ,
age_table_                 ( age_table )       ,
time_table_                ( time_table )      ,
integrand_table_           ( integrand_table ) ,
w_info_vec_                ( w_info_vec )      ,
double_time_line_object_   ( ode_age_grid )    ,
a1_double_time_line_object_( ode_age_grid )    ,
adj_object_(
	rate_case,
	age_table,
	time_table,
	integrand_table,
	s_info_vec,
	pack_object
)
{ }
/*
------------------------------------------------------------------------------
$begin avg_integrand_rectangle$$
$spell
	vec
	const
	CppAD
	Integrands
	covariates
$$

$section Computing One Average Integrand$$

$head Syntax$$
$icode%avg% = %avg_object%.rectangle(
	%age_lower%,
	%age_upper%,
	%time_lower%,
	%time_upper%,
	%weight_id%,
	%integrand_id%,
	%n_child%,
	%child%,
	%x%,
	%pack_vec%
)%$$

$head Prototype$$
$srcfile%devel/model/avg_integrand.cpp%
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

$head integrand_id$$
This is the $cref/integrand_id/integrand_table/integrand_id/$$
in the integrand table corresponding to this average integrand.

$head n_child_$$
set to number of $cref/children/option_table/parent_node_id/Children/$$.

$head child$$
Is the $cref/child/child_info/table_id2child/child/$$ corresponding
to this average.

$head x$$
This is the vector of covariates for this average.

$head pack_vec$$
is all the $cref model_variables$$ in the order
specified by $icode pack_object$$.

$subhead Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$children%example/devel/model/avg_integrand_xam.cpp
%$$

$head Example$$
The file $cref avg_integrand_xam.cpp$$ contains an example and test
of using this routine.

$end
*/

// BEGIN_RECTANGLE_PROTOTYPE
template <class Float>
Float avg_integrand::rectangle(
	double                           age_lower        ,
	double                           age_upper        ,
	double                           time_lower       ,
	double                           time_upper       ,
	size_t                           weight_id        ,
	size_t                           integrand_id     ,
	size_t                           n_child          ,
	size_t                           child            ,
	const CppAD::vector<double>&     x                ,
	const CppAD::vector<Float>&      pack_vec         ,
// END_RECTANGLE_PROTOTYPE
	time_line_vec<Float>&            time_line_object ,
	CppAD::vector<Float>&            line_adj         )
{	using CppAD::vector;
	typedef typename time_line_vec<Float>::time_point  time_point;
	double eps99 = std::numeric_limits<double>::epsilon();

	// weight information for this average
	const weight_info& w_info( w_info_vec_[weight_id] );

	// number of ages and time in the weight grid
	size_t n_age  = w_info.age_size();
	size_t n_time = w_info.time_size();

	// weight_grid_
	weight_grid_.resize(n_age * n_time);
	for(size_t i = 0; i < n_age; i++)
	{	for(size_t j = 0; j < n_time; ++j)
			weight_grid_[i * n_time + j] = w_info.weight(i, j);
	}


	// integrand for this average
	integrand_enum integrand = integrand_table_[integrand_id].integrand;

	// need_ode
	bool need_ode = false;
	switch( integrand )
	{	// -----------------------------------------------------------------
		// need_ode = true;
		case susceptible_enum:
		case withC_enum:
		case prevalence_enum:
		case Tincidence_enum:
		case mtspecific_enum:
		case mtall_enum:
		case mtstandard_enum:
		need_ode = true;
		break;

		// -----------------------------------------------------------------
		case Sincidence_enum:
		case remission_enum:
		case mtexcess_enum:
		case mtother_enum:
		case mtwith_enum:
		case relrisk_enum:
		break;

		// -----------------------------------------------------------------
		default:
		assert( false );
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

	// -----------------------------------------------------------------------
	if( ! need_ode )
	// -----------------------------------------------------------------------
	{	// n_time: number times in each time line
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
		//
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
		// line_adj_
		line_adj.resize(n_line);
		line_adj = adj_object_.line(
			line_age_,
			line_time_,
			integrand_id,
			n_child,
			child,
			x,
			pack_vec
		);
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
				point.value      = line_adj[k];
				time_line_object.add_point(age_index, point);
			}
		}
		Float avg = time_line_object.age_time_avg();
		return avg;
	}
	// -----------------------------------------------------------------------
	assert( need_ode );
	// -----------------------------------------------------------------------
	// cohorts that go through extended age grid and rectangle at time_lower
	// -----------------------------------------------------------------------
	for(size_t age_index = sub_lower; age_index <= sub_upper; ++age_index)
	{	// time_initial: initial time for this cohort
		double initial_time = time_lower - extend_grid[age_index];
		//
		// final_age: final age index for this cohort
		size_t final_index = sub_upper;
		double final_time  = time_lower + extend_grid[final_index] - age_lower;
		bool ok = time_line_vec<Float>::near_equal(final_time, time_upper);
		ok     |= final_time <= time_upper;
		while( ! ok )
		{	--final_index;
			assert( final_index >= age_index );
			final_time = time_lower + extend_grid[final_index] - age_lower;
			ok         = final_time <= time_upper;
		}
		// number of age points in this cohort
		size_t n_line = final_index + 1;
		//
		// line_age_, line_time_
		line_age_.resize(n_line);
		line_time_.resize(n_line);
		for(size_t k = 0; k < n_line; ++k)
		{	line_age_[k]  = extend_grid[k];
			line_time_[k] = initial_time + line_age_[k] - extend_grid[0];
		}
		// line_adj_
		line_adj.resize(n_line);
		line_adj = adj_object_.line(
			line_age_,
			line_time_,
			integrand_id,
			n_child,
			child,
			x,
			pack_vec
		);
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
		for(size_t k = sub_lower; k <= final_index; ++k)
		{	time_point point;
			point.time       = line_time_[k];
			point.weight     = line_weight_[k];
			point.value      = line_adj[k];
			time_line_object.add_point(k, point);
		}
	}
	// -----------------------------------------------------------------------
	if( one_time )
	{	Float avg = time_line_object.age_time_avg();
		return avg;
	}
	// -----------------------------------------------------------------------
	// cohorts that go through extended age grid and rectangle at time_upper
	// -----------------------------------------------------------------------
	for(size_t age_index = sub_lower; age_index <= sub_upper; ++age_index)
	{	// time line corresponding to this age
		const vector<time_point>& time_line =
			time_line_object.time_line(age_index);

		// last time in this time line
		n_time            = time_line.size();
		double final_time = time_line[n_time - 1].time;

		// if time_upper is in time line, it is the last time
		bool found = time_line_vec<Float>::near_equal(final_time, time_upper);
		if( ! found )
		{	// initial time for this cohort
			double initial_time = time_upper - extend_grid[age_index];

			// number of points in this cohort
			size_t n_line = age_index + 1;
			//
			// line_age_, line_time_
			line_age_.resize(n_line);
			line_time_.resize(n_line);
			for(size_t k = 0; k < n_line; ++k)
			{	line_age_[k]  = extend_grid[k];
				line_time_[k] = initial_time + line_age_[k] - extend_grid[0];
			}
			// line_adj_
			line_adj.resize(n_line);
			line_adj = adj_object_.line(
				line_age_,
				line_time_,
				integrand_id,
				n_child,
				child,
				x,
				pack_vec
			);
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
			for(size_t k = 0; k <= age_index; ++k)
			{	time_point point;
				point.time       = line_time_[k];
				point.weight     = line_weight_[k];
				point.value      = line_adj[k];
				time_line_object.add_point(k, point);
			}
		}
	}
	// -----------------------------------------------------------------------
	Float avg = time_line_object.age_time_avg();
	return avg;
}
/*
-----------------------------------------------------------------------------
$begin avg_integrand_add_cohort$$
$spell
	vec
	pini
	covariates
$$

$section Add ODE Solution for One Cohort To time_line_object$$

$head Syntax$$
$icode%avg% = %avg_object%.add_cohort(
	%n_line%,
	%weight_id%,
	%integrand_id%,
	%n_child%,
	%child%,
	%x%,
	%pack_vec%,
	%extend_grid%,
	%time_line_object%,
	%line_adj%
)%$$

$head Prototype$$
$srcfile%devel/model/avg_integrand.cpp%
	0%// BEGIN_ADD_COHORT_PROTOTYPE%// END_ADD_COHORT_PROTOTYPE%1
%$$

$head initial_time$$
is the initial time for this cohort; i.e., the time
corresponding to $cref/pini/rate_table/rate_name/pini/$$.

$head n_line$$
Is the number of age points in this cohort starting at the minimum
age corresponding to $cref/pini/rate_table/rate_name/pini/$$.

$head weight_id$$
This is the $cref/weight_id/weight_table/weight_id/$$
in the weight table corresponding to this average integrand.

$head integrand_id$$
This is the $cref/integrand_id/integrand_table/integrand_id/$$
in the integrand table corresponding to this average integrand.

$head n_child_$$
set to number of $cref/children/option_table/parent_node_id/Children/$$.

$head child$$
Is the $cref/child/child_info/table_id2child/child/$$ corresponding
to this average.

$head x$$
This is the vector of covariates for this average.

$subhead Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head pack_vec$$
is all the $cref model_variables$$ in the order
specified by $icode pack_object$$.

$head time_line_object$$
This is the object that are adding the cohorts $code time_point$$ to.

$head line_adj$$
This a temporary vector that is re-sized and over written.

$end
*/
// BEGIN_ADD_COHORT_PROTOTYPE
template <class Float>
void avg_integrand::add_cohort(
	double                       initial_time                     ,
	size_t                       n_line                           ,
	size_t                       weight_id                        ,
	size_t                       integrand_id                     ,
	size_t                       n_child                          ,
	size_t                       child                            ,
	const CppAD::vector<double>& x                                ,
	const CppAD::vector<Float>&  pack_vec                         ,
	time_line_vec<Float>&        time_line_object                 ,
	CppAD::vector<Float>&        line_adj                         )
// END_ADD_COHORT_PROTOTYPE
{	// weight information for this average
	const weight_info& w_info( w_info_vec_[weight_id] );

	// extend_grid
	const CppAD::vector<double>& extend_grid = time_line_object.extend_grid();

	// sub_lower
	size_t sub_lower = time_line_object.sub_lower();

	// line_age_, line_time_
	line_age_.resize(n_line);
	line_time_.resize(n_line);
	for(size_t k = 0; k < n_line; ++k)
	{	line_age_[k]  = extend_grid[k];
		line_time_[k] = initial_time + line_age_[k] - extend_grid[0];
	}
	// line_adj_
	line_adj.resize(n_line);
	line_adj = adj_object_.line(
		line_age_,
		line_time_,
		integrand_id,
		n_child,
		child,
		x,
		pack_vec
	);
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
	// add points
	assert( sub_lower <= n_line);
	for(size_t k = sub_lower; k < n_line; ++k)
	{	typename time_line_vec<Float>::time_point point;
		point.time       = line_time_[k];
		point.weight     = line_weight_[k];
		point.value      = line_adj[k];
		time_line_object.add_point(k, point);
	}
	return;
}

# define DISMOD_AT_INSTANTIATE_AVG_INTEGTAND_RECTANGLE(Float)  \
    template                                                   \
	Float avg_integrand::rectangle(                            \
		double                           age_lower        ,    \
		double                           age_upper        ,    \
		double                           time_lower       ,    \
		double                           time_upper       ,    \
		size_t                           weight_id        ,    \
		size_t                           integrand_id     ,    \
		size_t                           n_child          ,    \
		size_t                           child            ,    \
		const CppAD::vector<double>&     x                ,    \
		const CppAD::vector<Float>&      pack_vec         ,    \
		time_line_vec<Float>&            time_line_object ,    \
		CppAD::vector<Float>&            line_adj              \
	);                                                         \
\
	Float avg_integrand::rectangle(                            \
		double                           age_lower        ,    \
		double                           age_upper        ,    \
		double                           time_lower       ,    \
		double                           time_upper       ,    \
		size_t                           weight_id        ,    \
		size_t                           integrand_id     ,    \
		size_t                           n_child          ,    \
		size_t                           child            ,    \
		const CppAD::vector<double>&     x                ,    \
		const CppAD::vector<Float>&      pack_vec         )    \
	{	return rectangle(                                      \
			age_lower,                                         \
			age_upper,                                         \
			time_lower,                                        \
			time_upper,                                        \
			weight_id,                                         \
			integrand_id,                                      \
			n_child,                                           \
			child,                                             \
			x,                                                 \
			pack_vec,                                          \
			Float ## _time_line_object_,                       \
			Float ## _line_adj_                                \
		);                                                     \
	}                                                          \
\
    template                                                   \
	void avg_integrand::add_cohort(                            \
		double                       initial_time         ,    \
		size_t                       n_line               ,    \
		size_t                       weight_id            ,    \
		size_t                       integrand_id         ,    \
		size_t                       n_child              ,    \
		size_t                       child                ,    \
		const CppAD::vector<double>& x                    ,    \
		const CppAD::vector<Float>&  pack_vec             ,    \
		time_line_vec<Float>&        time_line_object     ,    \
		CppAD::vector<Float>&        line_adj                  \
		);

// instantiations
DISMOD_AT_INSTANTIATE_AVG_INTEGTAND_RECTANGLE( double )
DISMOD_AT_INSTANTIATE_AVG_INTEGTAND_RECTANGLE( a1_double )

} // END_DISMOD_AT_NAMESPACE
