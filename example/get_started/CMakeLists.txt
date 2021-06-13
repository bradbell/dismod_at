# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Build get_started Examples / Tests
SET(depends "")
FOREACH(cmd db2csv depend init fit modify predict old2new set simulate sample)
	ADD_CUSTOM_TARGET(
		check_example_get_started_${cmd}
		bin/user_test.sh example/get_started/${cmd}_command.py
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		DEPENDS dismod_at
	)
	SET( depends ${depends} check_example_get_started_${cmd} )
ENDFOREACH(cmd)
ADD_CUSTOM_TARGET( check_example_get_started DEPENDS ${depends} )
