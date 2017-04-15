# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Build get_started Examples / Tests

FOREACH(user_case
	asymptotic
	const_random
	const_value
	diff_constraint
	fit_sim
	jump_at_age
	lasso_covariate
	meas_covariate
	metropolis
	no_children
	predict_fit
	random_bound
	random_zero_sum
)
	ADD_CUSTOM_TARGET(
		check_example_user_${user_case}
		${python3_executable} example/user/${user_case}.py
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		DEPENDS dismod_at
	)
ENDFOREACH(user_case diff_constraint)
ADD_CUSTOM_TARGET(
	check_example_user
	DEPENDS
	check_example_user_asymptotic
	check_example_user_const_random
	check_example_user_const_value
	check_example_user_diff_constraint
	check_example_user_fit_sim
	check_example_user_jump_at_age
	check_example_user_lasso_covariate
	check_example_user_meas_covariate
	check_example_user_metropolis
	check_example_user_no_children
	check_example_user_predict_fit
	check_example_user_random_bound
	check_example_user_random_zero_sum
)
