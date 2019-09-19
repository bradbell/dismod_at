# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-19 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Build check_example_user
SET(depends "")
FOREACH(user_case
	age_avg_split
	sample_asy
	cascade
	change_grid
	censor
	const_random
	const_value
	continue_fit
	csv2db
	data_sim
	diff_constraint
	fit_fixed_both
	fit_gamma
	fit_random
	fit_sim
	ill_condition
	jump_at_age
	lasso_covariate
	meas_covariate
	metropolis
	no_children
	posterior
	predict_fit
	predict_mulcov
	zero_sum_random
	re_scale
	students
)
	ADD_CUSTOM_TARGET(
		check_example_user_${user_case}
		bin/user_test.sh example/user/${user_case}.py
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
SET(quasi_fixed     "false")
SET(ode_step_size    10)
#
ADD_CUSTOM_TARGET(
	check_example_user_speed
	${python3_executable} example/user/speed.py
		${random_seed}
		${n_children}
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
