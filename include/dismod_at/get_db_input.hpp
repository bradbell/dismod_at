// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_DB_INPUT_HPP
# define DISMOD_AT_GET_DB_INPUT_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/vector.hpp>

# include "get_option_table.hpp"
# include "get_covariate_table.hpp"
# include "get_data_table.hpp"
# include "get_avg_case_table.hpp"
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

namespace dismod_at {
	// BEGIN STRUCT
	struct db_input_struct {
		CppAD::vector<double>             age_table;
		CppAD::vector<double>             time_table;
		//
		CppAD::vector<option_struct>      option_table;
		CppAD::vector<avg_case_struct>    avg_case_table;
		CppAD::vector<covariate_struct>   covariate_table;
		CppAD::vector<data_struct>        data_table;
		CppAD::vector<double>             covariate_value;
		CppAD::vector<density_enum>       density_table;
		CppAD::vector<integrand_struct>   integrand_table;
		CppAD::vector<mulcov_struct>      mulcov_table;
		CppAD::vector<node_struct>        node_table;
		CppAD::vector<prior_struct>       prior_table;
		CppAD::vector<rate_struct>        rate_table;
		CppAD::vector<smooth_struct>      smooth_table;
		CppAD::vector<smooth_grid_struct> smooth_grid_table;
		CppAD::vector<weight_struct>      weight_table;
		CppAD::vector<weight_grid_struct> weight_grid_table;
	};
	// END STRUCT
	extern void get_db_input(
		sqlite3*      db            ,
		db_input_struct& db_input
	);
}

# endif
