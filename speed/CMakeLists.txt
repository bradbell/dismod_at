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
# dismod_at speed test
#
SET(random_seed 0)
#
ADD_CUSTOM_TARGET(
	speed_simulated
	${python3_executable} example/user/speed.py ${random_seed}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS dismod_at
)
# -----------------------------------------------------------------------------
