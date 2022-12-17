// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin avg_yes_ode_xam.cpp$$
$spell
   interp
   xam
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section C++ avg_yes_ode: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/data_model.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/age_avg_grid.hpp>

bool avg_yes_ode_xam(void)
{  bool   ok = true;
   using CppAD::abs;
   using CppAD::vector;
   using std::cout;
   typedef CppAD::AD<double> Float;
   //
   // ode_step_size
   double ode_step_size = 3.0;
   //
   // age_table
   // (make sure that ode grid lands on last age table point)
   double age = 0.0;
   vector<double> age_table;
   age_table.push_back(age);
   while( age < 100. )
   {  age += ode_step_size;
      age_table.push_back(age);
   }
   size_t n_age_table = age_table.size();
   //
   // time_table
   // (make sure that ode grid lands on last time table point)
   double time = 1980.0;
   vector<double> time_table;
   time_table.push_back(time);
   while( time < 2020.0 )
   {  time += ode_step_size;
      time_table.push_back(time);
   }
   size_t n_time_table = time_table.size();
   //
   // density_table
   vector<dismod_at::density_enum> density_table(7);
   density_table[0] = dismod_at::uniform_enum;
   density_table[1] = dismod_at::gaussian_enum;
   density_table[2] = dismod_at::laplace_enum;
   density_table[3] = dismod_at::students_enum;
   density_table[4] = dismod_at::log_gaussian_enum;
   density_table[5] = dismod_at::log_laplace_enum;
   density_table[6] = dismod_at::log_students_enum;

   // age and time smoothing grid indices
   size_t n_age_si   = 3;
   size_t n_time_si  = 2;
   vector<size_t> age_id(n_age_si), time_id(n_time_si);
   age_id[0]   = 0;
   age_id[1]   = n_age_table / 2;
   age_id[2]   = n_age_table - 1;
   time_id[0]  = 0;
   time_id[1]  = n_time_table - 1;
   //
   // w_info_vec
   // weight value should not matter when constant
   size_t n_si = n_age_si * n_time_si;
   vector<double> weight(n_si);
   for(size_t k = 0; k < n_si; k++)
      weight[k] = 0.5;
   dismod_at::weight_info w_info(
      age_table, time_table, age_id, time_id, weight
   );
   vector<dismod_at::weight_info> w_info_vec(2);
   w_info_vec[0] = w_info;
   //
   // prior table
   double inf = std::numeric_limits<double>::infinity();
   double nan = std::numeric_limits<double>::quiet_NaN();
   vector<dismod_at::prior_struct> prior_table(1);
   prior_table[0].prior_name = "prior_zero";
   prior_table[0].density_id = 0;
   prior_table[0].lower      = -inf;
   prior_table[0].upper      = +inf;
   prior_table[0].mean       = 0.0;
   prior_table[0].std        = nan;
   prior_table[0].eta        = nan;
   //
   // s_info_vec
   vector<dismod_at::smooth_info> s_info_vec(2);
   size_t mulstd_value = 1, mulstd_dage = 1, mulstd_dtime = 1;
   bool all_const_value = false;
   for(size_t smooth_id = 0; smooth_id < 2; smooth_id++)
   {  vector<size_t> age_id_tmp;
      if( smooth_id == 0 )
      {  n_si       = n_age_si * n_time_si;
         age_id_tmp = age_id;
      }
      else
      {  n_si = n_time_si;
         age_id_tmp.resize(1);
         age_id_tmp[0] = 0;
      }
      //
      vector<size_t> value_prior_id(n_si),
         dage_prior_id(n_si), dtime_prior_id(n_si);
      for(size_t i = 0; i < n_si; i++)
         value_prior_id[i] = 0;
      vector<double> const_value(n_si);
      for(size_t i = 0; i < n_si; ++i)
         const_value[i] = std::numeric_limits<double>::quiet_NaN();
      dismod_at::smooth_info s_info(
         age_table, time_table, age_id_tmp, time_id,
         value_prior_id, dage_prior_id, dtime_prior_id, const_value,
         mulstd_value, mulstd_dage, mulstd_dtime, all_const_value
      );
      s_info_vec[smooth_id] = s_info;
   }
   //
   // integrand_id = number_integrand - integrand_enum - 1
   size_t n_integrand = dismod_at::number_integrand_enum;
   vector<dismod_at::integrand_struct> integrand_table(n_integrand);
   for(size_t integrand_id = 0; integrand_id < n_integrand; integrand_id++)
   {  integrand_table[integrand_id].integrand =
         dismod_at::integrand_enum(n_integrand - integrand_id - 1);
      integrand_table[integrand_id].minimum_meas_cv = 0.0;
   }
   //
   // node_table:
   CppAD::vector<dismod_at::node_struct> node_table(3);
   node_table[0].parent = DISMOD_AT_NULL_INT; // node zero has two children
   node_table[1].parent = 0;
   node_table[2].parent = 0;
   //
   // parent_node_id
   size_t parent_node_id = 0;
   //
   // covariate table
   size_t n_covariate = 0;
   vector<dismod_at::covariate_struct> covariate_table(n_covariate);
   //
   // data_table
   vector<dismod_at::data_struct> data_table(3);
   vector<double> data_cov_value(data_table.size() * n_covariate);
   size_t data_id = 0;
   data_table[data_id].integrand_id =
      int(n_integrand) - int(dismod_at::susceptible_enum) - 1;
   data_table[data_id].node_id      = 1; // child node
   data_table[data_id].weight_id    = 0;
   data_table[data_id].age_lower    = 0.0;
   data_table[data_id].age_upper    = 100.0;
   data_table[data_id].time_lower   = 1990.0;
   data_table[data_id].time_upper   = 2000.0;
   data_table[data_id].meas_value   = 0.0;
   data_table[data_id].meas_std     = 1e-3;
   data_table[data_id].eta          = 1e-6;
   data_table[data_id].density_id   = dismod_at::uniform_enum;
   //
   data_id = 1;
   data_table[data_id]              = data_table[0];
   data_table[data_id].age_lower    = 10.;
   data_table[data_id].age_upper    = 90.0;
   data_table[data_id].time_lower   = 1990.0;
   data_table[data_id].integrand_id =
      int(n_integrand) - int(dismod_at::withC_enum) - 1;
   //
   data_id = 2;
   data_table[data_id]              = data_table[0];
   data_table[data_id].age_lower    = 30.;
   data_table[data_id].age_upper    = 60.0;
   data_table[data_id].time_lower   = 1990.0;
   data_table[data_id].integrand_id =
      int(n_integrand) - int(dismod_at::prevalence_enum) - 1;
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
   // mulcov_table
   vector<dismod_at::mulcov_struct> mulcov_table(0);
   // rate_table
   vector<dismod_at::rate_struct>   rate_table(dismod_at::number_rate_enum);
   for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
   {  size_t smooth_id = 0;
      if( rate_id == dismod_at::pini_enum )
         smooth_id = 1; // only one age
      rate_table[rate_id].parent_smooth_id = int( smooth_id );
      rate_table[rate_id].child_smooth_id  = int( smooth_id );
      rate_table[rate_id].child_nslist_id  = DISMOD_AT_NULL_INT;
   }
   // child_info
   dismod_at::child_info child_info4data(
      parent_node_id ,
      node_table     ,
      data_table
   );
   size_t n_child = child_info4data.child_size();
   assert( n_child == 2 );
   // pack_object
   // values in child_id2node_id do not matter because child_nslist_id is null
   vector<size_t> child_id2node_id(n_child);
   vector<dismod_at::nslist_pair_struct> nslist_pair(0);
   dismod_at::pack_info pack_object(
      n_integrand,
      child_id2node_id,
      subgroup_table,
      smooth_table,
      mulcov_table,
      rate_table,
      nslist_pair
   );
   // subset_data
   vector<dismod_at::subset_data_struct> subset_data_obj;
   vector<double> subset_data_cov_value;
   std::map<std::string, std::string> option_map;
   vector<dismod_at::data_subset_struct> data_subset_table(data_table.size());
   for(size_t i = 0; i < data_table.size(); ++i)
   {  data_subset_table[i].data_id = int(i);
      data_subset_table[i].hold_out = 0;
   }
   subset_data(
      option_map,
      data_subset_table,
      integrand_table,
      density_table,
      data_table,
      data_cov_value,
      covariate_table,
      child_info4data,
      subset_data_obj,
      subset_data_cov_value
   );
   //
   // data_model
   double bound_random = std::numeric_limits<double>::infinity();
   bool        fit_simulated_data = false;
   std::string meas_noise_effect = "add_std_scale_all";
   std::string rate_case       = "iota_pos_rho_pos";
   std::string age_avg_split   = "";
   vector<double> age_avg_grid = dismod_at::age_avg_grid(
      ode_step_size, age_avg_split, age_table
   );
   dismod_at::data_model data_object(
      fit_simulated_data,
      meas_noise_effect,
      rate_case,
      bound_random,
      n_covariate,
      ode_step_size,
      age_avg_grid,
      age_table,
      time_table,
      subgroup_table,
      integrand_table,
      mulcov_table,
      prior_table,
      subset_data_obj,
      subset_data_cov_value,
      w_info_vec,
      s_info_vec,
      pack_object,
      child_info4data
   );
   //
   // pack_vec
   double beta_parent   = 0.01;
   double random_effect = log(2.0);
   double beta          = beta_parent * exp( random_effect );
   vector<Float> pack_vec( pack_object.size() );
   dismod_at::pack_info::subvec_info info;
   size_t n_rate = dismod_at::number_rate_enum;
   for(size_t child_id = 0; child_id <= n_child; child_id++)
   {  for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
      {  info = pack_object.node_rate_value_info(rate_id, child_id);
         for(size_t k = 0; k < info.n_var; k++)
         {  if( rate_id == size_t(dismod_at::iota_enum) )
            {  if( child_id == n_child )
                  pack_vec[info.offset + k] = beta_parent;
               else
                  pack_vec[info.offset + k] = random_effect;
            }
            else
               pack_vec[info.offset + k] = 0.00;
         }
      }
   }
   /*
   No remission and no excess mortality so solution is:
   S(a) = exp( -beta * a )
   P(a) = C(a) = 1.0 - exp( -beta * a )

   int_b^c S(a) / (c - b) = - [ exp( -beta a ) / beta ]_b^c / (c - b)
   int_b^c C(a) / (c - b) = 1.0 - int_b^c S(a) / (c - b)
   int_b^c P(a) / (c - b) = 1.0 - int_b^c S(a) / (c - b)
   */
   using CppAD::exp;
   ok            &= data_table.size() == subset_data_obj.size();
   //
   data_id = 0;
   Float avg      = data_object.average(data_id, pack_vec);
   double b       = data_table[data_id].age_lower;
   double c       = data_table[data_id].age_upper;
   double avg_S   = - ( exp(-beta * c) - exp(-beta * b) ) / (beta * (c - b));
   ok             &= fabs( 1.0 - avg / avg_S ) <= 1e-3;
   //
   data_id = 1;
   avg            = data_object.average(data_id, pack_vec);
   b              = data_table[data_id].age_lower;
   c              = data_table[data_id].age_upper;
   avg_S          = - ( exp(-beta * c) - exp(-beta * b) ) / (beta * (c - b));
   double avg_C   = 1.0 - avg_S;
   ok             &= fabs( 1.0 - avg / avg_C ) <= 1e-3;
   //
   data_id = 2;
   avg            = data_object.average(data_id, pack_vec);
   b              = data_table[data_id].age_lower;
   c              = data_table[data_id].age_upper;
   avg_S          = - ( exp(-beta * c) - exp(-beta * b) ) / (beta * (c - b));
   double avg_P   = 1.0 - avg_S;
   ok             &= fabs( 1.0 - avg / avg_P ) <= 1e-3;
   return ok;
}
// END C++
