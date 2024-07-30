#! /usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-24 Bradley M. Bell
set -e -u
# ----------------------------------------------------------------------------
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
while [ "$#" != 0 ]
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
#
# verbose_makefile
# buld_type
# dismod_at_prefix
# python3_executable
# specific_compiler
# extra_cxx_flags
# cmake_libdir
# system_specific_library_list
eval $(bin/install_settings.py)
# --------------------------------------------------------------------------
export PKG_CONFIG_PATH="$dismod_at_prefix/$cmake_libdir/pkgconfig"
# --------------------------------------------------------------------------
if echo "$dismod_at_prefix" | grep '/dismod_at$' > /dev/null
then
   bin/build_type.sh run_cmake $dismod_at_prefix $build_type
fi
# --------------------------------------------------------------------------
if [ ! -e build ]
then
   mkdir build
fi
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
