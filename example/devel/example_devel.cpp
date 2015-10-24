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
$begin example_devel.cpp$$

$sectin Run C++ Examples$$

$head Syntax$$
$code example/devel/cpp$$

$head Purpose$$
This runs all the C++ examples and prints out their correctness
test results together with a summary result at the end.

$end
-----------------------------------------------------------------------------
*/
# include <iostream>
# include <cassert>
# include <cstring>
# include <dismod_at/configure.hpp>

// optional
extern bool cholmod_xam(void);

// mixed_cppad subdirectory
extern bool mixed_constraint_xam(void);
extern bool mixed_derived_xam(void);
extern bool constraint_eval_xam(void);
extern bool constraint_hes_xam(void);
extern bool constraint_jac_xam(void);
extern bool data_mismatch_xam(void);
extern bool d_logdet_xam(void);
extern bool d_ran_like_xam(void);
extern bool eigen_xam(void);
extern bool fix_like_hes_xam(void);
extern bool fix_like_jac_xam(void);
extern bool hes_cross_xam(void);
extern bool hes_ran_0_xam(void);
extern bool h_ran_like_xam(void);
extern bool ipopt_xam_run(void);
extern bool newton_step_xam(void);
extern bool no_random_xam(void);
extern bool optimize_fixed_xam(void);
extern bool optimize_random_xam(void);
extern bool prior_eval_xam(void);
extern bool ran_like_grad_xam(void);
extern bool ran_like_hes_xam(void);
extern bool ran_obj_beta_xam(void);
extern bool ran_obj_eval_xam(void);
extern bool ran_obj_hes_fix_xam(void);

// model subdirectory
extern bool avg_no_ode_xam(void);
extern bool avg_yes_ode_xam(void);
extern bool fit_model_xam(void);
extern bool like_all_xam(void);
extern bool like_one_xam(void);
extern bool prior_fixed_xam(void);
extern bool prior_random_xam(void);

// utility subdirectory
extern bool avgint_subset_xam(void);
extern bool bilinear_interp_xam(void);
extern bool child_info_xam(void);
extern bool data_subset_xam(void);
extern bool eigen_ode2_xam(void);
extern bool fixed_effect_xam(void);
extern bool integrate_1d_xam(void);
extern bool integrate_2d_xam(void);
extern bool interp_weight_xam(void);
extern bool manage_gsl_rng_xam(void);
extern bool pack_info_xam(void);
extern bool pack_diff_prior_xam(void);
extern bool pack_value_prior_xam(void);
extern bool random_effect_xam(void);
extern bool residual_density_xam(void);
extern bool sim_random_xam(void);
extern bool smooth2ode_xam(void);
extern bool solve_ode_xam(void);
extern bool to_string_xam(void);
extern bool variable_name_xam(void);

// table subdirectory
extern bool check_pini_n_age_xam(void);
extern bool create_table_xam(void);
extern bool get_age_table_xam(void);
extern bool get_avgint_table_xam(void);
extern bool get_option_table_xam(void);
extern bool get_column_max_xam(void);
extern bool get_covariate_table_xam(void);
extern bool get_data_table_xam(void);
extern bool get_density_table_xam(void);
extern bool get_integrand_table_xam(void);
extern bool get_prior_table_xam(void);
extern bool get_mulcov_table_xam(void);
extern bool get_node_table_xam(void);
extern bool get_rate_table_xam(void);
extern bool get_sample_table_xam(void);
extern bool get_simulate_table_xam(void);
extern bool get_smooth_grid_xam(void);
extern bool get_table_column_xam(void);
extern bool get_time_table_xam(void);
extern bool get_weight_grid_xam(void);
extern bool put_table_row_xam(void);
extern bool smooth_info_xam(void);
extern bool weight_info_xam(void);

// run last
extern bool capture_xam(void);

