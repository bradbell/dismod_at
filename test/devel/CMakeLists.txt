# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Build C++ Examples / Tests
#
LINK_DIRECTORIES(
	${dismod_at_BINARY_DIR}/cpp
	${ipopt_LIBRARY_DIRS}
)
# Program cpp, not installed, and depends on following source files
ADD_EXECUTABLE(test_devel EXCLUDE_FROM_ALL
	mixed_cppad/binomial.cpp
	mixed_cppad/delta_ranobj.cpp
	mixed_cppad/der_var_hes.cpp
	mixed_cppad/zero_random_one.cpp
	mixed_cppad/zero_random_two.cpp
	age_time_order.cpp
	data_model_subset.cpp
	meas_mulcov.cpp
	n_mixture.cpp
	rate_mulcov.cpp
	test_devel.cpp
)
SET_TARGET_PROPERTIES(
	test_devel PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}"
)
TARGET_LINK_LIBRARIES(test_devel
	devel
	mixed_cppad
	mixed_cppad_eigen
	mixed_cppad
	${ipopt_LIBRARIES}
	sqlite3
	gsl
	gslcblas
)
ADD_CUSTOM_TARGET(check_test_devel test_devel DEPENDS test_devel )
