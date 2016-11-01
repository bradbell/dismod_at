#! /bin/bash -e
# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-16 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
if [ "$0" != "bin/check_install.sh" ]
then
	echo "bin/check_install.sh: must be executed from its parent directory"
	exit 1
fi
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
# test install of python module and executable
prefix="$HOME/prefix/dismod_at"
#
echo_eval rm -rf $prefix/bin/dismod_at
echo_eval rm -rf $prefix/bin/dismodat.py
echo_eval rm -rf $prefix/lib/python*
#
echo_eval cd build
echo_eval make install
echo_eval cd ..
#
list='
	option.csv
	log.csv
	data.csv
	variable.csv
	predict.csv
'
for name in $list
do
	file="build/example/get_started/$name"
	if [ -e "$file" ]
	then
		echo_eval rm $file
	fi
done
#
python_dir=`find -L $HOME/prefix/dismod_at -name site-packages`
export PYTHONPATH="$python_dir"
echo "PYTHONPATH=$PYTHONPATH"
#
python_executable=`head -1 $prefix/bin/dismodat.py | sed -e 's|^#!||'`
echo_eval $python_executable example/get_started/predict_command.py
#
echo_eval $prefix/bin/dismodat.py build/example/get_started/example.db db2csv
#
for name in $list
do
	file="build/example/get_started/$name"
	if [ ! -e "$file" ]
	then
		echo "check_install.sh: Error: $file not found"
	fi
done
# -----------------------------------------------------------------------------
echo 'bin/check_install.sh: OK'
