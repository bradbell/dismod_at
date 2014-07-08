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
CONFIGURE_FILE(
	${CMAKE_CURRENT_SOURCE_DIR}/run_all.py.in
	${CMAKE_CURRENT_BINARY_DIR}/run_all.py
)
ADD_CUSTOM_TARGET( 
	check_table 
	python run_all.py
)
