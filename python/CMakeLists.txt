# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
CONFIGURE_FILE(
   ${CMAKE_CURRENT_SOURCE_DIR}/bin/dismodat.py.in
   ${CMAKE_CURRENT_SOURCE_DIR}/bin/dismodat.py
)
CONFIGURE_FILE(
   ${CMAKE_CURRENT_SOURCE_DIR}/dismod_at/__init__.py.in
   ${CMAKE_CURRENT_SOURCE_DIR}/dismod_at/__init__.py
)
ADD_CUSTOM_TARGET(
   install_python
   pip3 install --ignore-installed --prefix=${dismod_at_prefix} .
   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
#
# Install dismodat.py for backward compatibility on non windows systems.
# This has been replaced by the script dismod-at in pyproject.toml.
INSTALL(
   FILES ${CMAKE_CURRENT_SOURCE_DIR}/bin/dismodat.py
   DESTINATION ${dismod_at_prefix}/bin
   PERMISSIONS WORLD_READ WORLD_EXECUTE
               GROUP_READ GROUP_EXECUTE
               OWNER_READ OWNER_WRITE OWNER_EXECUTE
)
