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
INCLUDE(CheckCXXSourceRuns)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(dismod_at)
SET(dismod_at_version "20150711" )
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
IF( python_three_command )
	MESSAGE(STATUS "python_three_command = ${python_three_command}")
ELSE( python_three_command )
	MESSAGE(FATAL_ERROR "python_three_command not set by cmake command")
ENDIF( python_three_command )
# ----------------------------------------------------------------------------
IF( dismod_at_prefix )
	MESSAGE(STATUS "dismod_at_prefix = ${dismod_at_prefix}")
ELSE( dismod_at_prefix )
	MESSAGE(FATAL_ERROR "dismod_at_prefix not set by cmake command")
ENDIF( dismod_at_prefix )
# ----------------------------------------------------------------------------
# get other package prefixes
get_prefix(cppad)
get_prefix(eigen)
get_prefix(ipopt)
IF( suitesparse_prefix )
	SET(dismod_at_has_suitesparse 1)
	get_prefix(suitesparse)
ELSE( suitesparse_prefix )
	SET(dismod_at_has_suitesparse 0)
ENDIF( suitesparse_prefix )
# ----------------------------------------------------------------------------
# extra_cxx_flags
SET(extra_cxx_flags NOTFOUND CACHE STRING
	"extra compiler flags (besides debug and release"
)
IF( NOT extra_cxx_flags )
	MESSAGE(FATAL_ERROR "extra_cxx_flags not set by cmake command")
ENDIF( NOT extra_cxx_flags )
MESSAGE(STATUS "extra_cxx_flags = ${extra_cxx_flags}")
# ----------------------------------------------------------------------------
# set ipopt_LIBRARY_DIRS, ipopt_LIBRARIES
# (note CppAD must be installed with ipopt_prefix specified)
FIND_PACKAGE(PkgConfig)
pkg_check_modules(ipopt REQUIRED ipopt)
# ----------------------------------------------------------------------------
# dismod_at_has_nullptr
#
SET(source "
int main(void)
{	char *c = nullptr;
	return 0;
}"
)
IF( extra_cxx_flags )
	SET(CMAKE_REQUIRED_FLAGS "${extra_cxx_flags}")
ENDIF( extra_cxx_flags )
CHECK_CXX_SOURCE_RUNS("${source}" dismod_at_has_nullptr_flag)
IF( dismod_at_has_nullptr_flag )
	SET(dismod_at_has_nullptr 1)
ELSE( dismod_at_has_nullptr_flag )
	SET(dismod_at_has_nullptr 0)
ENDIF( dismod_at_has_nullptr_flag )
# ----------------------------------------------------------------------------
CONFIGURE_FILE(
	${CMAKE_CURRENT_SOURCE_DIR}/include/dismod_at/configure.hpp.in
	${CMAKE_CURRENT_SOURCE_DIR}/include/dismod_at/configure.hpp
)
# ----------------------------------------------------------------------------
# subdirectories
ADD_SUBDIRECTORY(devel)
ADD_SUBDIRECTORY(example/table)
ADD_SUBDIRECTORY(example/devel)
ADD_SUBDIRECTORY(test/devel)
ADD_SUBDIRECTORY(example/get_started)
# ----------------------------------------------------------------------------
ADD_CUSTOM_TARGET(check DEPENDS
	dismod_at
	devel_lib
	check_example_table
	check_example_devel
	check_test_devel
	check_get_started
)
