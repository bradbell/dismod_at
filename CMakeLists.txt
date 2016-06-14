# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-16 University of Washington
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
# required variables:    -D python3_executable=value \
#                        -D extra_cxx_flags=value \
#                        -D dismod_at_prefix=value \
#                        -D cppad_prefix=value \
#                        -D eigen_prefix=value \
#                        -D ipopt_prefix=value \
#                         ..
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
	SET( ${package}_prefix NOTFOUND CACHE PATH
		"prefix used during the installation of ${package}"
	)
	IF( NOT ${package}_prefix )
		MESSAGE(FATAL_ERROR "${package}_prefix not set by cmake command")
	ENDIF( NOT ${package}_prefix )
	MESSAGE(STATUS "${package}_prefix = ${${package}_prefix}")
	IF( ${package} STREQUAL "eigen" )
		INCLUDE_DIRECTORIES( SYSTEM ${${package}_prefix}/include )
	ELSE( ${package} STREQUAL "eigen" )
		INCLUDE_DIRECTORIES( ${${package}_prefix}/include )
	ENDIF( ${package} STREQUAL "eigen" )
ENDMACRO(get_prefix)
# ----------------------------------------------------------------------------
# required_definition( variable type docstring )
#
# variable
# If this varaible is defined, echo its definition in the output.
# Otherwise output a fatal error message.
#
# type
# Is the type of the variable in the CMake Gui, must be one of the following:.
# FILEPATH, PATH, STRING, BOOL.
# If the variable type is BOOL, then ${variable}_01 is set to
# 1 (for true) or 0 (for false).
#
# docstring
# Is the description used for the variable in the CMake Gui.
#
#
MACRO( required_definition variable type docstring )
	SET( ${variable} NOTFOUND CACHE ${type} "${docstring}" )
	IF( ${variable} )
		MESSAGE( STATUS "${variable} = ${${variable}}" )
	ELSE( ${variable} )
		MESSAGE( FATAL_ERROR "${variable} is not set by cmake command" )
	ENDIF( ${variable} )
ENDMACRO( required_definition )
# ----------------------------------------------------------------------------
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(dismod_at)
SET(dismod_at_version "20160613" )
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
required_definition( python3_executable STRING
	"command for python3 on this system"
)
required_definition( extra_cxx_flags STRING
	"extra flags used by C++ compiler (debug and release are automatic)"
)
required_definition(dismod_at_prefix PATH
	"prefix where dismod_at will be installed"
)
# ----------------------------------------------------------------------------
# get required package prefixes
get_prefix(cppad)
get_prefix(eigen)
get_prefix(ipopt)
get_prefix(suitesparse)
# ----------------------------------------------------------------------------
FOREACH( var cppad_mixed_set_sparsity cppad_mixed_libdir )
IF( ${var} )
	MESSAGE(FATAL_ERROR "${var} is no longer used in dismod_at cmake command")
ENDIF( ${var} )
ENDFOREACH( var )
# ----------------------------------------------------------------------------
MESSAGE(STATUS "END:")
# ----------------------------------------------------------------------------
# set ipopt_LIBRARY_DIRS, ipopt_LIBRARIES
# (note CppAD must be installed with ipopt_prefix specified)
FIND_PACKAGE(PkgConfig)
pkg_check_modules(ipopt REQUIRED ipopt)
# for debugging
FOREACH( dir ${ipopt_LIBRARY_DIRS} )
	MESSAGE(STATUS "ipopt_LIBRARY_DIR = ${dir}")
ENDFOREACH( dir )
# ----------------------------------------------------------------------------
# no pkg-config for SuiteSparse so explicitly set corresponding values
SET(suitesparse_LIBRARY_DIRS ${suitesparse_prefix}/lib )
SET(suitesparse_LIBRARIES "cholmod;amd;camd;colamd;ccolamd;suitesparseconfig" )
# ----------------------------------------------------------------------------
# dismod_at_has_nullptr_01
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
	SET(dismod_at_has_nullptr_01 1)
ELSE( dismod_at_has_nullptr_flag )
	SET(dismod_at_has_nullptr_01 0)
ENDIF( dismod_at_has_nullptr_flag )
MESSAGE(STATUS "dismod_at_has_nullptr = ${dismod_at_has_nullptr_01}")
# ----------------------------------------------------------------------------
CONFIGURE_FILE(
	${CMAKE_CURRENT_SOURCE_DIR}/include/dismod_at/configure.hpp.in
	${CMAKE_CURRENT_SOURCE_DIR}/include/dismod_at/configure.hpp
)
CONFIGURE_FILE(
	${CMAKE_CURRENT_SOURCE_DIR}/bin/dismodat.py.in
	${CMAKE_CURRENT_SOURCE_DIR}/bin/dismodat.py
)
CONFIGURE_FILE(
	${CMAKE_CURRENT_SOURCE_DIR}/bin/setup.py.in
	${CMAKE_CURRENT_SOURCE_DIR}/bin/setup.py
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
ADD_SUBDIRECTORY(speed)
# ----------------------------------------------------------------------------
ADD_CUSTOM_TARGET(check DEPENDS
	dismod_at
	devel
	check_example_table
	check_example_devel
	check_test_devel
	check_test_user
	check_example_get_started
	check_example_user
)
ADD_CUSTOM_TARGET(speed DEPENDS
	speed_simulated
)
ADD_CUSTOM_TARGET(
	install_python
	bin/setup.py install --prefix=${dismod_at_prefix}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
# ----------------------------------------------------------------------------
SET(setup_py
	"bin/setup.py install --prefix=${dismod_at_prefix}"
)
SET(work_dir
	"WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}"
)
INSTALL(CODE
	"EXECUTE_PROCESS( COMMAND ${setup_py} ${work_dir} )"
)

