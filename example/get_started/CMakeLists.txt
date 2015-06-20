# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
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

CONFIGURE_FILE(
	${CMAKE_CURRENT_SOURCE_DIR}/get_started.py.in
	${CMAKE_CURRENT_BINARY_DIR}/get_started.py
)
ADD_CUSTOM_TARGET(
	check_get_started
	${python_three_command} get_started.py
)
