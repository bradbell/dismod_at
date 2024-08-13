# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-24 Bradley M. Bell
# ----------------------------------------------------------------------------
CONFIGURE_FILE(
   ${CMAKE_CURRENT_SOURCE_DIR}/bin/dismodat.py.in
   ${CMAKE_CURRENT_SOURCE_DIR}/bin/dismodat.py
)
ADD_CUSTOM_TARGET(
   install_python
   pip3 install --ignore-installed --prefix=${dismod_at_prefix} .
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
#
# 2DO: add the install of dismodat.py functionality to pyproject.toml.
# Perhaps as dismod-at will be the python and dismod_at the C++ executable ?
INSTALL(
   FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/dismodat.py
   DESTINATION ${dismod_at_prefix}/bin
   PERMISSIONS WORLD_READ WORLD_EXECUTE
               GROUP_READ GROUP_EXECUTE
               OWNER_READ OWNER_WRITE OWNER_EXECUTE
)
