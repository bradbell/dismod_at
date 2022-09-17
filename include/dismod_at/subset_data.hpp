// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_SUBSET_DATA_HPP
# define DISMOD_AT_SUBSET_DATA_HPP

# include <cppad/utility/vector.hpp>
# include "get_data_table.hpp"
# include "get_covariate_table.hpp"
# include "get_density_table.hpp"
# include "get_integrand_table.hpp"
# include "get_data_subset.hpp"
# include "child_info.hpp"

namespace dismod_at {
   struct subset_data_struct {
      // values in avgint_subset_struct
      int         original_id;
      int         integrand_id;
      int         node_id;
      int         subgroup_id;
      int         weight_id;
      double      age_lower;
      double      age_upper;
      double      time_lower;
      double      time_upper;
      // values not in avgint_subset_struct
      int          hold_out; // depends on option table hold_out_integrand
      density_enum density;
      double       meas_value;
      double       meas_std;
      double       eta;
      double       nu;
      // value that depends data_sim table
      double       data_sim_value;
   };
   extern void subset_data(
      const std::map<std::string, std::string>& option_map              ,
      const CppAD::vector<data_subset_struct>&  data_subset_table       ,
      const CppAD::vector<integrand_struct>&    integrand_table         ,
      const CppAD::vector<density_enum>&        density_table           ,
      const CppAD::vector<data_struct>&         data_table              ,
      const CppAD::vector<double>&              data_cov_value          ,
      const CppAD::vector<covariate_struct>&    covariate_table         ,
      const child_info&                         child_info4data         ,
      CppAD::vector<subset_data_struct>&       subset_data_obj         ,
      CppAD::vector<double>&                   subset_data_cov_value
   );
}

# endif
