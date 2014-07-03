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
	get_table_column.cpp
	get_rate_table.cpp
)
SET_TARGET_PROPERTIES(cpp_lib PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}")
