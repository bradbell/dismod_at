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
# Test this version, not previous install
export PYTHONPATH=''
# ---------------------------------------------------------------------------
if [ "$0" != "bin/check_all.sh" ]
then
	echo 'bin/check_all.sh (+speed | -speed)'
	echo 'must be executed from its parent directory'
	exit 1
fi
# ---------------------------------------------------------------------------
if [ "$1" != '+speed' ] && [ "$1" != '-speed' ]
then
	echo 'check_all.sh: option is not +speed or -speed'
	exit 1
fi
bin/version.sh check
bin/check_devel_xam.sh
bin/check_include.sh
bin/check_srcfile.sh
bin/check_configure.sh
#
run_omhelp.sh -xml doc
bin/run_cmake.sh
# ----------------------------------------------------------------------------
cd build
echo "make check >& check_all.log"
if ! make check >& ../check_all.log
then
	echo 'check_all.sh: make check failed, see check_all.log'
	exit 1
fi
if grep ': *warning *:' ../check_all.log
then
	echo 'check_all.sh: aborting: see compiler warnings in check_all.log'
	exit 1
fi
if [ "$input" == '+speed' ]
then
	make speed
fi
# ----------------------------------------------------------------------------
cd ..
#
bin/check_install.sh
# -----------------------------------------------------------------------------
echo 'check_all.sh: OK'
