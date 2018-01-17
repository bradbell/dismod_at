#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Test this version, not previous install
export PYTHONPATH=''
# ---------------------------------------------------------------------------
if [ "$0" != "bin/check_all.sh" ]
then
	echo 'bin/check_all.sh (+speed | -speed)'
	echo 'must be executed from its parent directory'
	exit 1
fi
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
bin/version.sh check
bin/check_devel_xam.sh
bin/check_include.sh
bin/check_srcfile.sh
bin/check_configure.sh
#
# check latex in omhelp
run_omhelp.sh -xml doc
# ----------------------------------------------------------------------------
echo 'bin/run_cmake.sh 1> cmake.log 2> cmake.err'
bin/run_cmake.sh 1> cmake.log 2> cmake.err
#
cd build
#
echo 'make check 1> check.log 2> check.err'
make check 1> check.log 2> ../check.err
#
echo 'make speed 1> speed.log 2> speed.err'
make speed 1> speed.log 2> ../speed.err
#
cd ..
#
echo 'bin/check_install.sh 1> install.log 2> install.err'
bin/check_install.sh 1> install.log 2> install.err
# -----------------------------------------------------------------------------
for target in cmake check speed install
do
	err=`cat $target.err`
	if [ "$err" != '' ]
	then
		cat $target.err
		echo "bin/run_check_all.sh: $target.err is not empty."
		exit 1
	fi
	echo_eval rm $target.err
done
# -----------------------------------------------------------------------------
echo 'check_all.sh: OK'
