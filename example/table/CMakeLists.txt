# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# Build C++ Examples / Tests

# Release or deug build
CONFIGURE_FILE(
   ${CMAKE_CURRENT_SOURCE_DIR}/run_all.py.in
   ${CMAKE_CURRENT_BINARY_DIR}/run_all.py
)
ADD_CUSTOM_TARGET(
   check_example_table
   ${python3_executable} run_all.py
)
