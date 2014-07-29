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
# Build C++ Examples / Tests

# Release or deug build
SET(CMAKE_BUILD_TYPE DEBUG)

# Program cpp, not installed, and depends on following source files
ADD_EXECUTABLE(devel EXCLUDE_FROM_ALL 
	devel.cpp
	model/avg_integrand_xam.cpp
	model/bilinear_interp_xam.cpp
	model/eigen_ode2_xam.cpp
	model/integrate_1d_xam.cpp
	model/integrate_2d_xam.cpp
	model/interp_weight_xam.cpp
	model/smooth2ode_xam.cpp
	model/solve_ode_xam.cpp
	table/get_age_table_xam.cpp
	table/get_covariate_table_xam.cpp
	table/get_data_table_xam.cpp
	table/get_density_table_xam.cpp
	table/get_integrand_table_xam.cpp
	table/get_like_table_xam.cpp
	table/get_mulcov_table_xam.cpp
	table/get_node_table_xam.cpp
	table/get_rate_prior_xam.cpp
	table/get_rate_table_xam.cpp
	table/get_run_table_xam.cpp
	table/get_smooth_grid_xam.cpp
	table/get_table_column_xam.cpp
	table/get_time_table_xam.cpp
	table/get_weight_grid_xam.cpp
	table/smooth_info_xam.cpp
	table/weight_grid_xam.cpp
)

LINK_DIRECTORIES( ${dismod_at_BINARY_DIR}/cpp )
SET_TARGET_PROPERTIES(
	devel PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}"
)
TARGET_LINK_LIBRARIES(devel sqlite3 devel_lib)
ADD_CUSTOM_TARGET(check_devel devel DEPENDS devel ) 
