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

FOREACH(cmd init start fit truth simulate sample predict)
	ADD_CUSTOM_TARGET(
		check_example_get_started_${cmd}
		${python_three_command} example/get_started/${cmd}_command.py
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		DEPENDS dismod_at
	)
ENDFOREACH(cmd init start fit truth simulate sample predict)
ADD_CUSTOM_TARGET(
	check_example_get_started
	DEPENDS
	check_example_get_started_init
	check_example_get_started_start
	check_example_get_started_fit
	check_example_get_started_truth
	check_example_get_started_simulate
	check_example_get_started_sample
	check_example_get_started_predict
)
