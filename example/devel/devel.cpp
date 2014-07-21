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


// prototype for each of the tests
extern bool eigen_ode2_xam(void);
extern bool integrate_1d_xam(void);
extern bool integrate_2d_xam(void);
extern bool interp_weight_xam(void);
extern bool smooth2ode_xam(void);
//
extern bool get_age_table_xam(void);
extern bool get_covariate_table_xam(void);
extern bool get_data_table_xam(void);
extern bool get_density_table_xam(void);
extern bool get_integrand_table_xam(void);
extern bool get_like_table_xam(void);
extern bool get_multiplier_table_xam(void);
extern bool get_node_table_xam(void);
extern bool get_rate_prior_xam(void);
extern bool get_rate_table_xam(void);
extern bool get_run_table_xam(void);
extern bool get_smooth_grid_xam(void);
extern bool get_table_column_xam(void);
extern bool get_time_table_xam(void);
extern bool get_weight_grid_xam(void);
extern bool smooth_grid_xam(void);
extern bool weight_grid_xam(void);

// anonymous namespace
# include <dismod_at/dismod_at.hpp>

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
	RUN(eigen_ode2_xam);
	RUN(integrate_1d_xam);
	RUN(integrate_2d_xam);
	RUN(interp_weight_xam);
	RUN(smooth2ode_xam);
	//
	RUN(get_age_table_xam);
	RUN(get_covariate_table_xam);
	RUN(get_data_table_xam);
	RUN(get_density_table_xam);
	RUN(get_integrand_table_xam);
	RUN(get_like_table_xam);
	RUN(get_multiplier_table_xam);
	RUN(get_node_table_xam);
	RUN(get_rate_prior_xam);
	RUN(get_rate_table_xam);
	RUN(get_run_table_xam);
	RUN(get_smooth_grid_xam);
	RUN(get_table_column_xam);
	RUN(get_time_table_xam);
	RUN(get_weight_grid_xam);
	RUN(smooth_grid_xam);
	RUN(weight_grid_xam);

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
