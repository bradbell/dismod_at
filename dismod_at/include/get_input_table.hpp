// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_INPUT_TABLE_HPP
# define DISMOD_AT_GET_INPUT_TABLE_HPP

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
	struct input_table_struct {
		CppAD::vector<double>             age;
		CppAD::vector<double>             time;
		CppAD::vector<rate_enum>          rate;
		CppAD::vector<density_enum>       density;
		CppAD::vector<integrand_enum>     integrand;
		CppAD::vector<std::string>        weight;
		CppAD::vector<std::string>        smooth;
		//;
		CppAD::vector<covariate_struct>   covariate;
		CppAD::vector<node_struct>        node;
		CppAD::vector<like_struct>        like;
		CppAD::vector<weight_grid_struct> weight_grid;
		CppAD::vector<smooth_grid_struct> smooth_grid;
		CppAD::vector<rate_prior_struct>  rate_prior;
		CppAD::vector<multiplier_struct>  multiplier;
		CppAD::vector<data_struct>        data;
	};
	extern void get_input_table(
		sqlite3*      db            , 
		input_table_struct& input_table
	);
}

# endif
