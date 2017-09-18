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
if [ "$0" != 'bin/ihme_db.sh' ] || [ "$#" != '2' ]
then
	echo 'usage: bin/ihme_db.sh direction database_path'
	echo 'direction:     is get or put'
	echo 'database_path: is an ihme directory following by database name'
	echo 'alex directory: /snfs1/Project/dismod_at/test_databases'
	echo 'greg directory: /snfs2/HOME/gma1/tmp'
	exit 1
fi
direction="$1"
database_path="$2"
database=`echo $database_path | sed -e 's|.*/||'`
# ---------------------------------------------------------------------------
if [ "$direction" != 'get' ] && [ "$direction" != 'put' ]
then
	echo 'ihme_db.sh: direction is not "get" or "put"'
	exit 1
fi
# ---------------------------------------------------------------------------
if [ "$direction" == 'get' ] && [ ! -e "$database_path" ]
then
	echo "$database_path: does not exist"
	exit 1
fi
if [ "$direction" == 'put' ] && [ ! -e "imhe_db/$database" ]
then
	echo "build/ihme_db/$database: does not exist"
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
	if [ ! -e build/ihme_db ]
	then
		echo_eval mkdir -p build/ihme_db
	fi
	echo_eval cp $database_path build/ihme_db/$database
else
	echo_eval cp build/ihme_db/$database $database
fi
# ---------------------------------------------------------------------------
echo 'bin/ihme_db.sh: OK'
exit 0
