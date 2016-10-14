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
# set_libary_path( directory_list type library_list )
#
# directory_list
# is a list of directories to search for the specified libraries.
#
# library_list
# is a list of libraries to be search for. It is a fatal error if one
# of the libraries cannot be found. Otherwise, for each library
# a variable called ${library}_library_path is set to the full
# path for the required library.
#
MACRO( set_library_path directory_list library_list )
	FOREACH( library ${library_list} )
		FIND_LIBRARY(
			${library}_library_path ${library}
			PATHS ${directory_list}
		)
		IF( ${${library}_library_path} STREQUAL
			"${library}_library_path-NOTFOUND" )
			MESSAGE( STATUS "directory_list=${directory_list}")
			MESSAGE( FATAL_ERROR "Cannot find library=${library}" )
		ENDIF( ${${library}_library_path} STREQUAL "${library}-NOTFOUND" )
		MESSAGE(STATUS "${library}_library_path = ${${library}_library_path}")
	ENDFOREACH( library )
ENDMACRO( set_library_path )
# ============================================================================
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(dismod_at)
SET(dismod_at_version "20161014" )
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
required_definition(cmake_libdir STRING
	"directory below prefix where libraries are stored; e.g, lib64"
)
# ----------------------------------------------------------------------------
# get required package prefixes
get_prefix(cppad)
get_prefix(eigen)
get_prefix(ipopt)
get_prefix(suitesparse)
# ----------------------------------------------------------------------------
FOREACH( var cppad_mixed_set_sparsity )
IF( ${var} )
	MESSAGE(FATAL_ERROR "${var} is no longer used in dismod_at cmake command")
ENDIF( ${var} )
ENDFOREACH( var )
# ----------------------------------------------------------------------------
MESSAGE(STATUS "END:")
# ----------------------------------------------------------------------------
# For each library in ${ipopt_LIBRARIES} set ${library}_library_path
# (note CppAD must be installed with ipopt_prefix specified)
FIND_PACKAGE(PkgConfig)
pkg_check_modules(ipopt REQUIRED ipopt)
set_library_path( "${ipopt_LIBRARY_DIRS}" "${ipopt_LIBRARIES}" )
# ----------------------------------------------------------------------------
# For each library in ${suitesparse_LIBRARIES} set ${library}_library_path
# no pkg-config for SuiteSparse so explicitly set corresponding values
SET(suitesparse_LIBRARY_DIRS ${suitesparse_prefix}/${cmake_libdir} )
SET(suitesparse_LIBRARIES "cholmod;amd;camd;colamd;ccolamd;suitesparseconfig" )
set_library_path( "${suitesparse_LIBRARY_DIRS}" "${suitesparse_LIBRARIES}" )
# ----------------------------------------------------------------------------
# For each library in ${cppad_mixed_LIBRARIES} set ${library}_library_path
# no pkg-config for cppad_mixed so explicitly set corresponding values
SET(cppad_mixed_LIBRARY_DIRS ${cppad_prefix}/${cmake_libdir} )
SET(cppad_mixed_LIBRARIES "cppad_mixed" )
set_library_path( "${cppad_mixed_LIBRARY_DIRS}" "${cppad_mixed_LIBRARIES}" )
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

