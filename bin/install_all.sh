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
# ---------------------------------------------------------------------------
# BEGIN BASH
# ---------------------------------------------------------------------------
# build_type
cmd=`grep '^build_type=' bin/run_cmake.sh`
eval $cmd
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
# check for old log and error files
if [ -e install_all.log ]
then
	echo_eval rm install_all.log
fi
if [ -e install_all.err ]
then
	echo_eval rm install_all.err
fi
# -----------------------------------------------------------------------------
# modify install scripts to have proper flags
if [ "$build_type" == 'release' ]
then
	log_fatal_error='YES'
else
	log_fatal_error='NO'
fi
#
cat << EOF > junk.sed
s|^log_fatal_error=.*|log_fatal_error=\'$log_fatal_error\'|
EOF
list="
	run_cmake.sh
	install_eigen.sh
	install_ipopt.sh
	install_suitesparse.sh
	install_cppad.sh
	install_cppad_mixed.sh
"
for file in $list
do
	echo_eval sed -f junk.sed -i bin/$file
done
# -----------------------------------------------------------------------------
list="
	eigen
	ipopt
	suitesparse
	cppad
	cppad_mixed
"
for package in $list
do
	program="bin/install_$package.sh"
	echo "install_$package.sh 1>> install_all.log 2>> install_all.err"
	$program 1>> install_all.log 2>> install_all.err
done
# -----------------------------------------------------------------------------
# build and install dismod_at
echo_eval bin/run_cmake.sh
echo_eval cd build
pkg_path="$HOME/prefix/dismod_at/$libdir/pkgconfig"
echo_eval export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$pkg_path"
echo_eval export PYTHONPATH=''
make check
make speed
make install
cd ..
# -----------------------------------------------------------------------------
# modify install scripts to have proper flags
cat << EOF > junk.sed
s|^log_fatal_error=.*|log_fatal_error=\'NO'|
EOF
list="
	run_cmake.sh
	install_eigen.sh
	install_ipopt.sh
	install_suitesparse.sh
	install_cppad.sh
	install_cppad_mixed.sh
"
for file in $list
do
	echo_eval sed -f junk.sed -i bin/$file
done
# -----------------------------------------------------------------------------
echo 'install_all.sh: OK'
# END BASH
