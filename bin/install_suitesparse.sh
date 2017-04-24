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
# --------------------------------------------------------------------------
program='bin/install_suitesparse.sh'
if [ $0 != "$program" ]
then
	echo "$program: must be executed from its parent directory"
	exit 1
fi
# --------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
tarball='SuiteSparse-4.4.3.tar.gz'
web_page='http://faculty.cse.tamu.edu/davis/SuiteSparse'
libdir=`bin/libdir.sh`
# ---------------------------------------------------------------------------
# Get user configuration options from run_cmake.sh
#
# build_type
cmd=`grep '^build_type=' bin/run_cmake.sh`
eval $cmd
#
# ipopt_prefix
cmd=`grep '^ipopt_prefix=' bin/run_cmake.sh`
eval $cmd
#
# suitesparse_prefix
cmd=`grep '^suitesparse_prefix=' bin/run_cmake.sh`
eval $cmd
#
# metis_web_page
metis_web_page='http://glaros.dtc.umn.edu/gkhome/fetch/sw/metis/OLD'
#
# ipopt_version
cmd=`grep '^version=' bin/install_ipopt.sh | sed -e 's|version|ipopt_version|'`
eval $cmd
#
# metis_version
get_metis="build/external/$ipopt_version/ThirdParty/Metis/get.Metis"
if [ ! -e "$get_metis" ]
then
	echo "$get_metis does not exists"
	echo "must execute bin/install_ipopt.sh before bin/install_suitesparse.sh"
	exit 1
fi
metis_version=`grep "$metis_web_page" $get_metis | \
	sed -e 's|.*/||' -e 's|\.tar\.gz||'`
# --------------------------------------------------------------------------
if echo "$suitesparse_prefix" | grep '/dismod_at$' > /dev/null
then
	bin/build_type.sh install_suitesparse $suitesparse_prefix $build_type
fi
# --------------------------------------------------------------------------
if [ ! -e build/external ]
then
	mkdir -p build/external
fi
echo_eval cd build/external
# -----------------------------------------------------------------------------
if [ ! -e $tarball ]
then
	echo_eval wget $web_page/$tarball
fi
if [ -e 'SuiteSparse' ]
then
	echo_eval rm -r SuiteSparse
fi
echo_eval tar -xzf $tarball
cd SuiteSparse
# -----------------------------------------------------------------------------
metis_web_page='http://glaros.dtc.umn.edu/gkhome/fetch/sw/metis/OLD'
if [ ! -e "$metis_version.tar.gz" ]
then
	echo_eval wget "$metis_web_page/$metis_version.tar.gz"
fi
if [ -e "$metis_version" ]
then
	echo_eval rm -r "$metis_version"
fi
echo_eval tar -xzf "$metis_version.tar.gz"
if [ ! -e $metis_version ]
then
	echo 'install_suitesparse.sh: cannot get metis source'
	exit 1
fi
# suitesparse wants the metis source here
echo_eval mv $metis_version metis-4.0
# -----------------------------------------------------------------------------
sed -e \
"s|^\( *INSTALL_INCLUDE *\)=.*|\1= $suitesparse_prefix.$build_type/include|" \
-e "s|^\( *INSTALL_LIB *\)=.*|\1= $suitesparse_prefix.$build_type/$libdir|" \
-e 's|^\( *BLAS *\)=.*|\1= -lblas|' \
-e "s|^\( *METIS *\)=.*|\1= $ipopt_prefix/$libdir/libcoinmetis.a|" \
-e 's|^ *CF *=.*|& -DNTIMER|' \
-e '/^ *LIB *=/s| -lrt||' \
-i.bak SuiteSparse_config/SuiteSparse_config.mk
#
if [ "$build_type" == 'debug' ]
then
	sed \
		-e 's|^ *CFLAGS *= *$|CFLAGS = -g|' \
		-e '/^ *CF *=/s|-O3|-O0|' \
		-i.tmp SuiteSparse_config/SuiteSparse_config.mk
	rm SuiteSparse_config/SuiteSparse_config.mk.tmp
	sed \
		-e 's|^\( *Common->print *\)= *3 *;|\1= 4 ;|' \
		-i.bak CHOLMOD/Core/cholmod_common.c
fi
# -----------------------------------------------------------------------------
for dir in include lib
do
	if [ ! -e $suitesparse_prefix/$dir ]
	then
		echo_eval mkdir -p $suitesparse_prefix/$dir
	fi
done
# -----------------------------------------------------------------------------
echo_eval make
echo_eval make install
# -----------------------------------------------------------------------------
echo 'install_suitesparse.sh: OK'
