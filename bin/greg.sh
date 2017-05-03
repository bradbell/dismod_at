#! /bin/bash -eu
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
remote_dir='/snfs2/HOME/gma1/tmp'
local_dir='greg'
# ---------------------------------------------------------------------------
if [ "$0" != 'bin/greg.sh' ] || [ "$#" != '2' ]
then
	echo 'usage: bin/greg.sh direction database'
	echo 'where direction is get or put (gregs file system)'
	exit 1
fi
direction="$1"
database="$2"
if [ "$direction" != 'get' ] && [ "$direction" != 'put' ]
then
	echo 'greg.sh: direction is not "get" or "put"'
	exit 1
fi
# ---------------------------------------------------------------------------
if [ "$direction" == 'get' ] && [ ! -e "$remote_dir/$database" ]
then
	echo "$remote_dir/$database: does not exist"
	exit 1
fi
if [ "$direction" == 'put' ] && [ ! -e "$local_dir/$database" ]
then
	echo "$local_dir/$database: does not exist"
	exit 1
fi
if [ ! -e bin/dismodat.py ]
then
	echo 'bin/dismodat.py does not exist'
	exit 1
fi
# ---------------------------------------------------------------------------
if [ "$direction" == 'get' ]
then
	if [ ! -e greg ]
	then
		echo_eval mkdir -p greg
	fi
	echo_eval cp $remote_dir/$database $local_dir/$database
else
	echo_eval cp $local_dir/$database $remote_dir/brad/$database
fi
# ---------------------------------------------------------------------------
echo 'bin/greg.sh: OK'
exit 0
