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
# upstream source for dismod_at tarballs
web_page='http://moby.ihme.washington.edu/bradbell/dismod_at'
#
# which local tarball to use (if not there get it from upstream)
tarball='dismod_at-20161016.tgz'
#
# location on this system of python3 and newer version of sqlite3
anaconda='/usr/local/anaconda3-current'
#
# either 'true' or 'false' (can use special requirements from previous version)
install_special_requirements='true'
#
build_type="$1"
if [ "$build_type" != 'debug' ] && [ "$build_type" != 'release' ]
then
	echo 'usage: ./install_dismod_at.sh build_type'
	echo 'where build_type is debug or release'
	exit 1
fi
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
# check for old log and error files
if [ ! -e log ]
then
	echo_eval mkdir log
fi
for ext in log err
do
	if [ -e log/install_dismod_at.$ext ]
	then
		read -p "log/install_dismod_at.$ext exists, remove or append [r/a] ?" \
			response
		if [ "$response" != 'r' ] && [ "$response" != 'a' ]
		then
			echo "response is not 'r' or 'a'"
			exit 1
		fi
		if [ "$response" != 'r' ]
		then
			echo_eval rm log/install_dismod_at.$ext
		fi
	fi
done
start_dir=`pwd`
log_file='log/install_dismod_at.log'
err_file='log/install_dismod_at.err'
# ----------------------------------------------------------------------------
# check for necessary programs
list="
	g++
	gfortran
	cmake
	pkg-config
	$anaconda/bin/sqlite3
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
# download the most recent version
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
if [ -e $version ]
then
	echo rm -rf $version
fi
echo_eval tar -xzf $tarball
echo_eval cd $version
# -----------------------------------------------------------------------------
# check for gsl library
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
# patch source code references to sqlite3
echo "changing <sqlite3.h> -> <$anaconda/include/sqlite3.h>"
cat << EOF > junk.sed
s|^# *include *<sqlite3\\.h> *\$|# include <$anaconda/include/sqlite3.h>|
EOF
for ext in hpp cpp
do
	list=`find . -name "*.$ext"`
	for file in $list
	do
		sed -f junk.sed -i $file
	done
done
#
cat << EOF > junk.sed
/^TARGET_LINK_LIBRARIES(/! b done
: loop
N
/\\n)\$/! b loop
s|\\n\\tsqlite3\\n|\\n\\t$anaconda/lib/libsqlite3.a\\n|
s|)\$|\\tpthread\\n\\trt\\n)|
: done
EOF
#
# patch references to sqlite3 library
list='
	devel/CMakeLists.txt
	example/devel/CMakeLists.txt
	test/devel/CMakeLists.txt
'
for file in $list
do
	echo_eval sed -f junk.sed -i $file
done
#
if [ "$build_type" == 'release' ]
then
	log_fatal_error='YES'
else
	log_fatal_error='NO'
fi
#
# patch the install scripts
cat << EOF > junk.sed
s|^build_type=.*|build_type=\'$build_type\'|
s|^log_fatal_error=.*|log_fatal_error=\'$log_fatal_error\'|
s|\\(^python3_executable\\)=.*|\\1='$anaconda/bin/python3'|
s|-std=c++11|-std=c++0x|
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
# using a static link from suitespace to ipopt version of metis library.
sed -e 's|^\.\./configure |& -enable-static |' -i bin/install_ipopt.sh
# -----------------------------------------------------------------------------
# Build and install all the special requirements
if [ "$install_special_requirements" == 'true' ]
then
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
			echo "install_$package.sh 1>> $log_file 2>> $err_file"
		$program 1>> "$start_dir/$log_file" 2>> "$start_dir/$err_file"
	done
elif [ "$install_special_requirements" != 'false' ]
then
	echo 'install_special_requirements is not true or false'
	exit 1
fi
# -----------------------------------------------------------------------------
# Build and install dismod_at
echo_eval bin/run_cmake.sh
echo_eval cd build
pkg_path="$HOME/prefix/dismod_at/$libdir/pkgconfig"
echo_eval export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$pkg_path"
echo_eval export PYTHONPATH=''
make check
make speed
make install
# -----------------------------------------------------------------------------
echo 'install_dismod_at.sh: OK'
# END BASH
