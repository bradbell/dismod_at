# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
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

# cppad_prefix
SET(cppad_prefix NOTFOUND CACHE PATH "prefix used during CppAD install")
IF( NOT cppad_prefix )
	MESSAGE(FATAL_ERROR "cppad_prefix not set by cmake command")
ENDIF( NOT cppad_prefix )
MESSAGE(STATUS "cppad_prefix = ${cppad_prefix}")
INCLUDE_DIRECTORIES( ${cppad_prefix}/include )

# ipopt_prefix
SET(ipopt_prefix NOTFOUND CACHE PATH "prefix used during CppAD install")
IF( NOT ipopt_prefix )
	MESSAGE(FATAL_ERROR "ipopt_prefix not set by cmake command")
ENDIF( NOT ipopt_prefix )
MESSAGE(STATUS "ipopt_prefix = ${ipopt_prefix}")
INCLUDE_DIRECTORIES( ${ipopt_prefix}/include )

# suitesparse_prefix
SET(suitesparse_prefix NOTFOUND CACHE PATH "prefix used during CppAD install")
IF( NOT suitesparse_prefix )
	MESSAGE(FATAL_ERROR "suitesparse_prefix not set by cmake command")
ENDIF( NOT suitesparse_prefix )
MESSAGE(STATUS "suitesparse_prefix = ${suitesparse_prefix}")
INCLUDE_DIRECTORIES( ${suitesparse_prefix}/include )

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
