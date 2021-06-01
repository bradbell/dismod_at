// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_FIT_COMMAND_HPP
# define DISMOD_AT_FIT_COMMAND_HPP

# include <string>
# include <map>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/prior_model.hpp>
# include <dismod_at/pack_info.hpp>
# include <dismod_at/pack_prior.hpp>
# include <dismod_at/get_db_input.hpp>
# include <dismod_at/data_subset.hpp>
# include <dismod_at/get_density_table.hpp>

namespace dismod_at {
	void fit_command(
		bool                                          use_warm_start   ,
		const std::string&                            variables        ,
		const std::string&                            simulate_index   ,
		sqlite3*                                      db               ,
		CppAD::vector<dismod_at::data_subset_struct>& data_subset_obj  ,
		dismod_at::data_model&                        data_object      ,
		dismod_at::prior_model&                       prior_object     ,
		const dismod_at::pack_info&                   pack_object      ,
		const dismod_at::pack_prior&                  var2prior        ,
		const dismod_at::db_input_struct&             db_input         ,
		// effectively const
		std::map<std::string, std::string>&           option_map
	);
}

# endif
