// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin child_data_in_fit_xam.cpp$$
$spell
   data
   xam
$$

$section C++ subset_data: Example and Test$$

$srcthisfile%
   0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/get_data_subset.hpp>
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/get_data_table.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/child_data_in_fit.hpp>

bool child_data_in_fit_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;
   //
   // covariate table
   size_t n_covariate = 0;
   vector<dismod_at::covariate_struct> covariate_table(n_covariate);
   //
   // density_table
   vector<dismod_at::density_enum> density_table(2);
   density_table[0] = dismod_at::uniform_enum;
   density_table[1] = dismod_at::gaussian_enum;
   //
   // integrand_table
   vector<dismod_at::integrand_struct> integrand_table(1);
   integrand_table[0].integrand        = dismod_at::Sincidence_enum;
   integrand_table[0].minimum_meas_cv = 0.0;
   integrand_table[0].mulcov_id       = DISMOD_AT_NULL_INT;
   //
   // node table
   size_t n_node = 4;
   vector<dismod_at::node_struct> node_table(n_node);
   node_table[0].node_name = "world";
   node_table[1].node_name = "north_america";
   node_table[2].node_name = "united_states";
   node_table[3].node_name = "canada";
   //
   node_table[0].parent = DISMOD_AT_NULL_INT;
   node_table[1].parent = 0;
   node_table[2].parent = 1;
   node_table[3].parent = 1;
   //
   // default value for a data table record
   dismod_at::data_struct data_rec;
   data_rec.integrand_id   = 0;
   data_rec.node_id        = 0;
   data_rec.subgroup_id    = 0;
   data_rec.weight_id      = 0;
   data_rec.age_lower      = 10.0;
   data_rec.age_upper      = 90.0;
   data_rec.time_lower     = 2000.0;
   data_rec.time_upper     = 2010.0;
   data_rec.hold_out       = 0;
   data_rec.density_id     = 1;
   data_rec.meas_value     = 1e-4;
   data_rec.meas_std       = 1e-5;
   data_rec.eta            = 1e-7;
   data_rec.nu             = 5.0;

   // data table
   size_t n_data = 2 * n_node;
   vector<dismod_at::data_struct> data_table(n_data);
   for(size_t data_id = 0; data_id < n_data; data_id++)
   {  data_table[data_id]          = data_rec;
      data_table[data_id].node_id  =  int( data_id / 2 );
      // hold out one data per node
      data_table[data_id].hold_out = data_id % 2;
   }
   //
   // data_subset_table
   vector<dismod_at::data_subset_struct> data_subset_table;
   for(size_t data_id = 0; data_id < n_data; ++data_id)
   {  if( data_table[data_id].node_id > 0 )
      {  dismod_at::data_subset_struct subset_rec;
         subset_rec.data_id    = int( data_id );
         subset_rec.hold_out   = 0;
         subset_rec.density_id = data_table[data_id].density_id;
         subset_rec.eta        = data_table[data_id].eta;
         subset_rec.nu         = data_table[data_id].nu;
         data_subset_table.push_back(subset_rec);
      }
   }
   //
   // child_info4data
   size_t parent_node_id = 1; // north_america
   dismod_at::child_info child_info4data(
      parent_node_id, node_table, data_table
   );
   //
   // n_data_in_fit
   std::map<std::string, std::string> option_map;
   vector<size_t> n_data_in_fit = child_data_in_fit(
      option_map,
      data_subset_table,
      integrand_table,
      data_table,
      child_info4data
   );
   //
   // There are two children for this case, united_states and canada.
   ok &= n_data_in_fit.size() == 2;
   //
   // There are two data points for each node, but one is held out
   for(size_t child = 0; child < n_data_in_fit.size(); ++child)
      ok &= n_data_in_fit[child] == 1;
   //
   return ok;
}
// END C++
