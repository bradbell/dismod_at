#! /bin/bash -e
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
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
hash_key='56ce3bdecfa933fc396416f3ff75e6b969b0b4d4'
version='20210515'
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
# dismod_at_prefix
cmd=`grep '^dismod_at_prefix=' bin/run_cmake.sh`
eval $cmd
#
# cmake_libdir
cmd=`grep '^cmake_libdir=' bin/run_cmake.sh`
eval $cmd
# ---------------------------------------------------------------------------
export LD_LIBRARY_PATH="$dismod_at_prefix/$cmake_libdir"
export DYLD_LIBRARY_PATH="$LD_LIBRARY_PATH"
export PKG_CONFIG_PATH="$LD_LIBRARY_PATH/pkgconfig"
# ---------------------------------------------------------------------------
# set build link to build.debug or build.release depending on build_type
if echo "$dismod_at_prefix" | grep '/dismod_at$' > /dev/null
then
	bin/build_type.sh example_install.sh $dismod_at_prefix $build_type
fi
# ---------------------------------------------------------------------------
# cd into external
if [ ! -e external ]
then
	mkdir external
fi
echo_eval cd external
# --------------------------------------------------------------------------
# clone cppad_mixed.git
if [ ! -e cppad_mixed.git ]
then
	echo_eval git clone $web_page cppad_mixed.git
fi
cd cppad_mixed.git
echo_eval git reset --hard
echo_eval git checkout master
echo_eval git pull --ff-only
echo_eval git checkout --quiet $hash_key
check=`grep '^SET(cppad_mixed_version' CMakeLists.txt | \
	sed -e 's|^[^"]*"\([^"]*\).*|\1|'`
if [ "$version" != "$check" ]
then
	echo 'get_cppad_mixed.sh: version number does not agree with hash_key'
	exit 1
fi
if [ "$build_type" == 'release' ]
then
	optimize='yes'
else
	optimize='no'
fi
# -----------------------------------------------------------------------------
# transfer cppad_py install options to cppad_mixed run_cmake.sh
dir=$(pwd)
echo "edit $dir/bin/run_cmake.sh"
sed \
	-e "s|^verbose_makefile=.*|verbose_makefile='no'|" \
	-e "s|^build_type=.*|build_type='$build_type'|" \
	-e "s|^cmake_install_prefix=.*|cmake_install_prefix='$dismod_at_prefix'|" \
	-e "s|^extra_cxx_flags=.*|extra_cxx_flags='$extra_cxx_flags'|" \
	-e "s|^cmake_libdir=.*|cmake_libdir='$cmake_libdir'|" \
	-e "s|^ldlt_cholmod=.*|ldlt_cholmod='yes'|" \
	-e "s|^optimize_cppad_function=.*|optimize_cppad_function='$optimize'|" \
	-e "s|^for_hes_sparsity=.*|for_hes_sparsity='yes'|" \
	bin/run_cmake.sh > run_cmake.$$
mv run_cmake.$$ bin/run_cmake.sh
chmod +x bin/run_cmake.sh
#
# check edit
list="
	^verbose_makefile='no'
	^build_type='$build_type'
	^cmake_install_prefix='$dismod_at_prefix'
	^cmake_libdir='$cmake_libdir'
	^ldlt_cholmod='yes'
	^optimize_cppad_function='$optimize'
	^for_hes_sparsity='yes'
"
for pattern in $list
do
	if ! grep "$pattern" bin/run_cmake.sh > /dev/null
	then
		echo "pattern=$pattern"
		echo "get_cppad_mixed.sh: Edit of $dir/bin/run_cmake.sh failed"
		exit 1
	fi
done
# $extra_cxx_flags my have spaces in it
if ! grep "^extra_cxx_flags='$extra_cxx_flags'"  bin/run_cmake.sh > /dev/null
then
	echo "get_cppad_mixed.sh: Edit of $dir/bin/run_cmake.sh failed"
	exit 1
fi
# -----------------------------------------------------------------------------
# cppad_mixed example install
run_test='false'
replace='false'
echo_eval bin/example_install.sh $run_test $replace
#
# -----------------------------------------------------------------------------
echo 'get_cppad_mixed.sh: OK'
exit 0
