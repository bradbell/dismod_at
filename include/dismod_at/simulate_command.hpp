// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_SIMULATE_COMMAND_HPP
# define DISMOD_AT_SIMULATE_COMMAND_HPP

# include <map>
# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include <dismod_at/data_subset.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/pack_prior.hpp>
# include <dismod_at/get_db_input.hpp>

namespace dismod_at {

void simulate_command(
	const std::string&                       number_simulate   ,
	const std::string&                       meas_noise_effect ,
	sqlite3*                                 db                ,
	const CppAD::vector<data_subset_struct>& data_subset_obj   ,
	data_model&                              data_object       ,
	const pack_prior&                        var2prior         ,
	const pack_info&                         pack_object       ,
	const db_input_struct&                   db_input          ,
	// effectively const
	std::map<std::string, std::string>&      option_map
);

}

# endif
