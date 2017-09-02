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
hash_key='d82c11ad027cdc473f7d99be5018e4ddb5f27ba5'
version='20170801'
libdir=`bin/libdir.sh`
# ---------------------------------------------------------------------------
# special cppad_mixed settings (it is not suggested that you change these)
#
# use bool_sparsity yes or no
bool_sparsity='no'
#
# use cholmod (not eigen) for LDLT factorization (where possible)
ldlt_cholmod='yes'
#
# use atomic Cholesky factorization (strongly suggest no)
use_atomic_cholesky='no'
#
# use checkpointing of Newton step to reduce memory and increase execution time
checkpoint_newton_step='no'
#
# show ipopt the cppad_mixed re-scaling
hide_ipopt_scaling='yes'
#
# ---------------------------------------------------------------------------
# Get user configuration options from run_cmake.sh
#
# build_type
cmd=`grep '^build_type=' bin/run_cmake.sh`
eval $cmd
#
# log_fatal_error
cmd=`grep '^log_fatal_error=' bin/run_cmake.sh`
eval $cmd
#
# extra_cxx_flags
cmd=`grep '^extra_cxx_flags=' bin/run_cmake.sh`
eval $cmd
#
# eigen_prefix
cmd=`grep '^eigen_prefix=' bin/run_cmake.sh`
eval $cmd
#
# ipopt_prefix
cmd=`grep '^ipopt_prefix=' bin/run_cmake.sh`
eval $cmd
#
# cppad_prefix
cmd=`grep '^cppad_prefix=' bin/run_cmake.sh`
eval $cmd
#
# suitesparse_prefix
cmd=`grep '^suitesparse_prefix=' bin/run_cmake.sh`
eval $cmd
#
# cmake_cxx_compiler
cmd=`grep '^cmake_cxx_compiler=' bin/run_cmake.sh`
eval $cmd
# ---------------------------------------------------------------------------
# optimize the AD operation sequences
if [ "$build_type" == 'debug' ]
then
	optimize_cppad_function='no'
elif [ "$build_type" == 'release' ]
then
	optimize_cppad_function='yes'
else
	echo "bin/run_cmake.sh: build_type is not 'debug' or 'release'"
	exit 1
fi
# --------------------------------------------------------------------------
if echo "$cppad_prefix" | grep '/dismod_at$' > /dev/null
then
	bin/build_type.sh install_cppad_mixed $cppad_prefix $build_type
fi
# --------------------------------------------------------------------------
libdir=`bin/libdir.sh`
export PKG_CONFIG_PATH="$ipopt_prefix/$libdir/pkgconfig"
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
cmake_args="-D CMAKE_VERBOSE_MAKEFILE=no"
cmake_args="$cmake_args -D CMAKE_BUILD_TYPE=$build_type"
#
cmake_args="$cmake_args -D cppad_prefix=$cppad_prefix"
cmake_args="$cmake_args -D ipopt_prefix=$ipopt_prefix"
cmake_args="$cmake_args -D eigen_prefix=$eigen_prefix"
cmake_args="$cmake_args -D suitesparse_prefix=$suitesparse_prefix"
#
cmake_args="$cmake_args -D cmake_libdir=$libdir"
cmake_args="$cmake_args -D bool_sparsity=$bool_sparsity"
cmake_args="$cmake_args -D ldlt_cholmod=$ldlt_cholmod"
cmake_args="$cmake_args -D log_fatal_error=$log_fatal_error"
cmake_args="$cmake_args -D use_atomic_cholesky=$use_atomic_cholesky"
cmake_args="$cmake_args -D checkpoint_newton_step=$checkpoint_newton_step"
cmake_args="$cmake_args -D optimize_cppad_function=$optimize_cppad_function"
cmake_args="$cmake_args -D hide_ipopt_scaling=$hide_ipopt_scaling"
#
if [ "$cmake_cxx_compiler" != '' ]
then
	cmake_args="$cmake_args -D CMAKE_CXX_COMPILER=$cmake_cxx_compiler"
fi
echo "cmake $cmake_args -D cppad_cxx_flags=\"$extra_cxx_flags\" .."
cmake $cmake_args -D cppad_cxx_flags="$extra_cxx_flags" ..
# -----------------------------------------------------------------------------
echo_eval make install
# -----------------------------------------------------------------------------
echo 'install_cppad_mixed.sh: OK'
