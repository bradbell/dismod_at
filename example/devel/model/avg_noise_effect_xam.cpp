// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------

/*
$begin avg_noise_effect_xam.cpp$$
$spell
   std
   xam
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section C++ avg_noise_effect: Example and Test$$

$srcthisfile%
   0%// BEGIN C++%// END C++%1
%$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/avg_noise_effect.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/age_avg_grid.hpp>

namespace {
   double age_max_, time_max_, x_j = 0.5;
   double mulcov(double age, double time)
   {  double a = age / age_max_;
      double t = time / time_max_;
      double r = 0.1 + a + t + a * t;
      return r;
   }
   double avg_effect(
      double a0 ,
      double a1 ,
      double t0 ,
      double t1 )
   {  double sum = 0;
      sum += x_j * mulcov(a0, t0);
      sum += x_j * mulcov(a0, t1);
      sum += x_j * mulcov(a1, t0);
      sum += x_j * mulcov(a1, t1);
      return sum / 4.0;
   }
}
bool avg_noise_effect_xam(void)
{  bool   ok = true;
   using CppAD::vector;
   using std::cout;
   typedef CppAD::AD<double> Float;
   Float eps99 = 99.0 * CppAD::numeric_limits<Float>::epsilon();
   //
   // ode_step_size
   double ode_step_size = 10.0;
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
   double age_min     = age_table[0];
   age_max_           = age_table[n_age_table - 1];
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
   double time_min     = time_table[0];
   time_max_           = time_table[n_time_table - 1];

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
   // integrand_table
   size_t n_integrand = dismod_at::number_integrand_enum;
   vector<dismod_at::integrand_struct> integrand_table(n_integrand);
   for(size_t i = 0; i < n_integrand; i++)
   {  integrand_table[i].integrand       = dismod_at::integrand_enum(i);
      integrand_table[i].minimum_meas_cv = 0.0;
   }
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
   {  smooth_table[smooth_id].n_age  =
         int( s_info_vec[smooth_id].age_size() );
      smooth_table[smooth_id].n_time =
         int( s_info_vec[smooth_id].time_size() );
   }
   // mul_cov
   vector<dismod_at::mulcov_struct> mulcov_table(1);
   mulcov_table[0].mulcov_type  = dismod_at::meas_noise_enum;
   mulcov_table[0].rate_id      =  int( DISMOD_AT_NULL_INT );
   mulcov_table[0].integrand_id =  int( dismod_at::mtother_enum );
   mulcov_table[0].covariate_id =  0;
   mulcov_table[0].group_smooth_id    =  0;

   // rate_table
   vector<dismod_at::rate_struct>   rate_table(dismod_at::number_rate_enum);
   for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
   {  size_t smooth_id = 0;
      if( rate_id == dismod_at::pini_enum )
         smooth_id = 1; // only one age
      rate_table[rate_id].parent_smooth_id =  int( smooth_id );
      rate_table[rate_id].child_smooth_id =   int( smooth_id );
      rate_table[rate_id].child_nslist_id =  int( DISMOD_AT_NULL_INT );
   }

   // pack_object
   // values in child_id2node_id do not matter because child_nslist_id is null
   vector<size_t> child_id2node_id(0);
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

   // avg_noise_obj
   std::string age_avg_split = "";
   vector<double> age_avg_grid = dismod_at::age_avg_grid(
      ode_step_size, age_avg_split, age_table
   );
   dismod_at::avg_noise_effect avg_noise_obj(
      ode_step_size,
      age_avg_grid,
      age_table,
      time_table,
      subgroup_table,
      integrand_table,
      w_info_vec,
      s_info_vec,
      pack_object
   );
   // x
   vector<double> x(1);
   x[0] = x_j;
   //
   // pack_vec
   vector<Float> pack_vec( pack_object.size() );
   size_t mtother_id = dismod_at::mtother_enum;
   dismod_at::pack_info::subvec_info info =
      pack_object.group_meas_noise_info(mtother_id, 0);
   dismod_at::smooth_info& s_info = s_info_vec[info.smooth_id];
   for(size_t i = 0; i < s_info.age_size(); i++)
   {  age = age_table[ s_info.age_id(i) ];
      for(size_t j = 0; j < s_info.time_size(); j++)
      {  time           = time_table[ s_info.time_id(j) ];
         size_t index   = info.offset + i * s_info.time_size() + j;
         pack_vec[index] = mulcov(age, time);
      }
   }

   // evaluate average effect
   double age_lower       = age_min + ode_step_size;
   double age_upper       = age_lower + 2.0 * ode_step_size;
   double time_lower      = time_min;
   double time_upper      = time_lower + ode_step_size;
   size_t weight_id       = 0;
   size_t subgroup_id     = 0;
   size_t integrand_id    =  int( dismod_at::mtother_enum );
   Float avg = avg_noise_obj.rectangle(
      age_lower,
      age_upper,
      time_lower,
      time_upper,
      weight_id,
      subgroup_id,
      integrand_id,
      x,
      pack_vec
   );
   //
   // average std mulcov
   double avg_effect_1 = avg_effect(
      age_lower, age_lower + ode_step_size,
      time_lower, time_lower + ode_step_size
   );
   double avg_effect_2  = avg_effect(
      age_lower + ode_step_size, age_lower + 2.0 *ode_step_size,
      time_lower,                time_lower + ode_step_size
   );
   double check = (avg_effect_1 + avg_effect_2) / 2.0;
   //
   // check
   ok  &= fabs( 1.0 - avg / check ) <= eps99;
   /*
   cout << "Debugging" << std::endl;
   cout << ", check = " << check;
   cout << ", relerr    = " << 1.0 - avg / check  << std::endl;
   */
   return ok;
}
// END C++
