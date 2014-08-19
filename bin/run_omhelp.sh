#! /bin/bash -e
# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2014-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
if [ "$0" != 'bin/run_omhelp.sh' ]
then
	echo 'bin/run_omhelp.sh must be run from its parent directory'
	exit 1
fi
if [ -e doc ]
then
	rm -r doc
fi
mkdir doc
echo 'cd doc'
cd doc
#
echo "omhelp ../doc.omh -debug -noframe -xml > ../omhelp.log"
if ! omhelp ../doc.omh -debug -noframe -xml > ../omhelp.log
then
	cat ../omhelp.log
	exit 1
fi
#
if grep '^OMhelp Warning:' ../omhelp.log
then
	exit 1
else
	echo 'run_omhelp.sh: OK'
fi
exit 0
