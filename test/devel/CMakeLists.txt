# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Build C++ Examples / Tests
#
#
# Program is not installed, and depends on following source files
ADD_EXECUTABLE(test_devel EXCLUDE_FROM_ALL
	age_time_order.cpp
	data_model_subset.cpp
	grid2line.cpp
	meas_mulcov.cpp
	rate_mulcov.cpp
	test_devel.cpp
	cppad_mixed_xam.cpp
)
SET_TARGET_PROPERTIES(
	test_devel PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}"
)
TARGET_LINK_LIBRARIES(test_devel
	devel
	${cppad_mixed_library_path_list}
	${suitesparse_library_path_list}
	${ipopt_library_path_list}
	${sqlite3_library_path_list}
	${gsl_library_path_list}
	${system_specific_library_list}
)
ADD_CUSTOM_TARGET(check_test_devel test_devel DEPENDS test_devel )
