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

// devel/model directory
# include "include/avg_integrand.hpp"
# include "include/bilinear_interp.hpp"
# include "include/child_data.hpp"
# include "include/data_model.hpp"
# include "include/eigen_ode2.hpp"
# include "include/integrate_1d.hpp"
# include "include/integrate_2d.hpp"
# include "include/interp_weight.hpp"
# include "include/smooth2ode.hpp"
# include "include/solve_ode.hpp"
# include "include/pack_var.hpp"

// devel/table directory
# include "include/mulcov_type.hpp"
# include "include/check_table_id.hpp"
# include "include/exec_sql_cmd.hpp"
# include "include/get_age_table.hpp"
# include "include/get_covariate_table.hpp"
# include "include/get_data_table.hpp"
# include "include/get_density_table.hpp"
# include "include/get_db_input.hpp"
# include "include/get_integrand_table.hpp"
# include "include/get_like_table.hpp"
# include "include/get_mulcov_table.hpp"
# include "include/get_node_table.hpp"
# include "include/get_rate_table.hpp"
# include "include/get_run_table.hpp"
# include "include/get_smooth_grid.hpp"
# include "include/get_smooth_table.hpp"
# include "include/get_table_column.hpp"
# include "include/get_time_table.hpp"
# include "include/get_weight_grid.hpp"
# include "include/get_weight_table.hpp"
# include "include/open_connection.hpp"
# include "include/smooth_info.hpp"
# include "include/table_error_exit.hpp"
# include "include/weight_info.hpp"

# endif
