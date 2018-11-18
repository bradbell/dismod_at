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
direction="$1"
person="$2"
database="$3"
if [ "$4" != '' ]
then
	ok='no'
fi
case $person in
	greg)
	database_path="/ihme/homes/gma1/tmp/$database"
	;;

	katie)
	dir='/ihme/Project/dismod_at/shiny/gbd_dismod_at_viz/sqlite_files'
	database_path="$dir/$database"
	;;

	*)
	ok='no'
esac
if [ "$direction" != 'get' ] && [ "$direction" != 'put' ]
then
	ok='no'
fi
if [ "$ok" == 'no' ]
then
cat << EOF
usage: bin/ihme_db.sh direction person database'
       direction: get or put
       person:    greg, or katie
       database:  file name of the database in directory used by person.
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
	echo_eval chmod -x build/ihme_db/$database
else
	echo_eval cp build/ihme_db/$database $database
fi
# ---------------------------------------------------------------------------
echo 'bin/ihme_db.sh: OK'
exit 0
