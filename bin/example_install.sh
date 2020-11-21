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
# $OMhelpKeyCharacter=&
# &begin example_install.sh&& &newlinech #&&
#
# &section An Example Installation&&
#
# &head Syntax&&
# &code bin/example_install.sh&&
#
# &srcthisfile%0%# BEGIN BASH%# END BASH%1%&&
# &end
# ----------------------------------------------------------------------------
# BEGIN BASH
#
# set build_type to value in run_cmake.sh
cmd=`grep '^build_type=' bin/run_cmake.sh`
eval $cmd
#
# set cppad_prefix to value in run_cmake.sh
cmd=`grep '^cppad_prefix=' bin/run_cmake.sh`
eval $cmd
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# --------------------------------------------------------------------------
# remove old version of example_install.log and example_install.err
for ext in log err
do
	if [ -e "example_install.$ext" ]
	then
		echo_eval rm "example_install.$ext"
	fi
done
# --------------------------------------------------------------------------
# set build link to build.debug or build.release depending on build_type
if echo "$cppad_prefix" | grep '/dismod_at$' > /dev/null
then
	bin/build_type.sh example_install.sh $cppad_prefix $build_type
fi
# -----------------------------------------------------------------------------
# install the special requirements
echo_eval bin/get_cppad_mixed.sh
# ----------------------------------------------------------------------------
# dismod_at
# -----------------------------------------------------------------------------
# Check we can find ipopt.pc, echo PKG_CONFIG_PATH to help user set this value
dir=`find -L $cppad_prefix -name 'ipopt.pc' | sed -e 's|/ipopt.pc||'`
if [ "$dir" == '' ]
then
	echo "Cannot find ipopt.pc in $cppad_prefix directory"
	exit 1
else
	echo 'pkg-config setting for ipopt'
	echo_eval export PKG_CONFIG_PATH="$dir"
fi
#
# bin/run_cmake.sh
echo "bin/run_cmake.sh 1>> example_install.log 2>> example_install.err"
bin/run_cmake.sh 1>> ../example_install.log 2>> ../example_install.err
#
# change into build directory
echo_eval cd build
#
# make check, speed, and install
for cmd in check speed install
do
	echo "make $cmd 1>> example_install.log 2>> example_install.err"
	make $cmd 1>> ../example_install.log 2>> ../example_install.err
done
cd ..
# -----------------------------------------------------------------------------
echo 'bin/example_install.sh: OK'
exit 0
# END BASH
