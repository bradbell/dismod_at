# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# Sample Command line
#                     cmake \
#                        -Wno-dev \
# required variables:    -D python3_executable=value \
#                        -D extra_cxx_flags=value \
#                        -D cmake_libdir=value \
#                        -D dismod_at_prefix=value \
# optional
# standard variables:    -D CMAKE_VERBOSE_MAKEFILE=value \
#                        -D CMAKE_BUILD_TYPE=value \
#                        -D CMAKE_CXX_COMPILER=value \
#
# optional variables:    -D system_specific_library_list=value \
# ============================================================================
# Macro definitions
# ============================================================================
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
   IF( ${variable} STREQUAL NOTFOUND )
      MESSAGE( FATAL_ERROR "${variable} is not set by cmake command" )
   ELSE( ${variable} STREQUAL NOTFOUND )
      MESSAGE( STATUS "${variable} = ${${variable}}" )
   ENDIF( ${variable} STREQUAL NOTFOUND )
   IF( ${type} STREQUAL "BOOL" )
      IF( ${variable} )
         SET( ${variable}_01 1 )
      ELSE( ${variable} )
         SET( ${variable}_01 0 )
      ENDIF( ${variable} )
   ENDIF( ${type} STREQUAL "BOOL" )
ENDMACRO( required_definition )
# ----------------------------------------------------------------------------
# set_library_path_list(library_path_list search_directory_list library_list)
#
# library_path_list
# The input value of this variable does not matter. Upon return it is a list
# containing an absolute path for each of the requested libraries.
# The order for the libraries is the same as in library_list.
#
# search_directory_list
# is a list of directories to search for the specified libraries. The system
# default library directory is only searched when the libraries cannot
# be found in this list.
#
# library_list
# is a list of libraries to be search for. It is a fatal error if one
# of the libraries cannot be found. Otherwise, the absolute path for each
# library is included in library_path_list
#
MACRO( set_library_path_list
   library_path_list  search_directory_list library_list
)
   SET( ${library_path_list} "" )
   FOREACH( lib ${library_list} )
      FIND_LIBRARY(
         library_path
         ${lib}
         PATHS ${search_directory_list}
         NO_DEFAULT_PATH
      )
      # Only search the system default library directory when
      # the library is not in any of the specified search directories
      FIND_LIBRARY( library_path ${lib} )
      #
      IF( ${library_path} STREQUAL "library_path-NOTFOUND" )
         MESSAGE(STATUS "directory_list = ${search_directory_list}")
         MESSAGE(STATUS "CMAKE_LIBRARY_PATH = ${CMAKE_LIBRARY_PATH}")
         MESSAGE(FATAL_ERROR "Cannot find library = ${lib}" )
      ENDIF( ${library_path} STREQUAL "library_path-NOTFOUND" )
      MESSAGE(STATUS "${lib} path = ${library_path}")
      SET( ${library_path_list} "${${library_path_list}};${library_path}" )
      UNSET( library_path CACHE )
   ENDFOREACH( lib )
   # MESSAGE(STATUS "${library_path_list}=${${library_path_list}}"  )
