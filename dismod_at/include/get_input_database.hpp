// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_INPUT_DATABASE_HPP
# define DISMOD_AT_GET_INPUT_DATABASE_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/vector.hpp>

# include "get_rate_table.hpp"
# include "get_integrand_table.hpp"
# include "get_density_table.hpp"
# include "get_covariate_table.hpp"
# include "get_data_table.hpp"
# include "get_like_table.hpp"
# include "get_multiplier_table.hpp"
# include "get_node_table.hpp"
# include "get_rate_prior.hpp"
# include "get_smooth_grid.hpp"
# include "get_weight_grid.hpp"

namespace dismod_at {
	struct input_struct {
		CppAD::vector<double>             age_table;
		CppAD::vector<double>             time_table;
		CppAD::vector<rate_enum>          rate_table;
		CppAD::vector<density_enum>       density_table;
		CppAD::vector<integrand_enum>     integrand_table;
		CppAD::vector<std::string>        weight_table;
		CppAD::vector<std::string>        smooth_table;
		//;
		CppAD::vector<covariate_struct>   covariate_table;
		CppAD::vector<node_struct>        node_table;
		CppAD::vector<like_struct>        like_table;
		CppAD::vector<weight_grid_struct> weight_grid;
		CppAD::vector<smooth_grid_struct> smooth_grid;
		CppAD::vector<rate_prior_struct>  rate_prior;
		CppAD::vector<multiplier_struct>  multiplier_table;
		CppAD::vector<data_struct>        data_table;
	};
	extern void get_input_database(
		sqlite3*      db            , 
		input_struct& input_database
	);
}

# endif
