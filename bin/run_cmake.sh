#! /bin/bash -e
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
#
# {xrst_begin run_cmake.sh}
# {xrst_spell
#     cmake
#     colons
#     compilers
#     cxx
#     fortran
#     gcc
#     gfortran
#     gnu
#     libdir
#     makefile
#     pthread
#     usr
#     wconversion
#     wpedantic
#     wshadow
# }
# {xrst_comment_ch #}
#
# bin/run_cmake.sh: User Configuration Options
# ############################################
#
# verbose_makefile
# ****************
# Use 'no' for normal and 'yes' for verbose make output:
# {xrst_code sh}
verbose_makefile='no'
# {xrst_code}
#
# build_type
# **********
# Use either ``debug`` or ``release`` for the type of this build:
# {xrst_code sh}
build_type='release'
# {xrst_code}
#
# dismod_at_prefix
# ****************
# Prefix where dismod_at will be installed:
# {xrst_code sh}
dismod_at_prefix="$HOME/prefix/dismod_at"
# {xrst_code}
# If *dismod_at_prefix* ends in ``dismod_at`` ,
# ``run_cmake.sh`` will use a soft link from this prefix to
# *dismod_at_prefix* . ``debug`` or
# *dismod_at_prefix* . ``release``
# depending on the choice for *build_type* .
#
# Debug and Release
# *****************
# If a soft link is used for the install,
# the same technique will be used to map the ``build``
# directory to the debug or release version.
# If you are using both a debug and release versions of dismod_at,
# both versions of the
# :ref:`install_unix@Special Requirements`
# will need to be installed.
#
# python3_executable
# ******************
# Path to the python3 executable on this machine.
# {xrst_code sh}
python3_executable='/usr/bin/python3'
# {xrst_code}
# You can use the command $code which python3$$ to determine the location
# of the default version for this system.
# In the past, removing the  characters in front of the following
# command would work for the IHME cluster:
#
#     ``python3_executable`` ='/ ``usr/local/anaconda3-current/bin/python`` '
#
# specific_compiler
# *****************
# On some systems, e.g. the Mac using port, there are problems with mixing
# different compiler systems for fortran and C++; see
# `ipopt issue 471 <https://github.com/coin-or/Ipopt/discussions/471>`_.
# This variable allows you to set a specific compiler for
# C, and or CXX and or FC. For example
# ``specific_compiler='CC=gcc CXX=g++ FC=gfortran'``
# uses the gnu versions of these compilers.
# The configuration will automatically find compilers that are not specified;
# i.e., if
# {xrst_code sh}
specific_compiler=''
# {xrst_code}
#
# extra_cxx_flags
# ***************
# Extra C++ flags used during compilation:
# {xrst_code sh}
extra_cxx_flags='-std=c++11 -Wpedantic -Wall -Wshadow -Wconversion'
# {xrst_code}
# An alternative might be ``-Wall`` .
#
# cmake_libdir
# ************
# Sub-directory of each prefix where libraries are installed.
# {xrst_code sh}
cmake_libdir='lib64'
# {xrst_code}
#
# system_specific_library_list
# ****************************
# List of libraries that are needed for a particular system. For example,
# if when you build ``dismod_at`` the ``pthread`` library is
# required by your system, then include it here.
# Libraries in the list can be separated by spaces or semi-colons.
# {xrst_code sh}
system_specific_library_list=''
# {xrst_code}
#
# {xrst_end run_cmake.sh}
# ============================================================================
# bash function that echos and executes a command
echo_eval() {
   echo $*
   eval $*
}
# -----------------------------------------------------------------------------
if [ "$0" != 'bin/run_cmake.sh' ]
then
   echo 'bin/run_cmake.sh: must be executed from its parent directory'
   exit 1
fi
while [ "$1" != '' ]
do
   if [ "$1" == '--help' ]
   then
      cat << EOF
usage: bin/run_cmake.sh \\
   [--help] \\
   [--debug] \\
   [--verbose]
EOF
      exit 0
   fi
   if [  "$1" == '--debug' ]
   then
      build_type='debug'
   elif [ "$1" == '--verbose' ]
   then
      verbose_makefile='yes'
   else
      echo "'$1' is an invalid option"
      bin/run_cmake.sh --help
      exit 1
   fi
   shift
done
# --------------------------------------------------------------------------
export PKG_CONFIG_PATH="$dismod_at_prefix/$cmake_libdir/pkgconfig"
# --------------------------------------------------------------------------
if echo "$dismod_at_prefix" | grep '/dismod_at$' > /dev/null
then
   bin/build_type.sh run_cmake $dismod_at_prefix $build_type
fi
# --------------------------------------------------------------------------
echo_eval cd build
if [ -e 'CMakeCache.txt' ]
then
   echo_eval rm CMakeCache.txt
fi
# --------------------------------------------------------------------------
# cmake_cxx_compiler
if echo $specific_compiler | grep 'CXX=' > /dev/null
then
   cxx=$(echo $specific_compiler | sed -e 's|.*CXX=\([^ ]*\).*|\1|')
   if ! which $cxx > /dev/null
   then
      echo "run_cmake.sh: specific_compiler: cannot execute $cxx compiler"
      exit 1
   fi
   cxx_path=$(which $cxx)
   cmake_cxx_compiler="-D CMAKE_CXX_COMPILER=$cxx_path"
else
   cmake_cxx_compiler=''
fi
# --------------------------------------------------------------------------
# cmake_c_compiler
if echo $specific_compiler | grep 'CC=' > /dev/null
then
   cc=$(echo $specific_compiler | sed -e 's|.*CC=\([^ ]*\).*|\1|')
   if ! which $cc > /dev/null
   then
      echo "run_cmake.sh: specific_compiler: cannot execute $cc compiler"
      exit 1
   fi
   c_path=$(which $cc)
   cmake_c_compiler="-D CMAKE_C_COMPILER=$c_path"
else
   cmake_c_compiler=''
fi
# --------------------------------------------------------------------------
cmake \
   -Wno-dev \
   -D CMAKE_VERBOSE_MAKEFILE=$verbose_makefile \
   -D CMAKE_BUILD_TYPE=$build_type \
   $cmake_cxx_compiler \
   $cmake_c_compiler \
   \
   -D python3_executable=$python3_executable \
   -D extra_cxx_flags="$extra_cxx_flags" \
   -D cmake_libdir="$cmake_libdir" \
   \
   -D dismod_at_prefix="$dismod_at_prefix" \
   -D system_specific_library_list="$system_specific_library_list" \
   ..
# --------------------------------------------------------------------------
echo 'run_cmake.sh: OK'
