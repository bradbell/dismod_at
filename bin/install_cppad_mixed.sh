#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# BEGIN USER_SETTINGS
# type of build DEBUG or RELEASE
cmake_build_type='DEBUG'
# extra c++ flags used during compliation
extra_cxx_flags='-std=c++11 -Wall'
# prefix for packages
cppad_prefix="$HOME/prefix/dismod_at"
ipopt_prefix="$HOME/prefix/dismod_at"
eigen_prefix="$HOME/prefix/dismod_at"
# use set_sparsity YES or NO
set_sparsity='NO'
# directory below prefix with libraries are installed
cmake_libdir='lib64'
# ----------------------------------------------------------------------------
# setings for IHME cluster
# extra_cxx_flags='-Wall'
# END USER_SETTINGS
# ---------------------------------------------------------------------------
name='bin/install_cppad_mixed.sh'
if [ $0 != $name ]
then
	echo "$name: must be executed from its parent directory"
	exit 1
fi
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# --------------------------------------------------------------------------
web_page='https://github.com/bradbell/cppad_mixed.git'
hash_key='325f4e05f5ce2f580091c9f2f5810466368ed6b8'
version='20151214'
# --------------------------------------------------------------------------
if [ ! -e build/external ]
then
	mkdir -p build/external
fi
echo_eval cd build/external
# --------------------------------------------------------------------------
if [ ! -e cppad_mixed-$version ]
then
	echo_eval git clone $web_page cppad_mixed-$version
fi
echo_eval cd cppad_mixed-$version
if [ "$hash_key" != '' ]
then
	echo_eval git checkout --quiet $hash_key
else
	echo_eval git pull
fi
check=`bin/version.sh get`
if [ "$check" != "$version" ]
then
	bin/version.sh set $version
fi
bin/version.sh copy
#
if [ ! -e build ]
then
	mkdir build
fi
echo_eval cd build
if [ -e CMakeCache.txt ]
then
	echo_eval rm CMakeCache.txt
fi
# -----------------------------------------------------------------------------
if [ -e /usr/lib64 ]
then
	libdirs="'lib64;lib'"
else
	libdirs="'lib;lib64'"
fi
#
cmake_args="-D CMAKE_VERBOSE_MAKEFILE=NO"
cmake_args="$cmake_args -D CMAKE_BUILD_TYPE=$cmake_build_type"
cmake_args="$cmake_args -D extra_cxx_flags=$extra_cxx_flags"
cmake_args="$cmake_args -D cppad_prefix=$cppad_prefix"
cmake_args="$cmake_args -D ipopt_prefix=$ipopt_prefix"
cmake_args="$cmake_args -D eigen_prefix=$eigen_prefix"
cmake_args="$cmake_args -D set_sparsity=$set_sparsity"
cmake_args="$cmake_args -D cmake_libdir=$cmake_libdir"
echo "cmake $cmake_args .."
cmake $cmake_args ..
# -----------------------------------------------------------------------------
echo_eval make install
# -----------------------------------------------------------------------------
echo 'install_cppad_mixed.sh: OK'
