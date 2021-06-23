#! /bin/bash -e
# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
if [ "$0" != "bin/check_build_dir.sh" ]
then
	echo "bin/check_build_dir.sh: must be executed from its parent directory"
	exit 1
fi
# -----------------------------------------------------------------------------
if git grep 'build/example/user' test/user
then
	echo 'check_build_dir.sh: build/example/user occurs in test/user/*'
	exit 1
fi
if git grep 'build/test/user' example/user
then
	echo 'check_build_dir.sh: build/test/user occurs in example/user/*'
	exit 1
fi
echo 'check_build_dir.sh: OK'
exit 0
