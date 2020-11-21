#! /bin/bash -e
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
#
# {xsrst_comment_ch #}
#
# {xsrst_begin get_cppad_mixed_sh}
#
# .. include:: ../preamble.rst
#
# Get cppad_mixed
# ###############
#
# Syntax
# ******
# ``bin/get_cppad_mixed.sh``
#
# Top Source Directory
# ********************
# This program must be run from the
# :ref:`top_source_directory<setup_py.download.top_source_directory>`.
#
# Purpose
# *******
# If you are going to use the python ``cppad_mixed`` module,
# you will need to run this script to install the corresponding
# C++ module. This script includes the installation of cppad so it is not
# necessary to also run :ref:`get_cppad.sh<get_cppad.sh>`.
#
# Settings
# ********
# This scripts uses the
# :ref:`get_cppad.sh settings<get_cppad_sh.settings>` for
# *cmake_install_prefix* , *extra_cxx_flags*, and *build_type* .
#
# {xsrst_end get_cppad_mixed_sh}
# ---------------------------------------------------------------------------
# CppAD mixed version information
web_page='https://github.com/bradbell/cppad_mixed.git'
hash_key='c1ed84c888406800e401c57ad2b4e01a2b827771'
version='20201119'
# --------------------------------------------------------------------------
name='bin/get_cppad_mixed.sh'
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
# build_type
cmd=`grep '^build_type=' bin/run_cmake.sh`
eval $cmd
#
# extra_cxx_flags
cmd=`grep '^extra_cxx_flags=' bin/run_cmake.sh`
eval $cmd
#
# cppad_prefix
cmd=`grep '^cppad_prefix=' bin/run_cmake.sh`
eval $cmd
#
# cmake_libdir
cmd=`grep '^cmake_libdir=' bin/run_cmake.sh`
eval $cmd
# ---------------------------------------------------------------------------
export LD_LIBRARY_PATH="$cppad_prefix/$cmake_libdir"
export DYLD_LIBRARY_PATH="$LD_LIBRARY_PATH"
export PKG_CONFIG_PATH="$LD_LIBRARY_PATH/pkgconfig"
# ---------------------------------------------------------------------------
# set build link to build.debug or build.release depending on build_type
if echo "$cppad_prefix" | grep '/dismod_at$' > /dev/null
then
	bin/build_type.sh example_install.sh $cppad_prefix $build_type
fi
# ---------------------------------------------------------------------------
# cd into build/external
if [ ! -e build/external ]
then
    mkdir build/external
fi
echo_eval cd build/external
# --------------------------------------------------------------------------
# clone cppad_mixed.git
if [ ! -e cppad_mixed.git ]
then
    echo_eval git clone $web_page cppad_mixed.git
fi
cd cppad_mixed.git
git reset --hard
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
if [ "$build_type" == 'release' ]
then
    optimize='yes'
else
    optimize='no'
fi
# install options
echo 'edit build/external/cppad_mixed.git/bin/run_cmake.sh.sh'
sed \
    -e "s|^verbose_makefile=.*|verbose_makefile='no'|" \
    -e "s|^build_type=.*|build_type='$build_type'|" \
    -e "s|^cmake_install_prefix=.*|cmake_install_prefix='$cppad_prefix'|" \
    -e "s|^extra_cxx_flags=.*|extra_cxx_flags='$extra_cxx_flags'|" \
    -e "s|^cmake_libdir=.*|cmake_libdir='$cmake_libdir'|" \
    -e "s|^ldlt_cholmod=.*|ldlt_cholmod='yes'|" \
    -e "s|^optimize_cppad_function=.*|optimize_cppad_function='$optimize'|" \
    -e "s|^for_hes_sparsity=.*|for_hes_sparsity='yes'|" \
    bin/run_cmake.sh > run_cmake.$$
mv run_cmake.$$ bin/run_cmake.sh
chmod +x bin/run_cmake.sh
#
# supress call to cppad_mixed build_type.sh
echo 'edit build/external/cppad_mixed.git/bin/example_install.sh'
sed \
    -e 's|bin/build_type.sh .*|:|' \
    -e 's|for cmd in check speed install|for cmd in install|' \
    bin/example_install.sh > example_install.$$
mv example_install.$$ bin/example_install.sh
chmod +x bin/example_install.sh
# -----------------------------------------------------------------------------
# cppad_mixed example install
echo_eval bin/example_install.sh replace
#
# -----------------------------------------------------------------------------
echo 'get_cppad_mixed.sh: OK'
exit 0
