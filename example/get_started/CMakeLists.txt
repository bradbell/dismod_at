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
# Build get_started Examples / Tests

ADD_CUSTOM_TARGET(
	check_variable_command
	${python_three_command} example/get_started/variable_command.py
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS dismod_at
)
ADD_CUSTOM_TARGET(
	check_fit_command
	${python_three_command} example/get_started/fit_command.py
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS dismod_at
)
ADD_CUSTOM_TARGET(
	check_simulate_command
	${python_three_command} example/get_started/simulate_command.py
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS dismod_at
)
ADD_CUSTOM_TARGET(
	check_get_started
	DEPENDS check_variable_command check_fit_command check_simulate_command
)
