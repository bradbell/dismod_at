#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
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
	echo 'bin/check_all.sh build_type'
	echo 'must be executed from its parent directory'
	exit 1
fi
if [ "$1" != 'debug' ] && [ "$1" != 'release' ]
then
	echo 'bin/check_all.sh build_type'
	echo 'build_type is not debug or release'
	exit 1
fi
build_type="$1"
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
# run bin/check_*.sh and ~/bradbell/bin/check_copyright.sh
list=`ls bin/check_*.sh | sed \
	-e '/check_all.sh/d' -e '/check_install.sh/d' -e '/junk.sh/d' `
for script in $list
do
	$script
done
#
# check version number
echo_eval version.sh check
#
# build developer documentation and check latex
echo_eval run_omhelp.sh dev
#
# build user documentation
echo_eval run_omhelp.sh doc
# ----------------------------------------------------------------------------
# run cmake
sed -i bin/run_cmake.sh -e "s|^build_type=.*|build_type='$build_type'|"
#
echo 'bin/run_cmake.sh >& cmake.log'
bin/run_cmake.sh >& cmake.log
# ----------------------------------------------------------------------------
#
echo_eval cd build
#
echo 'make check >& check.log'
make check >& ../check.log
#
echo 'make speed >& speed.log'
make speed >& ../speed.log
#
echo_eval cd ..
#
echo 'bin/check_install.sh >& install.log'
bin/check_install.sh >& install.log
# -----------------------------------------------------------------------------
# check.log output is generalted by bin/user_test.sh which checks for warnings
# and re-runs. If a warning occurs a second time, bin/user_tes.sh error exits.
for target in cmake speed install
do
	if grep -i 'warning:' $target.log
	then
		echo "bin/run_check_all.sh: $target.log has warnings."
		exit 1
	fi
done
# -----------------------------------------------------------------------------
echo 'check_all.sh: OK'
exit 0
