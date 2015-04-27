# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(dismod_at)
INCLUDE_DIRECTORIES( ${dismod_at_SOURCE_DIR}/include )

# ----------------------------------------------------------------------------
# get_prefix(package)
#
# ${package}_prefix: (out)
# set to its value (as a path) on the cmake command line.
# If it is not present a fatal error message is generated.
#
# ${${package}_prefix}/include:
# is added to the set of include directories.
#
MACRO(get_prefix package)
	SET(${package}_prefix NOTFOUND CACHE PATH
		"prefix used during install of ${package}"
	)
	IF( NOT ${package}_prefix )
		MESSAGE(FATAL_ERROR "${package}_prefix not set by cmake command")
	ENDIF( NOT ${package}_prefix )
	MESSAGE(STATUS "${package}_prefix = ${${package}_prefix}")
	INCLUDE_DIRECTORIES( ${${package}_prefix}/include )
ENDMACRO(get_prefix)
# ----------------------------------------------------------------------------

# get package prefixes
get_prefix(cppad)
get_prefix(eigen)
get_prefix(ipopt)
get_prefix(suitesparse)

# extra_cxx_flags
SET(extra_cxx_flags NOTFOUND CACHE STRING
	"extra compiler flags (besides debug and release"
)
IF( NOT extra_cxx_flags )
	MESSAGE(FATAL_ERROR "extra_cxx_flags not set by cmake command")
ENDIF( NOT extra_cxx_flags )
MESSAGE(STATUS "extra_cxx_flags = ${extra_cxx_flags}")


ADD_SUBDIRECTORY(devel)
ADD_SUBDIRECTORY(example/table)
ADD_SUBDIRECTORY(example/devel)
ADD_SUBDIRECTORY(test/devel)
ADD_SUBDIRECTORY(example/get_started)

ADD_CUSTOM_TARGET(check DEPENDS
	check_example_table
	check_example_devel
	check_test_devel
	check_get_started
)
