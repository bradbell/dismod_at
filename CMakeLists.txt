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
# Sample Command line
#                     cmake \
#                        -Wno-dev \
# standard variables:    -D CMAKE_VERBOSE_MAKEFILE=value \
#                        -D CMAKE_BUILD_TYPE=value \
# required variables:    -D python_three_command=value \
#                        -D extra_cxx_flags=value \
#                        -D dismod_at_prefix=value \
#                        -D cppad_prefix=value \
#                        -D eigen_prefix=value \
#                        -D ipopt_prefix=value \
# optional variables:    -D suitesparse_prefix=value \
#                        -D cppad_mixed_libdir=value
# ============================================================================
# Macro definitions
# ============================================================================
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
# required_definition( variable )
#
# If the specified varaible is defined, echo its definition in the output.
# Otherwise output a fatal error message
#
MACRO( required_definition variable )
	IF( ${variable} )
		MESSAGE( STATUS "${variable} = ${${variable}}" )
	ELSE( ${variable} )
		MESSAGE( FATAL_ERROR "${variable} is not set by cmake command" )
	ENDIF( ${variable} )
ENDMACRO( required_definition )
# ----------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(dismod_at)
SET(dismod_at_version "20151020" )
INCLUDE_DIRECTORIES( ${dismod_at_SOURCE_DIR}/include )
INCLUDE(CheckCXXSourceRuns)
# ============================================================================
MESSAGE(STATUS "BEGIN:")
# ----------------------------------------------------------------------------
# standard cmake variables
MESSAGE(STATUS "CMAKE_VERBOSE_MAKEFILE = ${CMAKE_VERBOSE_MAKEFILE}")
MESSAGE(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
# ----------------------------------------------------------------------------
# required special cmake variables
required_definition( python_three_command )
required_definition( extra_cxx_flags )
required_definition( dismod_at_prefix )
# ----------------------------------------------------------------------------
# get required package prefixes
get_prefix(cppad)
get_prefix(eigen)
get_prefix(ipopt)
# ----------------------------------------------------------------------------
# optional package prefix
IF( suitesparse_prefix )
	SET(dismod_at_has_suitesparse 1)
	get_prefix(suitesparse)
ELSE( suitesparse_prefix )
	SET(dismod_at_has_suitesparse 0)
	MESSAGE(STATUS "suitesparse_prefix not specified "
		"so corresponding example not included."
	)
ENDIF( suitesparse_prefix )
# ----------------------------------------------------------------------------
# optional cppad_mixed_libdir
SET( cppad_mixed_libdir NOTFOUND CACHE STRING
	"directory below dismod_at_prefix where cppad_mixed library is installed"
)
IF( cppad_mixed_libdir )
	MESSAGE(STATUS "cppad_mixed library will be installed in\n"
		"\t${dismod_at_prefix}/${cppad_mixed_libdir}"
	)
ELSE( cppad_mixed_libdir )
	MESSAGE(STATUS "cppad_mixed_libdir not specified "
		"so its library will not be installed"
	)
ENDIF( NOT cppad_mixed_libdir )
# ----------------------------------------------------------------------------
MESSAGE(STATUS "END:")
# ----------------------------------------------------------------------------
# set eigen_cxx_flags
SET(eigen_cxx_flags
	"${extra_cxx_flags} -Wno-shadow -Wno-deprecated-declarations"
)
MESSAGE(STATUS "eigen_cxx_flags = ${eigen_cxx_flags}")
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
SET(CMAKE_REQUIRED_INCLUDES    "" )
SET(CMAKE_REQUIRED_LIBRARIES   "" )
SET(CMAKE_REQUIRED_DEFINITIONS "" )
IF( extra_cxx_flags )
	SET(CMAKE_REQUIRED_FLAGS "${extra_cxx_flags}")
ElSE( extra_cxx_flags )
	SET(CMAKE_REQUIRED_FLAGS "" )
ENDIF( extra_cxx_flags )
CHECK_CXX_SOURCE_RUNS("${source}" dismod_at_has_nullptr_flag)
IF( dismod_at_has_nullptr_flag )
	SET(dismod_at_has_nullptr 1)
ELSE( dismod_at_has_nullptr_flag )
	SET(dismod_at_has_nullptr 0)
ENDIF( dismod_at_has_nullptr_flag )
MESSAGE(STATUS "dismod_at_has_nullptr = ${dismod_at_has_nullptr}")
# ----------------------------------------------------------------------------
CONFIGURE_FILE(
	${CMAKE_CURRENT_SOURCE_DIR}/include/dismod_at/configure.hpp.in
	${CMAKE_CURRENT_SOURCE_DIR}/include/dismod_at/configure.hpp
)
# ----------------------------------------------------------------------------
# subdirectories
ADD_SUBDIRECTORY(devel)
ADD_SUBDIRECTORY(example/devel)
ADD_SUBDIRECTORY(example/get_started)
ADD_SUBDIRECTORY(example/table)
ADD_SUBDIRECTORY(example/user)
ADD_SUBDIRECTORY(test/devel)
ADD_SUBDIRECTORY(test/user)
# ----------------------------------------------------------------------------
ADD_CUSTOM_TARGET(check DEPENDS
	dismod_at
	devel
	cppad_mixed
	check_example_table
	check_example_devel
	check_test_devel
	check_test_user
	check_example_get_started
	check_example_user
)
