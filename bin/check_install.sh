#! /bin/bash -e
# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-22 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# test install of python module and executable
# ---------------------------------------------------------------------------
if [ "$0" != "bin/check_install.sh" ]
then
	echo "bin/check_install.sh: must be executed from its parent directory"
	exit 1
fi
if [ "$1" != 'debug' ] && [ "$1" != 'release' ]
then
	echo 'bin/check_all.sh build_type'
	echo 'build_type is not debug or release'
	exit 1
fi
if [ "$1" == debug ]
then
	bin/run_cmake.sh --debug
else
	bin/run_cmake.sh
fi
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
# dismod_at_prefix
cmd=`grep ^dismod_at_prefix bin/run_cmake.sh`
eval $cmd
#
# python3_executable
cmd=`grep ^python3_executable bin/run_cmake.sh`
eval $cmd
#
# get python site-packages directory
python_dir=''
if find -L $dismod_at_prefix -name site-packages > /dev/null
then
	python_dir=`find -L $dismod_at_prefix -name site-packages`
fi
bin_dir="$dismod_at_prefix/bin"
#
# remove old install executable and python packages
echo_eval rm -rf $bin_dir/dismod_at
echo_eval rm -rf $bin_dir/dismodat.py
if [ "$python_dir" != '' ]
then
	echo_eval rm -rf $pyton_dir/dismod_at
fi
#
# install fresh copy
echo_eval cd build
echo_eval make install install_python
echo_eval cd ..
#
# PATH
export PATH="$bin_dir:$PATH"
#
# PYTHONPATH
python_dir=`find -L $HOME/prefix/dismod_at -name site-packages`
export PYTHONPATH="$python_dir"
echo "PYTHONPATH=$PYTHONPATH"
#
# LD_LIBRARY_PATH
export LD_LIBRARY_PATH=''
#
# create check_install and change into it
if [ -e 'build/check_install' ]
then
	echo_eval rm -r build/check_install
fi
echo_eval mkdir build/check_install
echo_eval cd build/check_install
echo_eval cp ../../example/get_started/get_started_db.py get_started_db.py
#
# create get_started.db
cat << EOF > create_db.py
import get_started_db
get_started_db.get_started_db()
EOF
echo_eval $python3_executable create_db.py
#
# test running dismod_at
echo_eval dismod_at get_started.db init
#
# test running dismodat.py
echo_eval dismodat.py get_started.db db2csv
#
if ! podman images | grep '^localhost/dismod_at.image' > /dev/null
then
	echo 'podman cannot find dismod_at.image, skipping its test'
	echo 'check_install.sh: OK'
	exit 0
fi
# -----------------------------------------------------------------------------
# Test that OCI image gives the same result
echo_eval mkdir podman
echo_eval cd podman
echo_eval cp ../../../bin/dock_dismod_at.sh dock_dismod_at.sh
echo_eval cp ../get_started_db.py ../create_db.py .
echo_eval $python3_executable create_db.py
echo_eval ./dock_dismod_at.sh debug    get_started.db init
echo_eval ./dock_dismod_at.sh release  get_started.db db2csv
list='age_avg.csv  data.csv  option.csv  variable.csv'
for file in $list
do
	# check that both version gave the same result
	echo_eval diff $file ../$file
done
# -----------------------------------------------------------------------------
echo 'check_install.sh: OK'
exit 0
