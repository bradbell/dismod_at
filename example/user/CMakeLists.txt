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

FOREACH(user_case
	diff_constraint
	no_children
	lasso_covariate
	meas_covariate
)
	ADD_CUSTOM_TARGET(
		check_${user_case}
		${python_three_command} example/user/${user_case}.py
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		DEPENDS dismod_at
	)
ENDFOREACH(user_case diff_constraint)
ADD_CUSTOM_TARGET(
	check_user
	DEPENDS
	check_diff_constraint
	check_no_children
	check_lasso_covariate
	check_meas_covariate
)