ENDMACRO( set_library_path_list )
# ============================================================================
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(dismod_at)
SET(dismod_at_version "20221105")
INCLUDE_DIRECTORIES( ${dismod_at_SOURCE_DIR}/include )
INCLUDE(CheckCXXSourceRuns)
# ============================================================================
MESSAGE(STATUS "BEGIN:")
# ----------------------------------------------------------------------------
# standard cmake variables
MESSAGE(STATUS "CMAKE_VERBOSE_MAKEFILE = ${CMAKE_VERBOSE_MAKEFILE}")
MESSAGE(STATUS "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "CMAKE_CXX_COMPILER = ${CMAKE_CXX_COMPILER}")
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
# system_specific_library_list
#
SET( system_specific_library_list NOTFOUND CACHE STRING
   "include extra libraries that are needed by this particular system."
)
IF( "${system_specific_library_list}" STREQUAL "NOTFOUND"  )
   SET( system_specific_library_list "" )
ELSEIF( "${system_specific_library_list}" STREQUAL "NOTFOUND"  )
   MESSAGE(STATUS
      "system_specific_library_list = ${system_specific_library_list}"
   )
ENDIF( "${system_specific_library_list}" STREQUAL "NOTFOUND"  )
# ----------------------------------------------------------------------------
FOREACH( var cppad_mixed_set_sparsity eigen_prefix ipopt_prefix cppad_prefix)
   IF( ${var} )
   MESSAGE(FATAL_ERROR "${var} is no longer used in dismod_at cmake command")
   ENDIF( ${var} )
ENDFOREACH( var )
# ----------------------------------------------------------------------------
IF( NOT "${log_fatal_error}" STREQUAL ""  )
   MESSAGE(FATAL_ERROR "log_fatal_error has been removed from cmake command")
ENDIF( NOT "${log_fatal_error}" STREQUAL ""  )
# ----------------------------------------------------------------------------
MESSAGE(STATUS "END:")
# ----------------------------------------------------------------------------
# https://gitlab.kitware.com/cmake/community/-/wikis/doc/cmake/RPATH-handling
#
# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH FALSE)
#
# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
#
set(CMAKE_INSTALL_RPATH "${dismod_at_prefix}/${cmake_libdir}")
#
# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
#
# the RPATH to be used when installing, but only if it's not a system directory
list(FIND
   CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES
   "${dismod_at_prefix}/${cmake_libdir}"
   isSystemDir
)
if("${isSystemDir}" STREQUAL "-1")
   set(CMAKE_INSTALL_RPATH "${dismod_at_pefix}/${cmake_libdir}")
endif("${isSystemDir}" STREQUAL "-1")
# ----------------------------------------------------------------------------
# pkg-config information for: sqlite3, gsl, eigen, ipopt, cppad_mixed, cppad
FIND_PACKAGE(PkgConfig)
FOREACH(pkg sqlite3 gsl eigen3 ipopt cppad_mixed cppad)
   pkg_check_modules( ${pkg} QUIET ${pkg} )
   IF( ${pkg}_FOUND )
      MESSAGE(STATUS "Found ${pkg}.pc file")
   ELSE( ${pkg}_FOUND )
      MESSAGE(STATUS "Cannot find ${pkg}.pc file")
      MESSAGE(FATAL_ERROR  "PKG_CONFIG_PATH=$ENV{PKG_CONFIG_PATH}")
   ENDIF( ${pkg}_FOUND )
   # INLCUDE_DIRECTORIES
   IF( ${pkg} STREQUAL "eigen3" )
      INCLUDE_DIRECTORIES( SYSTEM ${eigen3_INCLUDE_DIRS} )
   ELSE( ${pkg} STREQUAL "eigen3" )
      INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
   ENDIF( ${pkg} STREQUAL "eigen3" )
   # LINK_DIRECTORIES
   LINK_DIRECTORIES( ${${pkg}_LIBRARY_DIRS} )
ENDFOREACH(pkg)
# ----------------------------------------------------------------------------
# no pkg-config for SuiteSparse so explicitly set
# suitesparse_INCLUDE_DIR
#
# suitesparse_prefix
# suitesparse_suffix
SET(suitesparse_prefix "NOTFOUND")
SET(suitesparse_suffix "NOTFOUND")
EXECUTE_PROCESS(
   COMMAND brew --prefix suitesparse
   OUTPUT_VARIABLE brew_prefix
   OUTPUT_STRIP_TRAILING_WHITESPACE
)
IF( NOT "${brew_prefix}" STREQUAL "" )
   FOREACH(suffix "include" "include/suitesparse")
      IF( EXISTS "${brew_prefix}/${suffix}/cholmod.h" )
         MESSAGE(STATUS "Using ${brew_prefix}/${suffix}/cholmod.h" )
         SET(suitesparse_prefix "${brew_prefix}")
         SET(suitesparse_suffix "${suffix}")
      ENDIF( EXISTS "${brew_prefix}/${suffix}/cholmod.h" )
   ENDFOREACH(suffix "include" "include/suitesparse")
ENDIF( NOT ( "${brew_prefix}" STREQUAL "" ) )
FOREACH(prefix
   /usr
   /usr/local
   /opt/local
   /usr/local/opt/suitesparse
   "${cmake_install_prefix}"
)
   FOREACH(suffix "include" "include/suitesparse")
      IF( EXISTS "${prefix}/${suffix}/cholmod.h" )
         IF( suitesparse_prefix )
            SET(same "yes")
            IF( NOT "${prefix}" STREQUAL "${suitesparse_prefix}" )
               SET(same "no")
            ELSEIF( NOT "${suffix}" STREQUAL "${suitesparse_prefix}" )
               SET(same "no")
            ENDIF()
            IF( same STREQUAL "no" )
               MESSAGE(STATUS "Not using ${prefix}/${suffix}/cholmod.h")
            ENDIF( same STREQUAL "no" )
         ELSE( suitesparse_prefix )
            MESSAGE(STATUS "Using ${prefix}/${suffix}/cholmod.h" )
            SET(suitesparse_prefix "${prefix}")
            SET(suitesparse_suffix "${suffix}")
         ENDIF( suitesparse_prefix )
      ENDIF( EXISTS "${prefix}/${suffix}/cholmod.h" )
   ENDFOREACH(suffix "include" "include/suitesparse")
ENDFOREACH(prefix)
#
# suitesparse_INCLUDE_DIR
IF( suitesparse_prefix )
   SET(suitesparse_INCLUDE_DIR  "${suitesparse_prefix}/${suitesparse_suffix}" )
ELSE( suitesparse_prefix )
   MESSAGE(FATAL_ERROR "cannot find cholmod.h" )
ENDIF( suitesparse_prefix )
IF( NOT "${suitesparse_INCLUDE_DIR}" STREQUAL "/usr/include" )
   MESSAGE(STATUS "suitesparse_INCLUDE_DIR=${suitesparse_INCLUDE_DIR}" )
   INCLUDE_DIRECTORIES( ${suitesparse_INCLUDE_DIR} )
ENDIF( "${suitesparse_INCLUDE_DIR}" != "/usr/include" )
# ----------------------------------------------------------------------------
# check compiler flags
#
SET(CMAKE_REQUIRED_INCLUDES    "" )
SET(CMAKE_REQUIRED_LIBRARIES   "" )
SET(CMAKE_REQUIRED_DEFINITIONS "" )
SET(CMAKE_REQUIRED_FLAGS       "" )
IF( extra_cxx_flags )
   SET(CMAKE_REQUIRED_FLAGS "${extra_cxx_flags}")
   SET(source "
   int main(void)
   {  return 0; }"
   )
   CHECK_CXX_SOURCE_RUNS("${source}" extra_cxx_flags_ok)
   IF( NOT extra_cxx_flags_ok )
      MESSAGE(STATUS "Cannot compile a simple C++ program, extra_cxx_flags")
   ENDIF( NOT extra_cxx_flags )
ENDIF( extra_cxx_flags )
# ----------------------------------------------------------------------------
# dismod_at_has_nullptr_01
#
SET(source "
int main(void)
{  char *c = nullptr;
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
   ${CMAKE_CURRENT_SOURCE_DIR}/bin/user_test.sh.in
   ${CMAKE_CURRENT_SOURCE_DIR}/bin/user_test.sh
)
# ----------------------------------------------------------------------------
# subdirectories
ADD_SUBDIRECTORY(python)
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
   check_example_table
   check_example_devel
   check_test_devel
   check_test_user
   check_example_get_started
   check_example_user
)
# check_example_user_speed and check_example_user_diabetes not include above
ADD_CUSTOM_TARGET(speed DEPENDS
   check_example_user_speed
   check_example_user_diabetes
)
