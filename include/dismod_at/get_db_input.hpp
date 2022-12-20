// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_DB_INPUT_HPP
# define DISMOD_AT_GET_DB_INPUT_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

# include "get_option_table.hpp"
# include "get_covariate_table.hpp"
# include "get_data_table.hpp"
# include "get_avgint_table.hpp"
# include "get_density_table.hpp"
# include "get_integrand_table.hpp"
# include "get_mulcov_table.hpp"
# include "get_node_table.hpp"
# include "get_prior_table.hpp"
# include "get_rate_table.hpp"
# include "get_smooth_grid.hpp"
# include "get_smooth_table.hpp"
# include "get_weight_grid.hpp"
# include "get_weight_table.hpp"
# include "get_nslist_table.hpp"
# include "get_nslist_pair.hpp"
# include "get_subgroup_table.hpp"
# include "get_node_cov_table.hpp"

namespace dismod_at {
   // BEGIN STRUCT
   struct db_input_struct {
      CppAD::vector<double>             age_table;
      CppAD::vector<double>             time_table;
      //
      CppAD::vector<option_struct>      option_table;
      CppAD::vector<avgint_struct>      avgint_table;
      CppAD::vector<double>             avgint_cov_value;
      CppAD::vector<covariate_struct>   covariate_table;
      CppAD::vector<data_struct>        data_table;
      CppAD::vector<double>             data_cov_value;
      CppAD::vector<density_enum>       density_table;
      CppAD::vector<integrand_struct>   integrand_table;
      CppAD::vector<mulcov_struct>      mulcov_table;
      CppAD::vector<node_struct>        node_table;
      CppAD::vector<node_cov_struct>    node_cov_table;
      CppAD::vector<prior_struct>       prior_table;
      CppAD::vector<rate_struct>        rate_table;
      CppAD::vector<smooth_struct>      smooth_table;
      CppAD::vector<smooth_grid_struct> smooth_grid_table;
      CppAD::vector<weight_struct>      weight_table;
      CppAD::vector<weight_grid_struct> weight_grid_table;
      CppAD::vector<std::string>        nslist_table;
      CppAD::vector<nslist_pair_struct> nslist_pair_table;
      CppAD::vector<subgroup_struct>    subgroup_table;
   };
   // END STRUCT
   extern void get_db_input(
      sqlite3*         db       ,
      db_input_struct& db_input
   );
}

# endif
