#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-19 University of Washington
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
hash_key='80fea7b62d67e6fc25e6b93f92f471faec5cd844'
version='20191008'
# ---------------------------------------------------------------------------
# special cppad_mixed settings (it is not suggested that you change these)
#
# use bool_sparsity yes or no
bool_sparsity='no'
#
# use cholmod (not eigen) for LDLT factorization (where possible)
ldlt_cholmod='yes'
#
# use for_hes_sparsity (rev_hes_sparsity) for Hessian w.r.t random effects
for_hes_sparsity='yes'
#
# ---------------------------------------------------------------------------
# Get user configuration options from run_cmake.sh
#
# build_type
cmd=`grep '^build_type=' bin/run_cmake.sh`
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
# cmake_cxx_compiler
cmd=`grep '^cmake_cxx_compiler=' bin/run_cmake.sh`
eval $cmd
#
# cmake_libdir
cmd=`grep '^cmake_libdir=' bin/run_cmake.sh`
eval $cmd
# ---------------------------------------------------------------------------
ipopt_pkg_config_path="$ipopt_prefix/$cmake_libdir/pkgconfig"
if ! echo $PKG_CONFIG_PATH | grep $ipopt_pkg_config_path  > /dev/null
then
    if [ "$PKG_CONFIG_PATH" == '' ]
    then
        export PKG_CONFIG_PATH="$ipopt_pkg_config_path"
    else
        export PKG_CONFIG_PATH="$ipopt_pkg_config_path;$PKG_CONFIG_PATH"
    fi
fi
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
if [ ! -e build/external ]
then
	mkdir -p build/external
fi
echo_eval cd build/external
# --------------------------------------------------------------------------
if [ ! -e cppad_mixed.git ]
then
	echo_eval git clone $web_page cppad_mixed.git
fi
cd cppad_mixed.git
echo_eval git checkout master
echo_eval git pull
echo_eval git checkout --quiet $hash_key
check=`grep '^SET(cppad_mixed_version' CMakeLists.txt | \
	sed -e 's|^[^"]*"\([^"]*\).*|\1|'`
if [ "$version" != "$check" ]
then
	echo 'install_cppad_mixed.sh: version number does not agree with hash_key'
	exit 1
fi
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
#
cmake_args="$cmake_args -D cmake_libdir=$cmake_libdir"
cmake_args="$cmake_args -D bool_sparsity=$bool_sparsity"
cmake_args="$cmake_args -D ldlt_cholmod=$ldlt_cholmod"
cmake_args="$cmake_args -D optimize_cppad_function=$optimize_cppad_function"
cmake_args="$cmake_args -D for_hes_sparsity=$for_hes_sparsity"
#
if [ "$cmake_cxx_compiler" != '' ]
then
	cmake_args="$cmake_args -D CMAKE_CXX_COMPILER=$cmake_cxx_compiler"
fi
echo "cmake $cmake_args -D extra_cxx_flags=\"$extra_cxx_flags\" .."
cmake $cmake_args -D extra_cxx_flags="$extra_cxx_flags" ..
# -----------------------------------------------------------------------------
echo_eval make install
# -----------------------------------------------------------------------------
echo 'install_cppad_mixed.sh: OK'
