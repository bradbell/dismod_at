// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin avg_integrand_xam.cpp dev}

C++ avg_integrand: Example and Test
###################################

{xrst_literal
   // BEGIN C++
   // END C++
}

{xrst_end avg_integrand_xam.cpp}
*/
// BEGIN C++
# include <dismod_at/avg_integrand.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/get_weight_table.hpp>

bool avg_integrand_xam(void)
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
   double age_ini     = 20.0;
   double age_end     = 100.0;
   double d_age       = (age_end - age_ini) / double(n_age_table - 1);
   vector<double> age_table(n_age_table);
   for(size_t i = 0; i < n_age_table; ++i)
      age_table[i] = age_ini + double(i) * d_age;
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
   // age_grid_id
   vector<size_t> age_grid_id(1);
   age_grid_id[0] = 1;
   //
   // time_grid_id
   vector<size_t> time_grid_id(1);
   time_grid_id[0] = n_time_table - 2;

   // w_info
   vector<double> weight(1);
   weight[0] = 2.0;
   dismod_at::weight_info w_info(
      age_table, time_table, age_grid_id, time_grid_id, weight
   );

   //
   // w_info_vec
   vector<dismod_at::weight_info> w_info_vec(2);
   w_info_vec[0] = w_info;

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
      age_grid_id,
      time_grid_id,
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
   size_t n_integrand = 2;
   vector<dismod_at::integrand_struct> integrand_table(n_integrand);
   integrand_table[0].integrand = dismod_at::Sincidence_enum;
   integrand_table[1].integrand = dismod_at::susceptible_enum;
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
   pack_vec: pini=0, rho=0, chi=0, omega=0, iota=beta:
   S(a) = exp( -beta * (a - age_ini) )
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
   // ode_step_size
   size_t n_ode_age     = 10;
   double ode_step_size = (age_end - age_ini) / double(n_ode_age - 1);
   //
   // age_avg_grid
   vector<double> age_avg_grid(n_ode_age);
   for(size_t i = 0; i < n_ode_age; ++i)
      age_avg_grid[i] = age_ini + double(i) * ode_step_size;
   //
   // avgint_obj
   dismod_at::avg_integrand avgint_obj(
      ode_step_size,
      rate_case,
      age_table,
      time_table,
      age_avg_grid,
      subgroup_table,
      integrand_table,
      mulcov_table,
      w_info_vec,
      s_info_vec,
      pack_object
   );
   double age_lower    = age_ini + 10.0;
   double age_upper    = age_end - 10.0;
   double time_lower   = 2000.0;
   double time_upper   = 2010.0;
   size_t weight_id    = 0;
   size_t subgroup_id  = 0;
   // -----------------------------------------------------------------------
   // Sincidence
   size_t integrand_id = 0;
   Float avg = avgint_obj.rectangle(
      age_lower,
      age_upper,
      time_lower,
      time_upper,
      weight_id,
      integrand_id,
      n_child,
      child,
      subgroup_id,
      x,
      pack_vec
   );
   // check result
   double eps99 = 99.0 * std::numeric_limits<double>::epsilon();
   ok          &= CppAD::NearEqual(avg, beta, eps99, eps99);
   // -----------------------------------------------------------------------
   // susceptible
   integrand_id = 1;
   avg = avgint_obj.rectangle(
      age_lower,
      age_upper,
      time_lower,
      time_upper,
      weight_id,
      integrand_id,
      n_child,
      child,
      subgroup_id,
      x,
      pack_vec
   );
   //
   // check result
   // S(a)        = exp( -beta * (a - age_ini) )
   // int S(a) da = - exp( -beta * (a - age_ini) ) / beta
   double low_int = - exp( - beta * ( age_lower - age_ini ) ) / beta;
   double up_int  = - exp( - beta * ( age_upper - age_ini ) ) / beta;
   double check   = (up_int - low_int) / (age_upper - age_lower );
   ok            &= CppAD::NearEqual(avg, check, 1e-2, 1e-2);
   //
   return ok;
}
// END C++
