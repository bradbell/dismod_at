#! /bin/bash -e
# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# Test this version, not previous install
export PYTHONPATH=''
# ---------------------------------------------------------------------------
if [ "$0" != "bin/check_all.sh" ]
then
   echo 'bin/check_all.sh build_type'
   echo 'must be executed from its parent directory'
   exit 1
fi
if [ "$1" != 'debug' ] && [ "$1" != 'release' ]
then
   echo 'bin/check_all.sh build_type'
   echo 'build_type is not debug or release'
   exit 1
fi
build_type="$1"
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
   echo $*
   eval $*
}
# -----------------------------------------------------------------------------
# check build_type in run_cmake.sh
if ! grep "^build_type='release'" bin/run_cmake.sh > /dev/null
then
   echo 'bin/check_all.sh: build_type in bin/run_cmake.sh is not release'
   exit 1
fi
#
# run bin/check_*.sh with exception of this file and bin/check_install.sh
list=`ls bin/check_*.sh | sed \
   -e '/check_all.sh/d' -e '/check_install.sh/d'`
for script in $list
do
   $script
done
#
# check version number
echo_eval version.sh check
#
# build developer documentation
echo_eval xrst \
   --group_list default dev \
   --local_toc \
   --html_theme sphinx_rtd_theme \
# ----------------------------------------------------------------------------
# run cmake
if [ "$build_type" == 'debug' ]
then
   flag='--debug'
else
   flag=''
fi
#
echo "bin/run_cmake.sh $flag >& cmake.log"
bin/run_cmake.sh $flag >& cmake.log
# ----------------------------------------------------------------------------
#
echo_eval cd build
#
echo 'make check >& check.log'
make check >& ../check.log
#
echo 'make speed >& speed.log'
make speed >& ../speed.log
#
echo_eval cd ..
#
echo "bin/check_install.sh $build_type >& install.log"
bin/check_install.sh $build_type >& install.log
# -----------------------------------------------------------------------------
# check.log output is generalted by bin/user_test.sh which checks for warnings
# and re-runs. If a warning occurs a second time, bin/user_tes.sh error exits.
for target in cmake speed install
do
   if grep -i 'warning:' $target.log
   then
      echo "bin/run_check_all.sh: $target.log has warnings."
      exit 1
   fi
done
# -----------------------------------------------------------------------------
echo 'check_all.sh: OK'
exit 0
