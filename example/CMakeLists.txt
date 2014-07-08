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

ADD_EXECUTABLE(get_started EXCLUDE_FROM_ALL
	get_started.cpp
)
LINK_DIRECTORIES( ${dismod_at_BINARY_DIR}/cpp )
SET_TARGET_PROPERTIES(
	get_started PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}"
)
TARGET_LINK_LIBRARIES(get_started sqlite3 cpp_lib)
#
CONFIGURE_FILE(
	${CMAKE_CURRENT_SOURCE_DIR}/get_started.py.in
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
