#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-16 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# BEGIN USER_SETTINGS
# build type can be debug or release
build_type='debug'
#
# use dismod_at prefix for packages
cppad_prefix="$HOME/prefix/dismod_at"
ipopt_prefix="$HOME/prefix/dismod_at"
suitesparse_prefix="$HOME/prefix/dismod_at"
eigen_prefix="$HOME/prefix/dismod_at"
# note eigen actually is assumed to be in $eigen_prefix/eigen
#
# extra c++ flags used during compliation
extra_cxx_flags='-std=c++11 -Wall'
#
# use bool_sparsity YES or NO
bool_sparsity='YES'
#
# use cholmod (not eigen) for LDLT factorization (where possible)
ldlt_cholmod='YES'
#
# put fatal error messages in log table or generate asserts
log_fatal_error='YES'
#
# which c++ compiler to use (empty means cmake will choose it)
cmake_cxx_compiler=''
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
hash_key='781af73035b02d709d444a218c6de8d8e84c48bc'
version='20161018'
libdir=`bin/libdir.sh`
# --------------------------------------------------------------------------
if echo "$cppad_prefix" | grep '/dismod_at$' > /dev/null
then
	bin/build_type.sh install_cppad_mixed $cppad_prefix $build_type
fi
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
cmake_args="-D CMAKE_VERBOSE_MAKEFILE=NO"
cmake_args="$cmake_args -D CMAKE_BUILD_TYPE=$build_type"
#
cmake_args="$cmake_args -D cppad_prefix=$cppad_prefix"
cmake_args="$cmake_args -D ipopt_prefix=$ipopt_prefix"
cmake_args="$cmake_args -D eigen_prefix=$eigen_prefix/eigen"
cmake_args="$cmake_args -D suitesparse_prefix=$suitesparse_prefix"
#
cmake_args="$cmake_args -D extra_cxx_flags=$extra_cxx_flags"
cmake_args="$cmake_args -D cmake_libdir=$libdir"
cmake_args="$cmake_args -D bool_sparsity=$bool_sparsity"
cmake_args="$cmake_args -D ldlt_cholmod=$ldlt_cholmod"
cmake_args="$cmake_args -D log_fatal_error=$log_fatal_error"
if [ "$cmake_cxx_compiler" != '' ]
then
	cmake_args="$cmake_args -D CMAKE_CXX_COMPILER=$cmake_cxx_compiler"
fi
echo "cmake $cmake_args .."
cmake $cmake_args ..
# -----------------------------------------------------------------------------
echo_eval make install
# -----------------------------------------------------------------------------
echo 'install_cppad_mixed.sh: OK'
