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
	get_covariate_table_xam.cpp
	get_density_table_xam.cpp
	get_integrand_table_xam.cpp
	get_like_table_xam.cpp
	get_node_table_xam.cpp
	get_rate_table_xam.cpp
	get_smooth_grid_xam.cpp
	get_table_column_xam.cpp
	get_weight_grid_xam.cpp
)
SET_TARGET_PROPERTIES(cpp PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}")
LINK_DIRECTORIES( ${dismod_at_BINARY_DIR}/cpp )
TARGET_LINK_LIBRARIES(cpp sqlite3 cpp_lib)

# make check_cpp will run cpp program
ADD_CUSTOM_TARGET(check_cpp cpp  DEPENDS cpp cpp_lib)
