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
$begin cpp.cpp$$

$sectin Run C++ Examples$$
$index C++, run examples$$
$index run, C++ examples$$
$index examples, run C++$$

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

// model subdirectory
extern bool data_mean_xam(void);
// utility subdirectory
extern bool avg_integrand_xam(void);
extern bool bilinear_interp_xam(void);
extern bool eigen_ode2_xam(void);
extern bool integrate_1d_xam(void);
extern bool integrate_2d_xam(void);
extern bool interp_weight_xam(void);
extern bool smooth2ode_xam(void);
extern bool pack_var_xam(void);
extern bool child_data_xam(void);
// table subdirectory
extern bool check_rate_smooth_xam(void);
extern bool get_age_table_xam(void);
extern bool get_covariate_table_xam(void);
extern bool get_data_table_xam(void);
extern bool get_density_table_xam(void);
extern bool get_integrand_table_xam(void);
extern bool get_like_table_xam(void);
extern bool get_mulcov_table_xam(void);
extern bool get_node_table_xam(void);
extern bool get_rate_table_xam(void);
extern bool get_run_table_xam(void);
extern bool get_smooth_grid_xam(void);
extern bool get_table_column_xam(void);
extern bool get_time_table_xam(void);
extern bool get_weight_grid_xam(void);
extern bool smooth_info_xam(void);
extern bool solve_ode_xam(void);
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
	// model subdirectory
	RUN(data_mean_xam);
	// utility subdirectory
	RUN(avg_integrand_xam);
	RUN(bilinear_interp_xam);
	RUN(eigen_ode2_xam);
	RUN(integrate_1d_xam);
	RUN(integrate_2d_xam);
	RUN(interp_weight_xam);
	RUN(smooth2ode_xam);
	RUN(solve_ode_xam);
	RUN(pack_var_xam);
	RUN(child_data_xam);
	// table subdirectory
	RUN(check_rate_smooth_xam);
	RUN(get_age_table_xam);
	RUN(get_covariate_table_xam);
	RUN(get_data_table_xam);
	RUN(get_density_table_xam);
	RUN(get_integrand_table_xam);
	RUN(get_like_table_xam);
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
