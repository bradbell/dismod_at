// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin avgint_subset_xam.cpp$$
$spell
   avgint
   avg
   xam
$$

$nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section C++ avgint_subset: Example and Test$$

$srcthisfile%
   0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/get_avgint_table.hpp>
# include <dismod_at/get_covariate_table.hpp>
# include <dismod_at/get_node_table.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/avgint_subset.hpp>
# include <dismod_at/null_int.hpp>

bool avgint_subset_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;
   double inf = std::numeric_limits<double>::infinity();

   // integrand table
   size_t n_integrand = 1;
   vector<dismod_at::integrand_struct> integrand_table(n_integrand);
   integrand_table[0].integrand       = dismod_at::Sincidence_enum;
   integrand_table[0].minimum_meas_cv = 0.0;
   integrand_table[0].mulcov_id       = DISMOD_AT_NULL_INT;

   // covariate table
   size_t n_covariate = 2;
   vector<dismod_at::covariate_struct> covariate_table(n_covariate);
   covariate_table[0].reference      = -0.5; // sex reference value
   covariate_table[0].max_difference = 0.6;  // maximum sex difference
   covariate_table[1].reference      = 0.0;
   covariate_table[1].max_difference = inf;

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

   // default value for a avgint table record
   dismod_at::avgint_struct record;
   record.integrand_id   = 0;
   record.weight_id      = 4;
   record.age_lower      = 10.0;
   record.age_upper      = 90.0;
   record.time_lower     = 2000.0;
   record.time_upper     = 2010.0;

   // avgint table
   size_t n_avgint = n_node;
   vector<dismod_at::avgint_struct> avgint_table(n_avgint);
   vector<double> avgint_cov_value( n_avgint * n_covariate );
   for(size_t avgint_id = 0; avgint_id < n_avgint; avgint_id++)
   {  avgint_table[avgint_id]         = record;
      avgint_table[avgint_id].node_id =  int( avgint_id );
      avgint_cov_value[ avgint_id * n_covariate + 1 ] =
         100. * double(avgint_id);
   }
   avgint_cov_value[0 * n_covariate + 0] = -0.5; // sex reference
   avgint_cov_value[1 * n_covariate + 0] = -0.5; // sex reference
   avgint_cov_value[2 * n_covariate + 0] = 0.0;  // within sex bounds
   avgint_cov_value[3 * n_covariate + 0] = 0.5;  // out of sex bounds
   //
   // child_info4avgint
   size_t parent_node_id = 1; // north_america
   dismod_at::child_info child_info4avgint(
      parent_node_id, node_table, avgint_table
   );

   // avgint_subset_obj
   vector<dismod_at::avgint_subset_struct> avgint_subset_obj;
   vector<double> avgint_subset_cov_value;
   avgint_subset(
      integrand_table,
      avgint_table,
      avgint_cov_value,
      covariate_table,
      child_info4avgint,
      avgint_subset_obj,
      avgint_subset_cov_value
   );

   // avgint_id = 0 is for world and hence not included
   ok &= avgint_subset_obj[0].original_id == 1;
   // avgint_id = 1 covariate values minus corresponding reference value
   ok &= avgint_subset_cov_value[0 * n_covariate + 0]    == 0.0;
   ok &= avgint_subset_cov_value[0 * n_covariate + 1]    == 100.;
   ok &= avgint_subset_obj[0].node_id == 1;
   // avgint_id = 2 covariate values minus corresponding reference value
   ok &= avgint_subset_obj[1].original_id == 2;
   ok &= avgint_subset_cov_value[1 * n_covariate + 0]    == 0.5;
   ok &= avgint_subset_cov_value[1 * n_covariate + 1]    == 200.;
   ok &= avgint_subset_obj[1].node_id == 2;
   // avgint_id = 3 is has a sex covariate that is out of bounds
   ok &= avgint_subset_obj.size() == 2;

   return ok;
}
// END C++
