// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_DISMOD_AT_HPP
# define DISMOD_AT_DISMOD_AT_HPP

# include <sqlite3.h>
# include <string>
# include <cppad/vector.hpp>

# include "include/check_table_id.hpp"
# include "include/exec_sql_cmd.hpp"
# include "include/get_age_table.hpp"
# include "include/get_covariate_table.hpp"
# include "include/get_data_table.hpp"
# include "include/get_density_table.hpp"
# include "include/get_input_table.hpp"
# include "include/get_integrand_table.hpp"
# include "include/get_like_table.hpp"
# include "include/get_multiplier_table.hpp"
# include "include/get_node_table.hpp"
# include "include/get_rate_prior.hpp"
# include "include/get_rate_table.hpp"
# include "include/get_run_table.hpp"
# include "include/get_smooth_grid.hpp"
# include "include/get_smooth_table.hpp"
# include "include/get_table_column.hpp"
# include "include/get_time_table.hpp"
# include "include/get_weight_grid.hpp"
# include "include/get_weight_table.hpp"
# include "include/open_connection.hpp"
# include "include/table_error_exit.hpp"

# endif
