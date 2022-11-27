# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
ADD_CUSTOM_TARGET(
   build_python
   ${python3_executable} -m build
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
CONFIGURE_FILE(
   ${CMAKE_CURRENT_SOURCE_DIR}/bin/dismodat.py.in
   ${CMAKE_CURRENT_SOURCE_DIR}/bin/dismodat.py
)
ADD_CUSTOM_TARGET(
   install_python
   pip3 install --ignore-installed dist/dismod_at-${dismod_at_version}.tar.gz
      --prefix=${dismod_at_prefix}
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
   DEPENDS build_python
)
