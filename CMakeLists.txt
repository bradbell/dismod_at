# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2014-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(dismod_at)
INCLUDE_DIRECTORIES( ${dismod_at_SOURCE_DIR} )

# cppad_prefix
SET(cppad_prefix NOTFOUND CACHE PATH "prefix used during CppAD install")
IF( NOT cppad_prefix )
	MESSAGE(FATAL_ERROR "cppad_prefix not set by cmake command")
ENDIF( NOT cppad_prefix )
MESSAGE(STATUS "cppad_prefix = ${cppad_prefix}")
INCLUDE_DIRECTORIES( ${cppad_prefix}/include )

# extra_cxx_flags
SET(extra_cxx_flags NOTFOUND CACHE STRING 
	"extra compiler flags (besides debug and release"
)
IF( NOT extra_cxx_flags )
	MESSAGE(FATAL_ERROR "extra_cxx_flags not set by cmake command")
ENDIF( NOT extra_cxx_flags )
MESSAGE(STATUS "extra_cxx_flags = ${extra_cxx_flags}")


ADD_SUBDIRECTORY(cpp)
ADD_SUBDIRECTORY(example/get_started)
ADD_SUBDIRECTORY(example/cpp)

ADD_CUSTOM_TARGET(check DEPENDS check_get_started check_cpp)
