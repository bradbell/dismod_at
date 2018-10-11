/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <cppad/mixed/exception.hpp>
# include <dismod_at/adj_integrand.hpp>
# include <dismod_at/null_int.hpp>

/*
$begin adj_integrand$$
$spell
	vec
	const
	CppAD
$$

$section Compute Adjusted Integrand On a Cohort$$

$head Under Construction$$

$head Syntax$$
$icode%adjust_integrand% = adj_integrand(
	%rate_case%,
	%age_table%,
	%time_table%,
	%integrand_table%,
	%s_info_vec%,
	%integrand_id%,
	%n_child%,
	%child%,
	%x%,
	%cohort_age%,
	%cohort_time%,
	%pack_object%,
	%pack_vec%
)%$$

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head n_cohort$$
We use the notation $icode n_cohort$$ for the number of grid
points along this cohort at which the approximate solution is returned.

$head rate_case$$
This is the value of
$cref/rate_case/option_table/rate_case/$$ in the option table.

$head age_table$$
This argument is the $cref age_table$$.

$head time_table$$
This argument is the $cref time_table$$.

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
None of the prior information in $icode s_info_vec$$ is used.

$head integrand_table$$
This argument is the $cref integrand_table$$.

$head integrand_id$$
This is the $cref/integrand_id/integrand_table/integrand_id/$$
in the integrand table.

$head n_child_$$
set to number of $cref/children/option_table/parent_node_id/Children/$$.

$head child$$
Is the $cref/child/child_info/table_id2child/child/$$ corresponding
to this adjustment of the integrand.

$head cohort_age$$
This vector has size $icode n_cohort$$ and is
the age grid on which the ODE is solved.
The $cref/rates/rate_table/$$
are approximated as constant for each interval in this grid.

$head cohort_time$$
This vector has size $icode n_cohort$$ and is
the time corresponding to each cohort age.
The initial time $icode%cohort_time%[0]%$$ is arbitrary.
For $icode%i% = 1 , %...%, %n_cohort%-1%$$
$codei%
	%cohort_time%[%i%] = %cohort_age%[%i%] + %cohort_time%[0] - %cohort_age%[0]
%$$

$head pack_object$$
This is the $cref pack_info$$ information corresponding to
the $cref model_variables$$.

$head pack_vec$$
is all the $cref model_variables$$ in the order
specified by $icode pack_object$$.

$head adjust_integrand$$
The return value is a vector with size $icode n_cohort$$
and $icode%adjust_integrand%[%i%]%$$ is the
$cref/adjusted integrand/avg_integrand/Adjusted Integrand/$$
at age $icode%cohort_age%[%i%]%$$
and time $icode%cohort_time%[%i%]%$$.

$head Prototype$$
$srcfile%devel/model/adj_integrand.cpp%
	0%// BEGIN_PROTOTYPE%// END_PROTOTYPE%1
%$$
$end
*/

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

// BEGIN_PROTOTYPE
template <class Float>
CppAD::vector<Float> adj_integrand(
	const std::string&                        rate_case        ,
	const CppAD::vector<double>&              age_table        ,
	const CppAD::vector<double>&              time_table       ,
	const CppAD::vector<smooth_info>&         s_info_vec       ,
	const CppAD::vector<integrand_struct>&    integrand_table  ,
	size_t                                    integrand_id     ,
	size_t                                    n_child          ,
	size_t                                    child            ,
	const CppAD::vector<double>&              x                ,
	const CppAD::vector<double>&              cohort_age       ,
	const CppAD::vector<double>&              cohort_time      ,
	const pack_info&                          pack_object      ,
	const CppAD::vector<Float>&               pack_vec         )
