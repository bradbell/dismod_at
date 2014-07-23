# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2014-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
SET(CMAKE_BUILD_TYPE DEBUG)
ADD_LIBRARY(devel_lib EXCLUDE_FROM_ALL 
	model/eigen_ode2.cpp
	model/avg_integrand.cpp
	model/integrate_1d.cpp
	model/integrate_2d.cpp
	model/interp_weight.cpp
	model/smooth2ode.cpp
	model/solve_ode.cpp
	table/check_table_id.cpp
	table/exec_sql_cmd.cpp
	table/get_age_table.cpp
	table/get_covariate_table.cpp
	table/get_data_table.cpp
	table/get_density_table.cpp
	table/get_db_input.cpp
	table/get_integrand_table.cpp
	table/get_like_table.cpp
	table/get_multiplier_table.cpp
	table/get_node_table.cpp
	table/get_rate_prior.cpp
	table/get_rate_table.cpp
	table/get_run_table.cpp
	table/get_smooth_grid.cpp
	table/get_smooth_table.cpp
	table/get_table_column.cpp
	table/get_time_table.cpp
	table/get_weight_grid.cpp
	table/get_weight_table.cpp
	table/open_connection.cpp
	table/smooth_grid.cpp
	table/table_error_exit.cpp
	table/weight_grid.cpp
)
SET_TARGET_PROPERTIES(devel_lib PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}")
