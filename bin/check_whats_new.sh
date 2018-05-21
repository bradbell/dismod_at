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
if [ "$0" != "bin/check_whats_new.sh" ]
then
	echo "bin/check_whats_new.sh: must be executed from its parent directory"
	exit 1
fi
# -----------------------------------------------------------------------------
yyyy=`date +%Y`
file='omh/install/unix.omh'
if ! grep "\$cref whats_new_$yyyy\$\$ release notes" $file > /dev/null
then
	echo "cannot find: \$cref whats_new_$yyyy\$\$ release notes"
	echo "in file:     $file"
	exit 1
fi
# -----------------------------------------------------------------------------
echo 'check_whats_new.sh: OK'
exit 0