// END_PROTOTYPE
{	using CppAD::vector;
	//
	// some temporaries
	pack_info::subvec_info info;
	vector<Float> smooth_value;
	vector< vector<Float> >& rate;
	//
	// integrand for this data point
	integrand_enum integrand = integrand_table[integrand_id].integrand;
	//
	// number of points in this cohort
	size_t n_cohort = cohort_age.size();
	//
	// vector of effects
	vector<Float> effect(n_cohort), temp(n_cohort);
	// -----------------------------------------------------------------------
	// get value for each rate on that cohort line
	for(size_t rate_id = 0; rate_id < number_rate_enum; ++rate_id)
	{	rate[rate_id].resize(n_cohort);
		//
		// parent rate for each point in the cohort
		info             = pack_object.rate_info(rate_id, n_child);
		size_t smooth_id = info.smooth_id;
		//
		if( smooth_id == DISMOD_AT_NULL_SIZE_T )
		{	for(size_t k = 0; k < n_cohort; ++k)
				rate[rate_id][k] = 0.0;
		}
		else
		{	// interpolate this rate from smoothing grid to cohort
			smooth_value.resize(info.n_var);
			for(size_t k = 0; k < info.n_var; ++k)
				smooth_value[k] = pack_vec[info.offset + k];
			const smooth_info& s_info = s_info_vec[smooth_id];
			rate[rate_id] = smooth2line(
				cohort_age,
				cohort_time,
				age_table,
				time_table,
				s_info,
				smooth_value
			);
		}
		//
		// initialize effect as zero
		for(size_t k = 0; k < n_cohort; ++k)
			effect[k] = 0.0;
		//
		// include the child effect
		if( child < n_child )
		{	// child effect rate for each point in the cohort
			info      = pack_object.rate_info(rate_id, child);
			smooth_id = info.smooth_id;
			if( smooth_id != DISMOD_AT_NULL_SIZE_T )
			{	// interpolate from smoothing grid to cohort
				smooth_value.resize(info.n_var);
				for(size_t k = 0; k < info.n_var; ++k)
					smooth_value[k] = pack_vec[info.offset + k];
				const smooth_info& s_info = s_info_vec[smooth_id];
				temp = smooth2line(
					cohort_age,
					cohort_time,
					age_table,
					time_table,
					s_info,
					smooth_value
				);
				for(size_t k = 0; k < n_cohort; ++k)
					effect[k] += temp[k];
			}
		}
		//
		// include the covariate effects on this rate
		size_t n_cov = pack_object.mulcov_rate_value_n_cov(rate_id);
		for(size_t j = 0; j < n_cov; ++j)
		{	info        = pack_object.mulcov_rate_value_info(rate_id, j);
			smooth_id   = info.smooth_id;
			double x_j  = x[ info.covariate_id ];
			// interpolate from smoothing grid to cohort
			smooth_value.resize(info.n_var);
			for(size_t k = 0; k < info.n_var; ++k)
				smooth_value[k] = pack_vec[info.offset + k];
			const smooth_info& s_info = s_info_vec[smooth_id];
			temp = smooth2line(
				cohort_age,
				cohort_time,
				age_table,
				time_table,
				s_info,
				smooth_value
			);
			for(size_t k = 0; k < n_cohort; ++k)
				effect[k] += temp[k] * x_j;
		}
		//
		// multiply parent rate by exponential of the total effect
		for(size_t k = 0; k < n_cohort; ++k)
			rate[rate_id][k] *= exp( effect[k] );
	}
	// -----------------------------------------------------------------------
	// solve the ode on the cohort line
	Float pini = rate[pini_enum][0];
	vector<Float> s_out(n_cohort), c_out(n_cohort);
	cohort_ode(
		rate_case,
		cohort_age,
		pini,
		rate[iota_enum],
		rate[rho_enum],
		rate[chi_enum],
		rate[omega_enum],
		s_out,
		c_out
	);
	// -----------------------------------------------------------------------
	// value of the integrand on the cohort line
	vector<Float> result(n_cohort);
	Float infinity = std::numeric_limits<double>::infinity();
	Float zero     =  0.0;
	for(size_t k = 0; k < n_cohort; ++k)
	{	Float P = c_out[k] / ( s_out[k] + c_out[k] );
		bool ok = zero <= P && P < infinity;
		ok     |= integrand == susceptible_enum;
		ok     |= integrand == withC_enum;
		if( ! ok )
		{	std::string message = "Numerical integration error.\n"
			"Prevalence is negative or infinite or Nan.";
			throw CppAD::mixed::exception( "avg_integrand_cohort", message);
		}
		switch(integrand)
		{
			case susceptible_enum:
			result[k] = s_out[k];
			break;

			case withC_enum:
			result[k] = c_out[k];
			break;

			case prevalence_enum:
			result[k] = P;
			break;

			case Tincidence_enum:
			result[k] = rate[iota_enum][k] * (1.0 - P);
			break;

			case mtspecific_enum:
			result[k] = rate[chi_enum][k] * P;
			break;

			case mtall_enum:
			result[k] = rate[omega_enum][k] + rate[chi_enum][k] * P;
			break;

			case mtstandard_enum:
			result[k] = rate[omega_enum][k] + rate[chi_enum][k];
			result[k] /= rate[omega_enum][k] + rate[chi_enum][k] * P;
			break;

			// This integrand does not requre the ode; i.e., S or C
			default:
			assert(false);
		}
	}
	// -----------------------------------------------------------------------
	// include measurement effects
	//
	// initialize effect as zero
	for(size_t k = 0; k < n_cohort; ++k)
		effect[k] = 0.0;
	size_t n_cov = pack_object.mulcov_meas_value_n_cov(integrand_id);
	for(size_t j = 0; j < n_cov; ++j)
	{	info             = pack_object.mulcov_meas_value_info(integrand_id, j);
		size_t smooth_id = info.smooth_id;
		double x_j       = x[ info.covariate_id ];
		// interpolate from smoothing grid to cohort
		smooth_value.resize(info.n_var);
		for(size_t k = 0; k < info.n_var; ++k)
			smooth_value[k] = pack_vec[info.offset + k];
		const smooth_info& s_info = s_info_vec[smooth_id];
		temp = smooth2line(
			cohort_age,
			cohort_time,
			age_table,
			time_table,
			s_info,
			smooth_value
		);
		for(size_t k = 0; k < n_cohort; ++k)
			effect[k] += temp[k] * x_j;
	}
	// multiply by exponential of total effect
	for(size_t k = 0; k < n_cohort; ++k)
		 result[k] *= exp( effect[k] );
	//
	return result;
}

} // END_DISMOD_AT_NAMESPACE
