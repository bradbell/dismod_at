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
# Build test / user
SET(depends "")
FOREACH(user_case
	avgint
	bound_frac
	bound_random
	csv2db
	db2csv
	fit_gamma
	fit_sim
	init_covariate
	laplace
	minimum_cv
	neg_iteration
	no_data
	not_ordered
	posterior
	relrisk
	scale_zero
	zero_random_1
	zero_random_2
)
	ADD_CUSTOM_TARGET(
		check_test_user_${user_case}
		${python3_executable} test/user/${user_case}.py
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		DEPENDS dismod_at
	)
	SET(depends ${depends} check_test_user_${user_case} )
ENDFOREACH(user_case zero_random)
ADD_CUSTOM_TARGET( check_test_user DEPENDS ${depends} )
