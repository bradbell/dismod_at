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
$begin cpp.cpp$$

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

// approx_mixed subdirectory
extern bool cholmod_xam(void);
extern bool eigen_xam(void);
extern bool hessian_random_xam(void);
extern bool optimize_random_xam(void);

// model subdirectory
extern bool avg_no_ode_xam(void);
extern bool avg_yes_ode_xam(void);
extern bool like_all_xam(void);
extern bool like_one_xam(void);
extern bool prior_fixed_xam(void);
extern bool prior_random_xam(void);

// utility subdirectory
extern bool bilinear_interp_xam(void);
extern bool child_data_xam(void);
extern bool eigen_ode2_xam(void);
extern bool fixed_effect_xam(void);
extern bool pack_info_xam(void);
extern bool integrate_1d_xam(void);
extern bool integrate_2d_xam(void);
extern bool interp_weight_xam(void);
extern bool random_effect_xam(void);
extern bool residual_density_xam(void);
extern bool smooth2ode_xam(void);
extern bool solve_ode_xam(void);

// table subdirectory
extern bool check_pini_n_age_xam(void);
extern bool get_age_table_xam(void);
extern bool get_covariate_table_xam(void);
extern bool get_data_table_xam(void);
extern bool get_density_table_xam(void);
extern bool get_integrand_table_xam(void);
extern bool get_prior_table_xam(void);
extern bool get_mulcov_table_xam(void);
extern bool get_node_table_xam(void);
extern bool get_rate_table_xam(void);
extern bool get_run_table_xam(void);
extern bool get_smooth_grid_xam(void);
extern bool get_table_column_xam(void);
extern bool get_time_table_xam(void);
extern bool get_weight_grid_xam(void);
extern bool smooth_info_xam(void);
extern bool weight_info_xam(void);

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
	// approx_mixed subdirectory
	RUN(cholmod_xam);
	RUN(eigen_xam);
	RUN(hessian_random_xam);
	RUN(optimize_random_xam);

	// model subdirectory
	RUN(avg_no_ode_xam);
	RUN(avg_yes_ode_xam);
	RUN(like_all_xam);
	RUN(like_one_xam);
	RUN(prior_fixed_xam);
	RUN(prior_random_xam);

	// utility subdirectory
	RUN(bilinear_interp_xam);
	RUN(child_data_xam);
	RUN(eigen_ode2_xam);
	RUN(fixed_effect_xam);
	RUN(integrate_1d_xam);
	RUN(integrate_2d_xam);
	RUN(interp_weight_xam);
	RUN(pack_info_xam);
	RUN(residual_density_xam);
	RUN(random_effect_xam);
	RUN(smooth2ode_xam);
	RUN(solve_ode_xam);

	// table subdirectory
	RUN(check_pini_n_age_xam);
	RUN(get_age_table_xam);
	RUN(get_covariate_table_xam);
	RUN(get_data_table_xam);
	RUN(get_density_table_xam);
	RUN(get_integrand_table_xam);
	RUN(get_prior_table_xam);
	RUN(get_mulcov_table_xam);
	RUN(get_node_table_xam);
	RUN(get_rate_table_xam);
	RUN(get_run_table_xam);
	RUN(get_smooth_grid_xam);
	RUN(get_table_column_xam);
	RUN(get_time_table_xam);
	RUN(get_weight_grid_xam);
	RUN(smooth_info_xam);
	RUN(weight_info_xam);

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
	return return_flag;
}
