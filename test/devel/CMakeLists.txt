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
ADD_EXECUTABLE(test_devel EXCLUDE_FROM_ALL 
	test_devel.cpp
)

LINK_DIRECTORIES( ${dismod_at_BINARY_DIR}/cpp )
SET_TARGET_PROPERTIES(
	test_devel PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}"
)
TARGET_LINK_LIBRARIES(test_devel sqlite3 devel_lib)
ADD_CUSTOM_TARGET(check_test_devel test_devel DEPENDS test_devel ) 
