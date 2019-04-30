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

# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include <dismod_at/get_integrand_table.hpp>
# include <dismod_at/data_subset.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/pack_prior.hpp>
# include <dismod_at/get_density_table.hpp>

namespace dismod_at {
	void simulate_command(
		const std::string&                                  number_simulate ,
		sqlite3*                                            db              ,
		const CppAD::vector<dismod_at::integrand_struct>&   integrand_table ,
		const CppAD::vector<dismod_at::data_subset_struct>& data_subset_obj ,
		dismod_at::data_model&                              data_object     ,
		const dismod_at::pack_prior&                        var2prior       ,
		const CppAD::vector<dismod_at::prior_struct>&       prior_table     ,
		const CppAD::vector<dismod_at::density_enum>&       density_table
);
}

# endif
