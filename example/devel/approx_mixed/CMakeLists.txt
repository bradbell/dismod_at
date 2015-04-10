# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Build C++ Examples / Tests

# Release or deug build
SET(CMAKE_BUILD_TYPE DEBUG)

# get linking flags using pkg-config
FIND_PACKAGE(PkgConfig)
pkg_check_modules(ipopt ipopt)

LINK_DIRECTORIES(
	${ipopt_LIBRARY_DIRS}
)

ADD_EXECUTABLE(ipopt_xam EXCLUDE_FROM_ALL
	ipopt_xam.cpp
)

LINK_DIRECTORIES( ${ipopt_LIBRARY_DIRS} )
SET_TARGET_PROPERTIES(
	ipopt_xam PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}"
)
TARGET_LINK_LIBRARIES(ipopt_xam
	${ipopt_LIBRARIES}
)
ADD_CUSTOM_TARGET(check_ipopt_xam ipopt_xam DEPENDS ipopt_xam )
