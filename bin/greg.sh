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
# ---------------------------------------------------------------------------
remote_dir='/snfs2/HOME/gma1/tmp'
if [ "$0" != 'bin/greg.sh' ] || [ "$#" != '1' ]
then
	echo 'usage: bin/greg.sh database'
	exit 1
fi
database="$1"
# ---------------------------------------------------------------------------
if [ ! -e "$remote_dir" ]
then
	echo "remote_dir=$remote_dir: does not exist"
	exit 1
fi
# ---------------------------------------------------------------------------
if [ ! -e "$remote_dir/$database" ]
then
	echo "database=$database: does not exist"
	exit 1
fi
# ---------------------------------------------------------------------------
if [ ! -e build/devel/dismod_at ]
then
	echo 'build/devel/dismod_at does not exist'
	exit 1
fi
# ---------------------------------------------------------------------------
if [ -e build/greg ]
then
	echo_eval rm -r build/greg
fi
echo_eval mkdir build/greg
# ---------------------------------------------------------------------------
echo_eval cp $remote_dir/$database build/greg/$database
echo_eval bin/dismodat.py build/greg/$database db2csv
# ---------------------------------------------------------------------------
echo_eval ls build/greg
echo 'bin/greg.sh: OK'
exit 0
