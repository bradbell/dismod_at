#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
if [ $0 != 'bin/install_cppad.sh' ]
then
	echo 'bin/install_cppad.sh: must be executed from its parent directory'
	exit 1
fi
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# --------------------------------------------------------------------------
web_page='https://github.com/coin-or/CppAD.git'
hash_key='3e14c2759271e9b4549a1f30fa518a3dbcae74f2'
version='20170311'
libdir=`bin/libdir.sh`
# ---------------------------------------------------------------------------
# build_type
cmd=`grep '^build_type=' bin/run_cmake.sh`
eval $cmd
#
# cppad_prefix
cmd=`grep '^cppad_prefix=' bin/run_cmake.sh`
eval $cmd
#
# extra_cxx_flags
cmd=`grep '^extra_cxx_flags=' bin/run_cmake.sh`
eval $cmd
# --------------------------------------------------------------------------
if echo "$cppad_prefix" | grep '/dismod_at$' > /dev/null
then
	bin/build_type.sh install_cppad $cppad_prefix $build_type
fi
# --------------------------------------------------------------------------
if [ ! -e build/external ]
then
	mkdir -p build/external
fi
echo_eval cd build/external
# --------------------------------------------------------------------------
if [ ! -e cppad-$version ]
then
	echo_eval git clone $web_page cppad-$version
fi
pwd
#
echo_eval cd cppad-$version
echo_eval git checkout --quiet $hash_key
bin/version.sh set $version
bin/version.sh check
#
if [ ! -e build ]
then
	mkdir build
fi
echo_eval cd build
if [ -e CMakeCache.txt ]
then
	rm -r CMakeCache.txt
fi
# -----------------------------------------------------------------------------
cmake_args="-D CMAKE_VERBOSE_MAKEFILE=0"
cmake_args="$cmake_args -D cppad_prefix=$cppad_prefix"
cmake_args="$cmake_args -D cmake_install_libdirs=$libdir"
echo "cmake $cmake_args -D cppad_cxx_flags='$extra_cxx_flags' .."
cmake $cmake_args -D cppad_cxx_flags="$extra_cxx_flags" ..
#
echo_eval make install
# -----------------------------------------------------------------------------
echo 'install_cppad.sh: OK'
