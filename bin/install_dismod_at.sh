#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-16 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# BEGIN BASH
web_page='http://moby.ihme.washington.edu/bradbell/dismod_at'
tarball='dismod_at-20161013.tgz'
anaconda='/usr/local/anaconda3-current'
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
# check that we are on the IHME cluster
if [ -e "$HOME/junk.$$" ] || [ -e "/snfs2/HOME/$USERE/junk.$$" ]
then
	echo 'Cannot test if this is the IHME cluster'
	exit 1
fi
test_dir="/snfs2/HOME/$USER"
if [ -e "$test_dir" ]
then
	touch $test_dir/junk.$$
fi
if [ ! -e "$HOME/junk.$$" ]
then
	echo 'install_dismod_at.sh only works on the IHME cluster'
	echo 'See the general install instructions at'
	echo 'http://moby.ihme.washington.edu/bradbell/dismod_at/install_unix.htm'
	if [ -e "$test_dir/junk.$$" ]
	then
		rm "$test_dir/junk.$$"
	fi
	exit 1
fi
if [ -e "$test_dir/junk.$$" ]
then
	rm "$test_dir/junk.$$"
fi
# ----------------------------------------------------------------------------
start_dir=`pwd`
log_file='install_dismod_at.log'
err_file='install_dismod_at.err'
for ext in log err
do
	if [ -e install_dismod_at.$exgt ]
	then
		echo "Must first remove old $log_file and $err_file"
		exit 1
	fi
done
# ----------------------------------------------------------------------------
list="
	g++
	gfortran
	cmake
	pkg-config
	sqlite3
	$anaconda/bin/python3
"
for program in $list
do
	if ! which $program > /dev/null
	then
		echo "Cannot find $program"
		exit 1
	fi
done
# ----------------------------------------------------------------------------
version=`echo $tarball | sed -e 's|.*/||' -e 's|\.tgz||'`
#
if ! cd $HOME/install
then
	echo "install_cppad.sh: cannot change into $HOME/install directory"
	exit 1
fi
if [ ! -e $tarball ]
then
	echo "wget --no-check-certificate $web_page/$tarball"
	if !  wget --no-check-certificate $web_page/$tarball
	then
		echo 'install_dismod_at: cannot download tarball'
		exit 1
	fi
fi
if [ ! -e $version ]
then
	echo_eval tar -xzf $tarball
fi
echo_eval cd $version
# -----------------------------------------------------------------------------
cat << EOF > junk.sed
s|^# *include *<sqlite3\\.h> *\$|# include <$anaconda/include/sqlite3.h>|
EOF
list=`find . -name '*.hpp'`
for file in $list
do
	echo_eval sed -f junk.sed -i $file
done
# -----------------------------------------------------------------------------
libdir=`bin/libdir.sh`
gsl_dir=''
for prefix in /usr /usr/local
do
	if [ -e "$prefix/$libdir/libgsl.so" ]
	then
		gsl_dir="$prefix/$libdir"
	fi
done
if [ "$gsl_dir" == '' ]
then
	echo 'Cannot find libgsl.so'
	exit 1
fi
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
	echo "sed -e 's|-std=c++11|-std=c++0x|' -i bin/install_$package.sh"
	sed -e 's|-std=c++11|-std=c++0x|' -i bin/install_$package.sh
	#
	program="bin/install_$package.sh"
	echo "$program 1>> $log_file 2>> $err_file"
	$program 1>> "$start_dir/$log_file" 2>> "$start_dir/$err_file"
done
# -----------------------------------------------------------------------------
cat << EOF > junk.sed
s|\\(^python3_executable\\)=.*|\\1='$anaconda/bin/python3'|
s|-std=c++11|-std=c++0x|
EOF
echo_eval sed -f junk.sed -i bin/run_cmake.sh
echo_eval bin/run_cmake.sh
echo_eval cd build
pkg_path="$HOME/prefix/dismod_at/$libdir/pkgconfig"
echo_eval export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$pkg_path"
make check
make speed
make install
# -----------------------------------------------------------------------------
echo 'install_dismod_at.sh: OK'
# END BASH
