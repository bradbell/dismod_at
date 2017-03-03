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
# BEGIN USER_SETTINGS
#
# which c++ compiler to use (empty means autotools will choose it)
autotools_cxx_compiler=''
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
version="Ipopt-3.12.6"
third_party="Mumps Metis"
web_page="http://www.coin-or.org/download/source/Ipopt"
# ---------------------------------------------------------------------------
# build_type
cmd=`grep '^build_type=' bin/run_cmake.sh`
eval $cmd
#
# ipopt_prefix
cmd=`grep '^ipopt_prefix=' bin/run_cmake.sh`
eval $cmd
# --------------------------------------------------------------------------
libdir=`bin/libdir.sh`
export PKG_CONFIG_PATH=$ipopt_prefix/$libdir/pkgconfig
# --------------------------------------------------------------------------
if echo "$ipopt_prefix" | grep '/dismod_at$' > /dev/null
then
	bin/build_type.sh install_ipopt $ipopt_prefix $build_type
fi
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
if [ "$build_type" == 'debug' ]
then
	debug_flag='--enable-debug'
else
	debug_flag=''
fi
if [ "$autotools_cxx_compiler" == '' ]
then
	comipler=''
	skip_warn=''
else
	compiler="CXX=$autotools_cxx_compiler"
	if [ "$autotools_cxx_compiler" == 'clang' ]
	then
		skip_warn='coin_skip_warn_cxxflags=yes'
	fi
fi
cat << EOF > config.sh
../configure \\
	$debug_flag \\
	$compiler \\
	$skip_warn \\
	--enable-static \\
	--prefix=$ipopt_prefix \\
	--libdir=$ipopt_prefix/$libdir \\
	--with-blas-lib="-lblas" \\
	--with-lapack-lib="-llapack"
EOF
echo_eval cat config.sh
echo_eval sh config.sh
echo_eval make install | tee make.log
# ----------------------------------------------------------------------------
echo 'install_ipopt.sh: OK'
