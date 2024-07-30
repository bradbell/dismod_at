#! /bin/bash -e
# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-24 Bradley M. Bell
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
   echo $*
   eval $*
}
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
#
# build_type
if ! grep "^build_type *= *'release'" bin/install_settings.py > /dev/null
then
   echo 'bin/check_all.sh: build_type in bin/install_setings.py is not release'
   exit 1
fi
build_type="$1"
#
# dismod_at_prefix
eval $(bin/install_settings.py | grep ^dismod_at_prefix)
#
# bin/check_*.sh
# exclude this file and bin/check_install.sh
list=`ls bin/check_*.sh | sed \
   -e '/check_all.sh/d' -e '/check_install.sh/d'`
for script in $list
do
   $script
done
#
# run_xrst.sh
if which xrst > /dev/null
then
   echo_eval bin/run_xrst.sh
fi
#
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
#
# dismod_at_prefix, PYTHONPATH
# Clean out previous install of dismod_at. Note that run_cmake.sh
# may have set this prefix to a link for the particular build type.
for file in dismod_at dismodat.py
do
   if [ -e "$dismod_at_prefix/bin/$file" ]
   then
      echo_eval rm "$dismod_at_prefix/bin/$file"
   fi
done
export PYTHONPATH=''
for dir in $(find -L $dismod_at_prefix -name 'site-packages' )
do
   if [ "$PYTHONPATH" == '' ]
   then
      PYTHONPATH="$dir"
   else
      PYTHONPATH="$PYTHONPATH:$dir"
   fi
   if ls "$dir/dismod_at"* >& /dev/null
   then
      echo_eval rm -r "$dir/dismod_at"*
   fi
done
# ----------------------------------------------------------------------------
#
echo_eval cd build
echo_eval make clean
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
