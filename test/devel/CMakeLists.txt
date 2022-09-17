# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
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
   ${cppad_mixed_LIBRARIES}
   ${gsl_LIBRARIES}
   ${sqlite3_LIBRARIES}
   ${ipopt_LIBRARIES}
   ${system_specific_library_list}
)
ADD_CUSTOM_TARGET(check_test_devel test_devel DEPENDS test_devel )