// anonymous namespace
namespace {
	// function that runs one test
	static size_t Run_ok_count    = 0;
	static size_t Run_error_count = 0;
	void Run(bool test_fun(void), const char* test_name)
	{	using std::cout;
		using std::endl;
		//
		std::streamsize width = 30;
		cout.width( width );
		cout.setf( std::ios_base::left );
		cout << test_name << ':';
		assert( std::strlen(test_name) < size_t(width) );
		//
		bool ok = test_fun();
		if( ok )
		{	cout << "OK" << endl;
			Run_ok_count++;
		}
		else
		{	cout << "Error" << endl;
			Run_error_count++;
		}
	}
}
// macro for calls Run
# define RUN(test_name) Run( test_name, #test_name )

// main program that runs all the tests
int main(void)
{
# if DISMOD_AT_HAS_SUITESPARSE
	RUN(cholmod_xam);
# endif
	// mixed_cppad subdirectory
	RUN(mixed_constraint_xam);
	RUN(mixed_derived_xam);
	RUN(constraint_eval_xam);
	RUN(constraint_hes_xam);
	RUN(constraint_jac_xam);
	RUN(data_mismatch_xam);
	RUN(d_logdet_xam);
	RUN(d_ran_like_xam);
	RUN(eigen_xam);
	RUN(fix_like_hes_xam);
	RUN(fix_like_jac_xam);
	RUN(hes_cross_xam);
	RUN(hes_ran_0_xam);
	RUN(h_ran_like_xam);
	RUN(ipopt_xam_run);
	RUN(newton_step_xam);
	RUN(ran_like_grad_xam);
	RUN(ran_like_hes_xam);
	RUN(no_random_xam);
	RUN(optimize_fixed_xam);
	RUN(optimize_random_xam);
	RUN(prior_eval_xam);

	// model subdirectory
	RUN(avg_no_ode_xam);
	RUN(avg_yes_ode_xam);
	RUN(fit_model_xam);
	RUN(like_all_xam);
	RUN(like_one_xam);
	RUN(prior_fixed_xam);
	RUN(prior_random_xam);

	// utility subdirectory
	RUN(avgint_subset_xam);
	RUN(bilinear_interp_xam);
	RUN(child_info_xam);
	RUN(data_subset_xam);
	RUN(eigen_ode2_xam);
	RUN(fixed_effect_xam);
	RUN(integrate_1d_xam);
	RUN(integrate_2d_xam);
	RUN(interp_weight_xam);
	RUN(manage_gsl_rng_xam);
	RUN(pack_info_xam);
	RUN(pack_diff_prior_xam);
	RUN(pack_value_prior_xam);
	RUN(residual_density_xam);
	RUN(random_effect_xam);
	RUN(sim_random_xam);
	RUN(smooth2ode_xam);
	RUN(solve_ode_xam);
	RUN(to_string_xam);
	RUN(variable_name_xam);

	// table subdirectory
	RUN(check_pini_n_age_xam);
	RUN(create_table_xam);
	RUN(get_age_table_xam);
	RUN(get_avgint_table_xam);
	RUN(get_option_table_xam);
	RUN(get_column_max_xam);
	RUN(get_covariate_table_xam);
	RUN(get_data_table_xam);
	RUN(get_density_table_xam);
	RUN(get_integrand_table_xam);
	RUN(get_prior_table_xam);
	RUN(get_mulcov_table_xam);
	RUN(get_node_table_xam);
	RUN(get_rate_table_xam);
	RUN(get_sample_table_xam);
	RUN(get_simulate_table_xam);
	RUN(get_smooth_grid_xam);
	RUN(get_table_column_xam);
	RUN(get_time_table_xam);
	RUN(get_weight_grid_xam);
	RUN(put_table_row_xam);
	RUN(smooth_info_xam);
	RUN(weight_info_xam);

# if ! DISMOD_AT_BFGS
	RUN(ran_obj_eval_xam);
	RUN(ran_obj_beta_xam);
	RUN(ran_obj_hes_fix_xam);
# endif

	// summary report
	using std::cout;
	using std::endl;
	int return_flag;
	if( Run_error_count == 0 )
	{	cout << "All " << Run_ok_count << " tests passed." << endl;
		return_flag = 0;
	}
	else
	{	cout << Run_error_count << " tests failed." << endl;
		return_flag = 1;
	}

	// BEGIN capture_xam
	if( ! capture_xam() )
		return_flag = 1;
	// END capture_xam

	return return_flag;
}
