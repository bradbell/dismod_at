#! /bin/bash -e
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
#
# {xrst_begin example_install.sh}
# {xrst_comment_ch #}
#
# An Example Installation
# #######################
#
# Syntax
# ******
# ``bin/example_install.sh``
#
# {xrst_literal
#     BEGIN BASH
#     END BASH
# }
#
# {xrst_end example_install.sh}
# ----------------------------------------------------------------------------
# BEGIN BASH
#
# build_type
eval $(bin/install_settings.py | grep '^build_type')
#
# dismod_at_prefix
eval $(bin/install_settings.py | grep '^dismod_at_prefix')
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
   echo $*
   eval $*
}
# --------------------------------------------------------------------------
# remove old version of
# example_install.log, example_install.err, cpapd_mixed.log, cppad_mixed.err
for script in example_install get_cppad_mixed
do
   for ext in log err
   do
      if [ -e "$script.$ext" ]
      then
         echo_eval rm "$script.$ext"
      fi
   done
done
# --------------------------------------------------------------------------
# set build link to build.debug or build.release depending on build_type
if echo "$dismod_at_prefix" | grep '/dismod_at$' > /dev/null
then
   bin/build_type.sh example_install.sh $dismod_at_prefix $build_type
fi
# -----------------------------------------------------------------------------
# install the special requirements
echo_eval bin/get_cppad_mixed.sh
# ----------------------------------------------------------------------------
# dismod_at
# -----------------------------------------------------------------------------
# Check we can find ipopt.pc, echo PKG_CONFIG_PATH to help user set this value
dir=`find -L $dismod_at_prefix -name 'ipopt.pc' | sed -e 's|/ipopt.pc||'`
if [ "$dir" == '' ]
then
   echo "Cannot find ipopt.pc in $dismod_at_prefix directory"
   exit 1
else
   echo 'pkg-config setting for ipopt'
   echo_eval export PKG_CONFIG_PATH="$dir"
fi
#
# bin/run_cmake.sh
echo "bin/run_cmake.sh 1>> example_install.log 2>> example_install.err"
bin/run_cmake.sh 1>> example_install.log 2>> example_install.err
#
# change into build directory
echo_eval cd build
#
if which nproc >& /dev/null
then
   n_job=$(nproc)
else
   n_job=$(sysctl -n hw.ncpu)
fi
#
# build dismod_at using n_jobs
echo "make -j $n_job 1>> ../example_install.log 2>> ../example_install.err"
if ! make -j $n_job 1>> ../example_install.log 2>> ../example_install.err
then
   echo "Try running the follow command in $(pwd)"
   echo "    make -j $n_job"
   echo 'to see wy the build of dismod_at failed.'
   exit 1
fi
#
# make check, speed, install, install_python
for cmd in check speed install install_python
do
   echo "make $cmd 1>> example_install.log 2>> example_install.err"
   if ! make $cmd 1>> ../example_install.log 2>> ../example_install.err
   then
      echo "Try running the follow command in $(pwd)"
      echo "    make $cmd"
      echo 'to see wy the check of dismod_at failed.'
   fi
done
cd ..
# -----------------------------------------------------------------------------
echo 'bin/example_install.sh: OK'
exit 0
# END BASH
