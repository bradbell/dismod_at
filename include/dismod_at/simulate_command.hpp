// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_SIMULATE_COMMAND_HPP
# define DISMOD_AT_SIMULATE_COMMAND_HPP

# include <map>
# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include <dismod_at/subset_data.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/pack_prior.hpp>
# include <dismod_at/get_db_input.hpp>

namespace dismod_at {

void simulate_command(
   const std::string&                       number_simulate   ,
   const std::string&                       meas_noise_effect ,
   sqlite3*                                 db                ,
   const CppAD::vector<subset_data_struct>& subset_data_obj   ,
   data_model&                              data_object       ,
   const pack_prior&                        var2prior         ,
   const pack_info&                         pack_object       ,
   const db_input_struct&                   db_input          ,
   // effectively const
   std::map<std::string, std::string>&      option_map
);

}

# endif
