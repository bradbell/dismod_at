# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Build check_example_user
SET(depends "")
FOREACH(user_case
	asymptotic
	change_grid
	const_random
	const_value
	continue_fit
	csv2db
	diff_constraint
	fit_fixed_both
	fit_random
	fit_sim
	jump_at_age
	lasso_covariate
	meas_covariate
	metropolis
	no_children
	posterior
	predict_fit
	zero_sum_random
	re_scale
	students
)
	ADD_CUSTOM_TARGET(
		check_example_user_${user_case}
		${python3_executable} example/user/${user_case}.py
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		DEPENDS dismod_at
	)
	SET( depends ${depends} check_example_user_${user_case} )
ENDFOREACH(user_case)
#
ADD_CUSTOM_TARGET( check_example_user DEPENDS ${depends} )
# ---------------------------------------------------------------------------
# Build check_example_user_speed (not included above)
SET(random_seed      0)
SET(n_children       10)
SET(n_data_per_child 1)
SET(quasi_fixed     "false")
SET(ode_step_size    5)
#
ADD_CUSTOM_TARGET(
	check_example_user_speed
	${python3_executable} example/user/speed.py
		${random_seed}
		${n_children}
		${n_data_per_child}
		${quasi_fixed}
		${ode_step_size}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS dismod_at
)
# -----------------------------------------------------------------------------
# Build check_example_user_diabetes (not included above)
ADD_CUSTOM_TARGET(
	check_example_user_diabetes
	${python3_executable} example/user/diabetes.py
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS dismod_at
)
# -----------------------------------------------------------------------------
