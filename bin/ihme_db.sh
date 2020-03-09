#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -----------------------------------------------------------------------------
# $begin ihme_db.sh$$ $newlinech #$$
# $spell
#	dismod
#	ihme
#	dir
#	epi
#	csv
#	cmake
# $$
#
# $section Make a Local Copy an IHME Dismod_at Database$$
#
# $head Syntax$$
# $codei%bin/ihme_db.sh [--%user%] %relative_dir% %database%
# %$$
#
# $head Purpose$$
# This script copies a dismod_at database to a local directory
# and then runs the $cref db2csv_command$$ on the database.
# This script is only intended to be used by members of IHME
# and to be run from the IHME cluster.
# It can also be run on a computer that has mounted the
# $code /ihme/epi$$ and $code /ihme/homes$$ directories.
#
# $head Download$$
# You must first change into a directory where you have write permission,
# download $cref/dismod_at.git/install_unix/Download dismod_at.git/$$,
# and then change into the dismod_at.git directory.
# This script can only be run from the download directory dismod_at.git.
#
# $head python3$$
# You must inform dismod_at where the python3 executable is located.
# The setting for this is in the
# $cref/run_cmake.sh/run_cmake.sh/python3_executable/$$ file.
# If that setting is not correct, change it, and then execute the command
# $codei%
#	bin/run_cmake.sh
# %$$
#
# $head absolute_dir$$
# We use $icode absolute_dir$$ to refer to an absolute path to a
# directory below which the database is stored before executing this script.
# You must have read permission below this directory
# (but do not need write permission).
# The default value for this directory is
# $codei%
#      %absolute_dir%=/ihme/epi/at_cascade
# %$$
#
# $head --user$$
# The brackets around $codei%--%user%$$ indicate that this
# command line argument is optional.
# If it is present, $icode absolute_dir$$ is the home directory for
# the specified user; i.e.,
# $codei%
#      %absolute_dir%=/ihme/homes/%user%
# %$$
#
# $head relative_dir$$
# The command line argument $icode relative_dir$$ is the directory,
# relative to the absolute directory,
# where the database is stored before executing this script.
#
# $head database$$
# The command line argument $icode database$$
# is the local name of the database file; i.e.,
# there are no $code /$$ characters in $icode database$$.
# The original database is stored in the location
# $codei%
#      %absolute_dir%/%relative_dir%/%database%
# %$$
#
# $head db2csv$$
# The original database is copied to the directory
# $codei%
#      ihme_db/%relative_dir%
# %$$
# relative to the download directory dismod_at.git.
# We refer to this as the local directory.
# The db2csv_command is run on the database.
# The resulting CSV files are all in the local directory.
#
# $end
# ---------------------------------------------------------------------------
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
if [ ! -d '/ihme' ]
then
	echo 'bin/ihme_db.sh: The directory /ihme does not exist.'
	exit 1
fi
absolute_path='/ihme/epi/at_cascade'
if [[ "$1" == --* ]]
then
	user=`echo "$1" | sed -e 's|^--||'`
	absolute_path="/ihme/homes/$user"
	shift
fi
if [ "$2" == '' ]
then
	echo 'usage: bin/ihme_db.sh [--user] relative_dir database'
	echo 'see: https://bradbell.github.io/dismod_at/doc/ihme_db.sh.htm'
	exit 1
fi
relative_dir="$1"
database="$2"
full_path="$absolute_path/$relative_dir/$database"
if [ ! -e "$full_path" ]
then
	echo 'bin/ihme_db.sh: Cannot find the following database:'
	echo "$full_path"
	exit 1
fi
# -----------------------------------------------------------------------------
local_dir="ihme_db/$relative_dir"
if [ ! -e "$local_dir" ]
then
	echo_eval mkdir -p "$local_dir"
fi
echo_eval cp $full_path $local_dir/$database
echo_eval bin/dismodat.py $local_dir/$database db2csv
# ---------------------------------------------------------------------------
echo 'bin/ihme_db.sh: OK'
exit 0
