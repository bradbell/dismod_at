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
list=`ls bin/check_*.sh`
for script in $list
do
	if [ "$script" != 'bin/check_all.sh' ] \
	&& [ "$script" != 'bin/check_install.sh' ]
	then
		$script
	fi
done
#
# check version number
version.sh check
#
# check latex in omhelp
run_omhelp.sh -xml doc
# ----------------------------------------------------------------------------
#
cd build
#
echo 'make check >& check.log'
make check >& ../check.log
#
echo 'make speed >& speed.log'
make speed >& ../speed.log
#
cd ..
#
echo 'bin/check_install.sh >& install.log'
bin/check_install.sh >& install.log
# -----------------------------------------------------------------------------
for target in cmake check speed install
do
	if grep -i 'warning:' $target.log
	then
		echo "bin/run_check_all.sh: $target.log has warnings."
		exit 1
	fi
done
# -----------------------------------------------------------------------------
echo 'check_all.sh: OK'
