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
# Prefix below which eigen will be installed. Note that eigen_prefix/eigen
# is actually used so we can suppress warnings for the eigen include files.
# If this directory ends with /dismod_at, separate directories are used
# for the debug and release versions.
eigen_prefix="$HOME/prefix/dismod_at"
# END USER_SETTINGS
# ---------------------------------------------------------------------------
if [ $0 != 'bin/install_eigen.sh' ]
then
	echo 'bin/install_eigen.sh: must be executed from its parent directory'
	exit 1
fi
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# ---------------------------------------------------------------------------
version='3.2.9'
web_page='https://bitbucket.org/eigen/eigen/get'
# --------------------------------------------------------------------------
if echo "$eigen_prefix" | grep '/dismod_at$' > /dev/null
then
	bin/build_type.sh install_eigen $eigen_prefix $build_type
fi
# --------------------------------------------------------------------------
if [ ! -e build/external ]
then
	mkdir -p build/external
fi
cd build/external
# --------------------------------------------------------------------------
if [ ! -e eigen-$version.tar.gz ]
then
	echo_eval wget $web_page/$version.tar.gz
	echo_eval mv $version.tar.gz eigen-$version.tar.gz
fi
if [ -e "eigen-$version" ]
then
	echo_eval rm -rf eigen-$version
fi
#
echo_eval tar -xzf eigen-$version.tar.gz
git_name=`ls | grep eigen-eigen`
echo_eval mv $git_name eigen-$version
# --------------------------------------------------------------------------
echo_eval cd eigen-$version
#
echo_eval mkdir build
echo_eval cd build
# --------------------------------------------------------------------------
echo_eval cmake \
	-Wno-dev \
	-D CMAKE_INSTALL_PREFIX=$eigen_prefix/eigen \
	-D CMAKE_BUILD_TYPE=$build_type \
	..
echo_eval make install
# --------------------------------------------------------------------------
include_dir="$eigen_prefix/eigen/include"
if [ ! -h $include_dir/Eigen ]
then
	echo_eval ln -s $include_dir/eigen3/Eigen $include_dir/Eigen
fi
# --------------------------------------------------------------------------
echo 'install_eigen.sh: OK'
