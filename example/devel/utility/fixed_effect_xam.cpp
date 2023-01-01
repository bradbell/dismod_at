// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin fixed_effect_xam.cpp$$
$spell
   var
$$

$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section C++ fixed_effect: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/fixed_effect.hpp>
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/open_connection.hpp>

bool fixed_effect_xam(void)
{  bool ok = true;
   using CppAD::vector;

   size_t n_integrand     = 1;
   size_t n_child         = 0;
   //
   // initialize
   size_t n_fixed_effect = 0;
   //
   // subgroup_table
   size_t n_subgroup = 1;
   vector<dismod_at::subgroup_struct> subgroup_table(n_subgroup);
   subgroup_table[0].subgroup_name = "world";
   subgroup_table[0].group_id      = 0;
   subgroup_table[0].group_name    = "world";
   //
   size_t n_smooth = 2;
   vector<dismod_at::smooth_struct> smooth_table(n_smooth);
   smooth_table[0].n_age  = 1;
   smooth_table[0].n_time = 3;
   smooth_table[1].n_age  = 2;
   smooth_table[1].n_time = 3;
   //
   // mulstd
   n_fixed_effect += n_smooth * 3;
   //
   // parent rates
   vector<dismod_at::rate_struct> rate_table(dismod_at::number_rate_enum);
   for(size_t rate_id = 0; rate_id < rate_table.size(); rate_id++)
   {  size_t parent_smooth_id = 1;
      if( rate_id == dismod_at::pini_enum )
         parent_smooth_id  = 0;
      rate_table[rate_id].parent_smooth_id =  int( parent_smooth_id );
      size_t n_age    = smooth_table[parent_smooth_id].n_age;
      size_t n_time   = smooth_table[parent_smooth_id].n_time;
      n_fixed_effect += n_age * n_time;
   }
   //
   // covariate multipliers
   size_t n_mulcov = 3;
   vector<dismod_at::mulcov_struct> mulcov_table(n_mulcov);
   //
   // mulcov_meas_value
   mulcov_table[0].mulcov_type  = dismod_at::meas_value_enum;
   mulcov_table[0].rate_id      = -1;
   mulcov_table[0].integrand_id = 0;
   mulcov_table[0].covariate_id = 0;
   mulcov_table[0].group_smooth_id    = 1;
   n_fixed_effect += smooth_table[1].n_age * smooth_table[1].n_time;
   //
   // mulcov_meas_noise
   mulcov_table[1].mulcov_type  = dismod_at::meas_noise_enum;
   mulcov_table[1].rate_id      = -1;
   mulcov_table[1].integrand_id = 0;
   mulcov_table[1].covariate_id = 1;
   mulcov_table[1].group_smooth_id    = 0;
   n_fixed_effect += smooth_table[0].n_age * smooth_table[0].n_time;
   //
   // mulcov_rate_value
   mulcov_table[2].mulcov_type  = dismod_at::rate_value_enum;
   mulcov_table[2].rate_id      = 3;
   mulcov_table[2].integrand_id = 0;
   mulcov_table[2].covariate_id = 2;
   mulcov_table[2].group_smooth_id    = 1;
   n_fixed_effect += smooth_table[1].n_age * smooth_table[1].n_time;
   //
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
   //
   // check n_fixed_effect
   ok &= n_fixed_effect == dismod_at::number_fixed(pack_object);

   // pack_index
   CppAD::vector<size_t> pack_index = dismod_at::fixed2var_id(pack_object);

   // pack_vec
   CppAD::vector<double> pack_vec( pack_object.size() );

   // fixed_vec
   CppAD::vector<double> fixed_vec(n_fixed_effect);

   // set value of fixed effects in pack_vec
   for(size_t i = 0; i < n_fixed_effect; i++)
      fixed_vec[i] = double(i + 1);
   dismod_at::pack_fixed(pack_object, pack_vec, fixed_vec);

   // check pack_index values
   for(size_t i = 0; i < n_fixed_effect; i++)
      ok &= pack_vec[ pack_index[i] ] == double(i+1);

   // clear fixed_vec
   for(size_t i = 0; i < n_fixed_effect; i++)
      fixed_vec[i] = 0.0;

   // get the fixed effects in pack_info
   dismod_at::unpack_fixed(pack_object, pack_vec, fixed_vec);

   // check value of fixed effects
   for(size_t i = 0; i < n_fixed_effect; i++)
      ok &= fixed_vec[i] == double(i + 1);

   return ok;
}
// END C++
