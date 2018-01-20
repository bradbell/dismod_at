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
echo 'bin/run_cmake.sh >& cmake.log'
bin/run_cmake.sh >& cmake.log
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
