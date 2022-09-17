// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_SAMPLE_COMMAND_HPP
# define DISMOD_AT_SAMPLE_COMMAND_HPP

# include <map>
# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/prior_model.hpp>
# include <dismod_at/get_db_input.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

void sample_command(
   const std::string&                                 method           ,
   const std::string&                                 variables        ,
   const std::string&                                 number_sample    ,
   const std::string&                                 sample_index     ,
   sqlite3*                                           db               ,
   CppAD::vector<dismod_at::subset_data_struct>&      subset_data_obj  ,
   dismod_at::data_model&                             data_object      ,
   dismod_at::prior_model&                            prior_object     ,
   const CppAD::vector<dismod_at::prior_struct>&      prior_table      ,
   const dismod_at::pack_info&                        pack_object      ,
   const dismod_at::pack_prior&                       var2prior        ,
   const dismod_at::db_input_struct&                  db_input         ,
   const std::map<std::string, std::string>&          option_map
);

} // END_DISMOD_AT_NAMESPACE

# endif
