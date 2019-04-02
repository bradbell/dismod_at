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
# -----------------------------------------------------------------------------
if [ ! -d '/ihme' ]
then
	echo 'bin/ihme_db.sh: The directory /ihme does not exist.'
	echo 'perhaps you need to mount it.'
	exit 1
fi
# -----------------------------------------------------------------------------
if ! which dismodat.py >& /dev/null
then
	echo 'bin/ihme_db.sh: cannot find the the program dismodat.py'
	echo 'You must add its location to you unix PATH shell variable'
	exit 1
fi
# -----------------------------------------------------------------------------
ihme_path='
	/ihme/homes/gma1/tmp
	/ihme/epi/at_cascade/prod
'
if [ "$1" == '' ]
then
cat << EOF
usage: bin/ihme_db.sh relative_path

makes a copy of a database in the following location
	build/ihme_db/relative_path
where relative_path is a path relative to one of the following directories:
EOF
	for path in $ihme_path
	do
		echo "	$path"
	done
	exit 1
fi
relative_path="$1"
# -----------------------------------------------------------------------------
full_path=''
for path in $ihme_path
do
	if [ -e "$path/$relative_path" ]
	then
		if [ "$full_path" != '' ]
		then
			echo 'bin/ihme_db.sh: Found two matching databases:'
			echo "	$full_path"
			echo "	$path/$relative_path"
			echo 'Use a longer relative path so they are different'
			exit 1
		fi
		full_path="$path/$relative_path"
	fi
done
if [ "$full_path" == '' ]
then
	echo 'bin/ihme_db.sh: Cannot find either of the following databases:'
	for path in $ihme_path
	do
		echo "	$path/$relative_path"
	done
	exit 1
fi
# -----------------------------------------------------------------------------
local_dir=`echo $relative_path | sed -e 's|/[^/]*$||'`
local_file=`echo $relative_path | sed -e 's|.*/||'`
if [ "$local_dir" == "$relative_path" ]
then
	local_dir='build/ihme_db'
else
	local_dir="build/ihme_db/$local_dir"
fi
if [ ! -e "$local_dir" ]
then
	echo_eval mkdir -p "$local_dir"
fi
echo_eval cd $local_dir
echo_eval cp $full_path $local_file
echo_eval dismodat.py $local_file db2csv
# ---------------------------------------------------------------------------
echo 'bin/ihme_db.sh: OK'
exit 0
