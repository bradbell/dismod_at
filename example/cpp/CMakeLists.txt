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
ADD_EXECUTABLE(cpp EXCLUDE_FROM_ALL 
	cpp.cpp
	get_age_table_xam.cpp
	get_covariate_table_xam.cpp
	get_data_table_xam.cpp
	get_density_table_xam.cpp
	get_integrand_table_xam.cpp
	get_like_table_xam.cpp
	get_multiplier_table_xam.cpp
	get_node_table_xam.cpp
	get_rate_prior_xam.cpp
	get_rate_table_xam.cpp
	get_smooth_grid_xam.cpp
	get_table_column_xam.cpp
	get_time_table_xam.cpp
	get_weight_grid_xam.cpp
)

ADD_EXECUTABLE(get_started EXCLUDE_FROM_ALL
	get_started.cpp
)
LINK_DIRECTORIES( ${dismod_at_BINARY_DIR}/cpp )
FOREACH(target cpp get_started)
	SET_TARGET_PROPERTIES(
		${target} PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}"
	)
	TARGET_LINK_LIBRARIES(${target} sqlite3 cpp_lib)
ENDFOREACH(target)
#
ADD_CUSTOM_TARGET(check_cpp cpp DEPENDS cpp ) 
#
CONFIGURE_FILE(
	${CMAKE_SOURCE_DIR}/example/get_started.py.in
	${CMAKE_CURRENT_BINARY_DIR}/get_started.py
)
ADD_CUSTOM_TARGET( 
	run_get_started_py 
	python get_started.py
)
ADD_CUSTOM_TARGET(
	check_get_started 
	get_started example.db
	DEPENDS 
	get_started run_get_started_py
)
