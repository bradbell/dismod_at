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
# /bin/bash -e
# determine if we should use lib or lib64
if [ "$0" != "bin/libdir.sh" ]
then
	echo "bin/libdir.sh: must be executed from its parent directory"
	exit 1
fi
if [ -e /usr/lib64 ]
then
    echo 'lib64'
else
    echo 'lib'
fi

