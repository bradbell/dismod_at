#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# BEGIN USER_SETTINGS
# prefix below which cppad will be installed
cppad_prefix="$HOME/prefix/dismod_at"
# extra c++ flags used during compliation
extra_cxx_flags='-std=c++11 -Wall'
# END USER_SETTINGS
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
tarball='http://www.coin-or.org/download/source/CppAD/cppad-20150000.9.gpl.tgz'
# --------------------------------------------------------------------------
version=`echo $tarball | sed \
	-e 's|^.*/||' \
	-e 's|cppad-||' \
	-e 's|\.[a-z]*\.tgz||'`
# --------------------------------------------------------------------------
if [ ! -e build/external ]
then
	mkdir -p build/external
fi
cd build/external
# --------------------------------------------------------------------------
if [ ! -e cppad-$version.tgz ]
then
	echo_eval wget $tarball
fi
if [ -e cppad-$version ]
then
	echo_eval rm -rf cppad-$version
fi
local_tarball=`echo $tarball | sed -e 's|^.*/||'`
echo_eval tar -xzf $local_tarball
echo_eval cd cppad-$version
# -----------------------------------------------------------------------------
if [ -e /usr/lib64 ]
then
	libdirs="'lib64;lib'"
else
	libdirs="'lib;lib64'"
fi
mkdir build
echo_eval cd build
#
cmake_args="-D CMAKE_VERBOSE_MAKEFILE=0"
cmake_args="$cmake_args -D cmake_install_prefix=$cppad_prefix"
cmake_args="$cmake_args -D cmake_install_libdirs=$libdirs"
echo "cmake $cmake_args -D cppad_cxx_flags='$extra_cxx_flags' .."
cmake $cmake_args -D cppad_cxx_flags="$extra_cxx_flags" ..
#
echo_eval make install
