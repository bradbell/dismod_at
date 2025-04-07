# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
# Build get_started Examples / Tests
SET(depends "")
FOREACH(cmd db2csv depend init fit modify predict old2new set simulate sample)
   ADD_CUSTOM_TARGET(
      check_example_get_started_${cmd}
      ${python3_executable} bin/user_test.py example/get_started/${cmd}_command.py
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      DEPENDS dismod_at
   )
   SET( depends ${depends} check_example_get_started_${cmd} )
ENDFOREACH(cmd)
ADD_CUSTOM_TARGET( check_example_get_started DEPENDS ${depends} )
