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
# Test this version, not previous install
export PYTHONPATH=''
# ---------------------------------------------------------------------------
if [ "$0" != "bin/check_all.sh" ]
then
	echo 'bin/check_all.sh '
	echo 'must be executed from its parent directory'
	exit 1
fi
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# ----------------------------------------------------------------------------
# run cmake first so know right away if debug or release build
set +e
eval random_01="`expr $RANDOM % 2`"
set -e
if [ "$random_01" == '0' ]
then
	echo 'bin/run_cmake.sh >& cmake.log'
	bin/run_cmake.sh >& cmake.log
else
	echo 'bin/run_cmake.sh -switch_build_type >& cmake.log'
	bin/run_cmake.sh --switch_build_type >& cmake.log
fi
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
