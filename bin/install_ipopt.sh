#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# BEGIN USER_SETTINGS
ipopt_prefix="$HOME/prefix/dismod_at"
# END USER_SETTINGS
# ---------------------------------------------------------------------------
if [ $0 != 'bin/install_ipopt.sh' ]
then
	echo 'bin/install_ipopt.sh: must be executed from its parent directory'
	exit 1
fi
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# --------------------------------------------------------------------------
version="Ipopt-3.11.9"
third_party="Mumps Metis"
web_page="http://www.coin-or.org/download/source/Ipopt"
# --------------------------------------------------------------------------
if [ -e /usr/lib64 ]
then
	libdir='lib64'
else
	libdir='lib'
fi
export PKG_CONFIG_PATH=$ipopt_prefix/$libdir/pkgconfig
# --------------------------------------------------------------------------
if [ ! -e build/external ]
then
	mkdir -p build/external
fi
cd build/external
# --------------------------------------------------------------------------
if [ ! -e $version.tgz ]
then
	echo_eval wget "$web_page/$version.tgz"
fi
if [ -e $version ]
then
	echo_eval rm -rf $version
fi
echo_eval tar -xzf $version.tgz
# --------------------------------------------------------------------------
echo_eval cd $version
if [ -e ThirdParty/HSL ]
then
	echo_eval rm -rf ThirdParty/HSL
fi
#
for package in $third_party
do
	echo_eval cd ThirdParty/$package
	echo_eval ./get.$package
	echo_eval cd ../..
done
# ----------------------------------------------------------------------------
if [ ! -e build ]
then
	echo_eval mkdir build
fi
cd build
cat << EOF > config.sh
../configure \\
	--enable-debug \\
	--prefix=$ipopt_prefix \\
	--libdir=$ipopt_prefix/$libdir \\
	--with-blas-lib="-lblas" \\
	--with-lapack-lib="-llapack"
EOF
echo_eval cat config.sh
echo_eval sh config.sh
echo_eval make install | tee make.log
