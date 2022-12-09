// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin example_devel.cpp}
{xrst_spell
   cd
   cmake
   devel
}

Run C++ Examples
################

Syntax
******
``example/devel/example_devel``

Purpose
*******
This runs all the C++ examples and prints out their correctness
test results together with a summary result at the end.

Steps
*****
::

   bin/run_cmake.sh
   cd build
   make example_devel
   example/devel/example_devel

{xrst_end example_devel.cpp}
-----------------------------------------------------------------------------
*/
# include <iostream>
# include <cassert>
# include <cstring>

// model subdirectory
extern bool adj_integrand_xam(void);
extern bool avg_integrand_xam(void);
extern bool avg_no_ode_xam(void);
extern bool avg_noise_effect_xam(void);
extern bool avg_yes_ode_xam(void);
extern bool fit_model_xam(void);
extern bool like_all_xam(void);
extern bool like_one_xam(void);
extern bool prior_fixed_xam(void);
extern bool prior_random_xam(void);

// utility subdirectory
extern bool avgint_subset_xam(void);
extern bool balance_pair_xam(void);
extern bool bilinear_interp_xam(void);
extern bool child_info_xam(void);
extern bool child_data_in_fit_xam(void);
extern bool cohort_ode_xam(void);
extern bool subset_data_xam(void);
extern bool eigen_ode2_xam(void);
extern bool trap_ode2_xam(void);
extern bool fixed_effect_xam(void);
extern bool manage_gsl_rng_xam(void);
extern bool age_avg_grid_xam(void);
extern bool pack_info_xam(void);
extern bool pack_prior_xam(void);
extern bool random_effect_xam(void);
extern bool n_random_const_xam(void);
extern bool residual_density_xam(void);
extern bool sim_random_xam(void);
extern bool grid2line_xam(void);
extern bool split_space_xam(void);
extern bool time_line_vec_xam(void);

// table subdirectory
extern bool get_bnd_mulcov_table_xam(void);
extern bool blob_table_xam(void);
extern bool check_pini_n_age_xam(void);
extern bool create_table_xam(void);
extern bool get_age_table_xam(void);
extern bool get_nslist_table_xam(void);
extern bool get_avgint_table_xam(void);
extern bool get_option_table_xam(void);
extern bool get_column_max_xam(void);
extern bool get_covariate_table_xam(void);
extern bool get_data_table_xam(void);
extern bool get_data_subset_xam(void);
extern bool get_density_table_xam(void);
extern bool get_integrand_table_xam(void);
extern bool get_prior_table_xam(void);
extern bool get_prior_sim_table_xam(void);
extern bool get_mulcov_table_xam(void);
extern bool get_node_table_xam(void);
extern bool get_rate_table_xam(void);
extern bool get_sample_table_xam(void);
extern bool get_data_sim_table_xam(void);
extern bool get_nslist_pair_xam(void);
extern bool get_smooth_grid_xam(void);
extern bool get_table_column_xam(void);
extern bool get_time_table_xam(void);
extern bool get_weight_grid_xam(void);
extern bool get_subgroup_table_xam(void);
extern bool put_table_row_xam(void);
extern bool smooth_info_xam(void);
extern bool weight_info_xam(void);

// anonymous namespace
namespace {
   // function that runs one test
   static size_t Run_ok_count    = 0;
   static size_t Run_error_count = 0;
   void Run(bool test_fun(void), const char* test_name)
   {  using std::cout;
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
      {  cout << "OK" << endl;
         Run_ok_count++;
      }
      else
      {  cout << "Error" << endl;
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
   RUN(adj_integrand_xam);
   RUN(avg_integrand_xam);
   RUN(avg_no_ode_xam);
   RUN(avg_noise_effect_xam);
   RUN(avg_yes_ode_xam);
   RUN(fit_model_xam);
   RUN(like_all_xam);
   RUN(like_one_xam);
   RUN(prior_fixed_xam);
   RUN(prior_random_xam);

   // utility subdirectory
   RUN(avgint_subset_xam);
   RUN(balance_pair_xam);
   RUN(bilinear_interp_xam);
   RUN(child_info_xam);
   RUN(child_data_in_fit_xam);
   RUN(cohort_ode_xam);
   RUN(subset_data_xam);
   RUN(eigen_ode2_xam);
   RUN(trap_ode2_xam);
   RUN(fixed_effect_xam);
   RUN(manage_gsl_rng_xam);
   RUN(age_avg_grid_xam);
   RUN(pack_info_xam);
   RUN(pack_prior_xam);
   RUN(residual_density_xam);
   RUN(random_effect_xam);
   RUN(n_random_const_xam);
   RUN(sim_random_xam);
   RUN(grid2line_xam);
   RUN(split_space_xam);
   RUN(time_line_vec_xam);

   // table subdirectory
   RUN(get_bnd_mulcov_table_xam);
   RUN(blob_table_xam);
   RUN(check_pini_n_age_xam);
   RUN(create_table_xam);
   RUN(get_age_table_xam);
   RUN(get_nslist_table_xam);
   RUN(get_avgint_table_xam);
   RUN(get_option_table_xam);
   RUN(get_column_max_xam);
   RUN(get_covariate_table_xam);
   RUN(get_data_table_xam);
   RUN(get_data_subset_xam);
   RUN(get_density_table_xam);
   RUN(get_integrand_table_xam);
   RUN(get_prior_table_xam);
   RUN(get_prior_sim_table_xam);
   RUN(get_mulcov_table_xam);
   RUN(get_node_table_xam);
   RUN(get_rate_table_xam);
   RUN(get_sample_table_xam);
   RUN(get_data_sim_table_xam);
   RUN(get_smooth_grid_xam);
   RUN(get_nslist_pair_xam);
   RUN(get_table_column_xam);
   RUN(get_time_table_xam);
   RUN(get_weight_grid_xam);
   RUN(get_subgroup_table_xam);
   RUN(put_table_row_xam);
   RUN(smooth_info_xam);
   RUN(weight_info_xam);

   // summary report
   using std::cout;
   using std::endl;
   int return_flag;
   if( Run_error_count == 0 )
   {  cout << "All " << Run_ok_count << " tests passed." << endl;
      return_flag = 0;
   }
   else
   {  cout << Run_error_count << " tests failed." << endl;
      return_flag = 1;
   }

   return return_flag;
}
