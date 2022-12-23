// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin adj_integrand_xam.cpp dev}
{xrst_spell
   adj
}

C++ adj_integrand: Example and Test
###################################

{xrst_literal
   // BEGIN C++
   // END C++
}

{xrst_end adj_integrand_xam.cpp}
*/
// BEGIN C++
# include <dismod_at/adj_integrand.hpp>
# include <dismod_at/null_int.hpp>

bool adj_integrand_xam(void)
{  bool   ok = true;
   using CppAD::abs;
   using CppAD::vector;
   using std::cout;
   typedef double Float;
   //
   // rate_case
   std::string rate_case = "iota_pos_rho_zero";
   //
   // age_table
   size_t n_age_table = 6;
   double age_min     = 20.0;
   double age_max     = 100.0;
   double d_age       = (age_max - age_min) / double(n_age_table - 1);
   vector<double> age_table(n_age_table);
   for(size_t i = 0; i < n_age_table; ++i)
      age_table[i] = age_min + double(i) * d_age;
   //
   // time_table
   size_t n_time_table = 4;
   double time_min     = 1990.0;
   double time_max     = 2010.0;
   double d_time       = (time_max - time_min) / double(n_time_table - 1);
   vector<double> time_table(n_time_table);
   for(size_t i = 0; i < n_time_table; ++i)
      time_table[i] = time_min + double(i) * d_time;
   //
   // age_si_id
   vector<size_t> age_si_id(1);
   age_si_id[0] = 1;
   //
   // time_si_id
   vector<size_t> time_si_id(1);
   time_si_id[0] = n_time_table - 2;

   // priors are not used
   bool all_const_value = true;
   vector<size_t> value_prior_id(1);
   vector<size_t> dage_prior_id(1);
   vector<size_t> dtime_prior_id(1);
   vector<double> const_value;
   size_t mulstd_value   = 0;
   size_t mulstd_dage    = 0;
   size_t mulstd_dtime   = 0;

   // s_info: testing constructor
   dismod_at::smooth_info s_info(
      age_table,
      time_table,
      age_si_id,
      time_si_id,
      value_prior_id,
      dage_prior_id,
      dtime_prior_id,
      const_value,
      mulstd_value,
      mulstd_dage,
      mulstd_dtime,
      all_const_value
   );
   //
   // s_info_vec
   vector<dismod_at::smooth_info> s_info_vec(1);
   s_info_vec[0] = s_info;
   //
   // integrand_table
   size_t n_integrand = 1;
   vector<dismod_at::integrand_struct> integrand_table(n_integrand);
   size_t integrand_id = 0;
   integrand_table[integrand_id].integrand = dismod_at::withC_enum;
   //
   // number of children
   size_t n_child = 0;
   //
   // index for parent
   size_t child   = n_child;
   //
   // x
   vector<double> x(0);
   //
   // cohort_age, cohort_time
   size_t n_cohort = 7;
   vector<double> cohort_age(n_cohort), cohort_time(n_cohort);
   double d_cohort = (age_max - age_min) / double(n_cohort - 1);
   for(size_t i = 0; i < n_cohort; ++i)
   {  cohort_age[i]  = age_min  + double(i) * d_cohort;
      cohort_time[i] = time_min + double(i) * d_cohort;
   }
   //
   // child_id2node_id
   vector<size_t> child_id2node_id(n_child);
   //
   // subgroup_table
   size_t n_subgroup = 1;
   vector<dismod_at::subgroup_struct> subgroup_table(n_subgroup);
   subgroup_table[0].subgroup_name = "world";
   subgroup_table[0].group_id      = 0;
   subgroup_table[0].group_name    = "world";
   //
   // smooth_table
   vector<dismod_at::smooth_struct> smooth_table(s_info_vec.size());
   for(size_t smooth_id = 0; smooth_id < s_info_vec.size(); smooth_id++)
   {  smooth_table[smooth_id].n_age
         = int( s_info_vec[smooth_id].age_size() );
      smooth_table[smooth_id].n_time
         = int( s_info_vec[smooth_id].time_size() );
   }
   //
   // mulcov_table
   vector<dismod_at::mulcov_struct> mulcov_table(0);
   //
   // rate_table
   vector<dismod_at::rate_struct>   rate_table(dismod_at::number_rate_enum);
   for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
   {  size_t smooth_id = 0;
      rate_table[rate_id].parent_smooth_id = int( smooth_id );
      rate_table[rate_id].child_smooth_id  = DISMOD_AT_NULL_INT;
      rate_table[rate_id].child_nslist_id  = DISMOD_AT_NULL_INT;
   }
   //
   // nslist_pair
   vector<dismod_at::nslist_pair_struct> nslist_pair(0);
   //
   // pack_object
   dismod_at::pack_info pack_object(
      n_integrand,
      child_id2node_id,
      subgroup_table,
      smooth_table,
      mulcov_table,
      rate_table,
      nslist_pair
   );
   /*
   pack_vec: Zero pini, rho, chi, or omega, so solution is:
   S(a) = exp( -beta * (a - age_min) )
   C(a) = 1.0 - exp( -beta * (a - age_min) )
   */
   double beta          = 0.01;
   double random_effect = log(2.0);
   vector<Float> pack_vec( pack_object.size() );
   dismod_at::pack_info::subvec_info info;
   size_t n_rate = dismod_at::number_rate_enum;
   for(size_t child_id = 0; child_id <= n_child; child_id++)
   {  for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
      {  info = pack_object.node_rate_value_info(rate_id, child_id);
         for(size_t k = 0; k < info.n_var; k++)
         {  if( rate_id == size_t(dismod_at::iota_enum) )
            {  if( child_id == n_child )
                  pack_vec[info.offset + k] = beta;
               else
                  pack_vec[info.offset + k] = random_effect;
            }
            else
               pack_vec[info.offset + k] = 0.00;
         }
      }
   }
   //
   // adjust_object
   size_t n_covariate = 0;
   dismod_at::adj_integrand adjint_obj(
      n_covariate,
      rate_case,
      age_table,
      time_table,
      subgroup_table,
      integrand_table,
      mulcov_table,
      s_info_vec,
      pack_object
   );
   size_t subgroup_id = 0;
   vector<Float> adj_line = adjint_obj.line(
      cohort_age,
      cohort_time,
      integrand_id,
      n_child,
      child,
      subgroup_id,
      x,
      pack_vec
   );
   //
   // check result
   double eps99 = 99.0 * std::numeric_limits<double>::epsilon();
   ok &= adj_line.size() == n_cohort;
   for(size_t i = 0; i < n_cohort; ++i)
   {  double a = cohort_age[i];
      double C = 1.0 - std::exp( - beta * (a - age_min) );
      // std::cout << "adj_line = " << adj_line[i];
      // std::cout << ", C = " << C << "\n";
      ok      &= CppAD::NearEqual(adj_line[i], C, eps99, eps99);
   }
   return ok;
}
// END C++
