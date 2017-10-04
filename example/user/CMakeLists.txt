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
# Build check_user_example
FOREACH(user_case
	asymptotic
	const_random
	const_value
	continue_fit
	diff_constraint
	fit_fixed
	fit_random
	fit_sim
	jump_at_age
	lasso_covariate
	meas_covariate
	metropolis
	no_children
	predict_fit
	random_zero_sum
	re_scale
	students
)
	ADD_CUSTOM_TARGET(
		check_example_user_${user_case}
		${python3_executable} example/user/${user_case}.py
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		DEPENDS dismod_at
	)
ENDFOREACH(user_case)
#
ADD_CUSTOM_TARGET(
	check_example_user
	DEPENDS
	check_example_user_asymptotic
	check_example_user_const_random
	check_example_user_const_value
	check_example_user_continue_fit
	check_example_user_diff_constraint
	check_example_user_fit_fixed
	check_example_user_fit_random
	check_example_user_fit_sim
	check_example_user_jump_at_age
	check_example_user_lasso_covariate
	check_example_user_meas_covariate
	check_example_user_metropolis
	check_example_user_no_children
	check_example_user_predict_fit
	check_example_user_random_zero_sum
	check_example_user_re_scale
	check_example_user_students
)
# ---------------------------------------------------------------------------
# Build speed_user_example
SET(random_seed      0)
SET(n_children       10)
SET(n_data_per_child 20)
SET(quasi_fixed     "false")
#
ADD_CUSTOM_TARGET(
	speed_example_user
	${python3_executable} example/user/speed.py
		${random_seed} ${n_children} ${n_data_per_child} ${quasi_fixed}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS dismod_at
)
# -----------------------------------------------------------------------------
