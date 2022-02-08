#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-22 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
ADD_CUSTOM_TARGET(
	build_python
	${python3_executable} -m build
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
ADD_CUSTOM_TARGET(
	install_python
	pip3 install dist/dismod_at-${dismod_at_version}.tar.gz
		--prefix=${dismod_at_prefix}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	DEPENDS build_python
)
