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
# BEGIN BASH
# ---------------------------------------------------------------------------
# Get user configuration options from run_cmake.sh
#
# build_type
cmd=`grep '^build_type=' bin/run_cmake.sh`
eval $cmd
#
# ipopt_prefix
cmd=`grep '^ipopt_prefix=' bin/run_cmake.sh`
eval $cmd
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
# check for old log and error files
if [ -e install_all.log ]
then
	echo_eval rm install_all.log
fi
if [ -e install_all.err ]
then
	echo_eval rm install_all.err
fi
# -----------------------------------------------------------------------------
list="
	eigen
	ipopt
	suitesparse
	cppad
	cppad_mixed
"
for package in $list
do
	program="bin/install_$package.sh"
	echo "install_$package.sh 1>> install_all.log 2>> install_all.err"
	$program 1>> install_all.log 2>> install_all.err
done
# -----------------------------------------------------------------------------
dir=`find -L $ipopt_prefix -name 'ipopt.pc' | sed -e 's|/ipopt.pc||'`
if [ "$dir" == '' ]
then
	echo "Cannot find ipopt.pc in $ipopt_prefix directory"
	exit 1
else
	echo_eval export PKG_CONFIG_PATH="$dir"
fi
echo_eval export PYTHONPATH=''
# -----------------------------------------------------------------------------
# build and install dismod_at
echo_eval bin/run_cmake.sh
echo_eval cd build
make check
make speed
make install
cd ..
# -----------------------------------------------------------------------------
echo 'install_all.sh: OK'
# END BASH
