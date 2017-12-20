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
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
if [ "$0" != 'bin/ihme_db.sh' ]
then
	echo 'bin/ihme_db.sh: must be executed from its parent directory'
	exit 1
fi
ok='yes'
person="$1"
database="$2"
direction="$3"
if [ "$4" != '' ]
then
	ok='no'
fi
if [ "$person" == 'greg' ]
then
	database_path="/snfs2/HOME/gma1/tmp/$database"
elif [ "$person" == 'alex' ]
then
	database_path="/snfs1/Project/dismod_at/test_databases/$database"
else
	ok='no'
fi
if [ "$direction" != 'get' ] && [ "$direction" != 'put' ]
then
	ok='no'
fi
if [ "$ok" == 'no' ]
then
cat << EOF
usage: bin/ihme_db.sh person database direction'
       person:    is alex or greg
       database:  file name of the database in directory used by person.
       direction: is get or put
EOF
	exit 1
fi
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
