# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Build test / user
SET(depends "")
FOREACH(user_case
	asymptotic
	avgint
	bound_frac
	bound_random
	censor_1
	censor_2
	const_value
	csv2db
	db2csv
	fit_meas_noise
	fit_sim
	init_covariate
	laplace
	minimum_cv
	neg_iteration
	no_data
	not_ordered
	parent_node_id
	posterior
	relrisk
	scale_gamma
	scale_zero
	set_command
	subgroup_mulcov
	zero_random_1
	zero_random_2
)
	ADD_CUSTOM_TARGET(
		check_test_user_${user_case}
		bin/user_test.sh test/user/${user_case}.py
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		DEPENDS dismod_at
	)
	SET(depends ${depends} check_test_user_${user_case} )
ENDFOREACH(user_case zero_random)
ADD_CUSTOM_TARGET( check_test_user DEPENDS ${depends} )
