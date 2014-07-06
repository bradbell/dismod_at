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
ADD_LIBRARY(cpp_lib EXCLUDE_FROM_ALL 
	check_table_id.cpp
	exec_sql_cmd.cpp
	get_covariate_table.cpp
	get_data_table.cpp
	get_density_table.cpp
	get_integrand_table.cpp
	get_like_table.cpp
	get_multiplier_table.cpp
	get_node_table.cpp
	get_rate_prior.cpp
	get_rate_table.cpp
	get_smooth_grid.cpp
	get_smooth_table.cpp
	get_table_column.cpp
	get_weight_grid.cpp
	get_weight_table.cpp
	open_connection.cpp
	table_error_exit.cpp
)
SET_TARGET_PROPERTIES(cpp_lib PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}")
