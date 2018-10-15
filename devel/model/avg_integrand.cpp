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

/*
$begin devel_avg_integrand$$
$spell
	vec
	const
	CppAD
	Integrands
$$

$section Computing Average Integrands$$

$head Under Construction$$

$head Syntax$$
$codei%avg_integrand %avg_object%(
	%rate_case%,
	%age_ode_grid%,
	%age_table%,
	%time_table%,
	%integrand_table%,
	%w_info_vec%,
	%s_info_vec%,
	%pack_object%
);
%$$
$icode%avg% = %avg_object%.rectangle(
	%age_lower%,
	%age_upper%,
	%time_lower%,
	%time_upper%,
	%integrand_id%,
	%n_child%,
	%child%,
	%x%,
	%pack_vec%
)%$$

$head Prototype$$
$srcfile%devel/model/avg_integrand.cpp%
	0%// BEGIN_AVG_INTEGRAND_PROTOTYPE%// END_AVG_INTEGRAND_PROTOTYPE%1
%$$
$srcfile%devel/model/avg_integrand.cpp%
	0%// BEGIN_RECTANGLE_PROTOTYPE%// END_RECTANGLE_PROTOTYPE%1
%$$

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

$head age_lower, age_upper, time_lower, time_upper$$
are the lower age, upper age, lower time, and upper time
that define the rectangle over which the integrand is averaged.

$head integrand_id$$
This is the $cref/integrand_id/integrand_table/integrand_id/$$
in the integrand table.

$head n_child_$$
set to number of $cref/children/option_table/parent_node_id/Children/$$.

$head child$$
Is the $cref/child/child_info/table_id2child/child/$$ corresponding
to this average.

$head pack_vec$$
is all the $cref model_variables$$ in the order
specified by $icode pack_object$$.

$subhead Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$comment%example/devel/model/avg_integrand_xam.cpp
%$$
$head Example$$
The file $comment avg_integrand_xam.cpp$$ contains an example and test
of using this routine.

$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// BEGIN_AVG_INTEGRAND_PROTOTYPE
avg_integrand::avg_integrand(
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
integrand_table_           ( integrand_table ) ,
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

// BEGIN_RECTANGLE_PROTOTYPE
template <class Float>
Float avg_integrand::rectangle(
	double                           age_lower        ,
	double                           age_upper        ,
	double                           time_lower       ,
	double                           time_upper       ,
	size_t                           integrand_id     ,
	size_t                           n_child          ,
	size_t                           child            ,
	const CppAD::vector<double>&     x                ,
	const CppAD::vector<Float>&      pack_vec         ,
// END_RECTANGLE_PROTOTYPE
	time_line_vec<Float>&            time_line_object ,
	CppAD::vector<Float>&            float_vec        )
{	using CppAD::vector;

	// specialize the time_line object for this rectangle
	time_line_object.specialize(
		age_lower, age_upper, time_lower, time_upper
	);

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

	// The extended age grid
	const vector<double>& extend_grid = time_line_object.extend_grid();
	size_t                sub_lower   = time_line_object.sub_lower();
	size_t                sub_upper   = time_line_object.sub_upper();

	// one_time
	bool one_time = time_line_vec<double>::near_equal(time_lower, time_upper);


	return Float(0);
}

# define DISMOD_AT_INSTANTIATE_AVG_INTEGTAND_RECTANGLE(Float)  \
    template                                                   \
	Float avg_integrand::rectangle(                            \
		double                           age_lower        ,    \
		double                           age_upper        ,    \
		double                           time_lower       ,    \
		double                           time_upper       ,    \
		size_t                           integrand_id     ,    \
		size_t                           n_child          ,    \
		size_t                           child            ,    \
		const CppAD::vector<double>&     x                ,    \
		const CppAD::vector<Float>&      pack_vec         ,    \
		time_line_vec<Float>&            time_line_object ,    \
		CppAD::vector<Float>&            float_vec             \
	);                                                         \
\
	Float avg_integrand::rectangle(                            \
		double                           age_lower        ,    \
		double                           age_upper        ,    \
		double                           time_lower       ,    \
		double                           time_upper       ,    \
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
			integrand_id,                                      \
			n_child,                                           \
			child,                                             \
			x,                                                 \
			pack_vec,                                          \
			Float ## _time_line_object_,                       \
			Float ## _vec_                                     \
		);                                                     \
	}

// instantiations
DISMOD_AT_INSTANTIATE_AVG_INTEGTAND_RECTANGLE( double )
DISMOD_AT_INSTANTIATE_AVG_INTEGTAND_RECTANGLE( a1_double )

} // END_DISMOD_AT_NAMESPACE
