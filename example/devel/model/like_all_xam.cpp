// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-23 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin like_all_xam.cpp dev}

C++ like_all: Example and Test
##############################

{xrst_literal
   // BEGIN C++
   // END C++
}

{xrst_end like_all_xam.cpp}
*/
// BEGIN C++
# include <limits>
# include <dismod_at/data_model.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/age_avg_grid.hpp>
# include <dismod_at/cov2weight_map.hpp>

bool like_all_xam(void)
{  bool   ok = true;
   using CppAD::abs;
   using CppAD::vector;
   using std::cout;
   typedef CppAD::AD<double> Float;
   Float eps = CppAD::numeric_limits<Float>::epsilon() * 100;
   //
   // ode_step_size
   double ode_step_size = 30.0;
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
   double age_max     = age_table[n_age_table - 1];
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
   double time_max     = time_table[n_time_table - 1];
   //
   // density table
   size_t n_density = dismod_at::number_density_enum;
   vector<dismod_at::density_enum> density_table(n_density);
   for(size_t density_id = 0; density_id < n_density; ++density_id)
      density_table[density_id] = dismod_at::density_enum(density_id);
   //
   //
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
   double nan = std::numeric_limits<double>::quiet_NaN();
   vector<dismod_at::prior_struct> prior_table(1);
   prior_table[0].prior_name = "prior_zero";
   prior_table[0].density_id = size_t( dismod_at::uniform_enum );
   prior_table[0].lower      = -1.0;
   prior_table[0].upper      = +1.0;
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
   // integrand_table
   size_t n_integrand = dismod_at::number_integrand_enum;
   vector<dismod_at::integrand_struct> integrand_table(n_integrand);
   for(size_t i = 0; i < n_integrand; i++)
   {  integrand_table[i].integrand       = dismod_at::integrand_enum(i);
      integrand_table[i].minimum_meas_cv = 0.0;
   }
   //
   // n_node, node_table:
   size_t n_node = 1;
   CppAD::vector<dismod_at::node_struct> node_table(n_node);
   node_table[0].parent = DISMOD_AT_NULL_INT;
   //
   // parent_node_id
   size_t parent_node_id = 0;
   //
   // covariate table
   size_t n_covariate = 0;
   vector<dismod_at::covariate_struct> covariate_table(n_covariate);
   //
   // cov2weight_obj
   size_t n_weight = 0;
   std::string splitting_covariate = "";
   CppAD::vector<dismod_at::rate_eff_cov_struct> rate_eff_cov_table(0);
   dismod_at::cov2weight_map cov2weight_obj(
      n_node,
      n_weight,
      splitting_covariate,
      covariate_table,
      rate_eff_cov_table
   );
   //
   // data_table
   vector<dismod_at::data_struct> data_table(2);
   vector<double> data_cov_value(data_table.size() * n_covariate);
   //
   // parent node, time and age integrantion.
   for(size_t data_id = 0; data_id < 2; data_id++)
   {
      data_table[data_id].integrand_id =  int( dismod_at::mtother_enum );
      data_table[data_id].node_id    =  int( 0 );
      data_table[data_id].weight_id  =  int( 0 );
      data_table[data_id].age_lower  = 35.0;
      data_table[data_id].age_upper  = 55.0;
      data_table[data_id].time_lower = 1990.0;
      data_table[data_id].time_upper = 2000.0;
      data_table[data_id].meas_value = ( 50. * 1995) / (100.0 * 2000.0);
      data_table[data_id].meas_std   = data_table[data_id].meas_value / 10.;
      data_table[data_id].eta        = 1e-4;
      if( data_id == 0 )
         data_table[data_id].density_id =
            size_t( dismod_at::gaussian_enum );
      else
         data_table[data_id].density_id =
            size_t( dismod_at::log_gaussian_enum );
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
   {  smooth_table[smooth_id].n_age  =  int( s_info_vec[smooth_id].age_size() );
      smooth_table[smooth_id].n_time =  int( s_info_vec[smooth_id].time_size() );
   }
   // mulcov_table
   vector<dismod_at::mulcov_struct> mulcov_table(0);
   // rate_table
   vector<dismod_at::rate_struct>   rate_table(dismod_at::number_rate_enum);
   for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
   {  size_t smooth_id = 0;
      if( rate_id == dismod_at::pini_enum )
         smooth_id = 1; // only one age
      rate_table[rate_id].parent_smooth_id =  int( smooth_id );
      rate_table[rate_id].child_smooth_id  =  int( smooth_id );
      rate_table[rate_id].child_nslist_id  =  int( DISMOD_AT_NULL_INT );
   }
   // child_info
   dismod_at::child_info child_info4data(
      parent_node_id ,
      node_table     ,
      data_table
   );
   size_t n_child = child_info4data.child_size();
   assert( n_child == 0 );
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
   {  data_subset_table[i].data_id    = int(i);
      data_subset_table[i].hold_out   = 0;
      data_subset_table[i].density_id = data_table[i].density_id;
      data_subset_table[i].eta        = data_table[i].eta;
      data_subset_table[i].nu         = data_table[i].nu;
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
      cov2weight_obj,
      n_covariate,
      fit_simulated_data,
      meas_noise_effect,
      rate_case,
      bound_random,
      ode_step_size,
      age_avg_grid,
      age_table,
      time_table,
      covariate_table,
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
   data_object.replace_like(subset_data_obj);
   //
   // pack_vec
   vector<Float> pack_vec( pack_object.size() );
   dismod_at::pack_info::subvec_info info;
   for(size_t child_id = 0; child_id <= n_child; child_id++)
   {  info = pack_object.node_rate_value_info(dismod_at::omega_enum, child_id);
      dismod_at::smooth_info& s_info = s_info_vec[info.smooth_id];
      for(size_t i = 0; i < s_info.age_size(); i++)
      {  age = age_table[ s_info.age_id(i) ];
         for(size_t j = 0; j < s_info.time_size(); j++)
         {  time    = time_table[ s_info.time_id(j) ];
            size_t index   = info.offset + i * s_info.time_size() + j;
            pack_vec[index] = age * time / (age_max*time_max);
         }
      }
   }
   // check results
   ok &= data_table.size() == subset_data_obj.size();
   bool hold_out = false;
   // parent node data does not depend on random effects
   bool random_depend = false;
   CppAD::vector< dismod_at::residual_struct<Float> > residual_vec =
      data_object.like_all(hold_out, random_depend, pack_vec);
   for(size_t data_id = 0; data_id < data_table.size(); data_id++)
   {  Float avg         = data_object.average(data_id, pack_vec);
      Float  wres       = residual_vec[data_id].wres;
      Float  loglike    = residual_vec[data_id].logden_smooth;
      loglike          -= fabs( residual_vec[data_id].logden_sub_abs );
      double Delta      = data_table[data_id].meas_std;
      double y          = data_table[data_id].meas_value;
      double eta        = 1e-4;
      size_t density_id = data_table[data_id].density_id;
      //
      // check wres
      Float check;
      if( dismod_at::log_density( density_table[density_id] ) )
         check = (log(y+eta) - log(avg+eta)) / log(1.0 + Delta/(y+eta));
      else
         check = (y - avg) / Delta;
      ok  &= fabs( 1.0 - wres / check ) <= eps;
      /*
      if( data_id == 0 )
         cout << "Debugging" << std::endl;
      cout << "wres = " << wres;
      cout << ", check = " << check;
      cout << ", relerr    = " << 1.0 - wres / check  << std::endl;
      */
      // check loglike
      double pi =  3.14159265358979323846264338327950288;
      switch( density_table[density_id] )
      {  double delta;

         case dismod_at::uniform_enum:
         check = 0.0;
         break;

         case dismod_at::gaussian_enum:
         check = - log( Delta * sqrt(2.0 * pi) ) - wres * wres / 2.0;
         break;

         case dismod_at::laplace_enum:
         check = - log( Delta * sqrt(2.0) ) - sqrt(2.0) * fabs(wres);
         break;

         case dismod_at::log_gaussian_enum:
         delta = log(1.0 + Delta / (y + eta) );
         check = - log( delta * sqrt(2.0 * pi) ) - wres * wres / 2.0;
         break;

         case dismod_at::log_laplace_enum:
         delta = log(1.0 + Delta / (y + eta) );
         check = - log( delta * sqrt(2.0) ) - sqrt(2.0) * fabs(wres);
         break;

         default:
         assert(false);
      }
      if( density_table[density_id] == dismod_at::uniform_enum )
         ok &= check == loglike;
      else
      {  Float relerr = 1.0 - loglike / check;
         ok &= fabs( relerr ) <= eps;
      }
      /*
      cout << "loglike = " << loglike;
      cout << ", check = " << check;
      if( density_table[density_id] != dismod_at::uniform_enum )
      {  Float relerr = 1.0 - loglike / check;
         cout << ", relerr    = " << relerr;
         ok &= fabs( relerr ) <= eps;
      }
      cout << std::endl;
      */
   }

   return ok;
}
// END C++
