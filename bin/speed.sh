#! /bin/bash -e
# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# parameters that can be changed
#
# non-zero so same random see each time.
random_seed='123'
#
# number of random effects is 2 * n_children
n_children='20'
#
# use quasi-Newton (otherwise Newton)
quasi_fixed='false'
#
# smaller increase work per function evaluation
ode_step_size='1.0'
#
# n_data
n_data=200
#
# max_fit
max_fit=50
#
# If non-empty, compare branches.
# If empty, and there is a new sub-directory, use new subdirectory for changes.
# If empty, and no new sub-directory, only run branch1 case.
branch2=''
#
# Assume that at beginning, installs correspond to the original branch.
# If true, re-install release version corresponding to each version and
# at the end ensure installs correspond to original branch.
get_cppad_mixed='false'
# -----------------------------------------------------------------------------
if [ "$0" != 'bin/speed.sh' ]
then
   echo 'bin/speed.sh must be run from its parent directory'
   exit 1
fi
if [ "$1" == '' ]
then
   echo 'usage: bin/speed.sh branch1'
   echo 'example: bin/speed.sh master'
   echo 'see settings at top of bin/speed.sh'
   exit 1
fi
branch1="$1"
# -----------------------------------------------------------------------------
original_branch=`git branch | sed -n -e '/^\*/p' | sed -e 's|^\* *||'`
if [ "$branch2" == "$original_branch" ]
then
   echo "bin/speed.sh: branch2 cannot be original_branch = $original_branch"
   exit 1
fi
if [ "$branch1" == "$branch2" ]
then
   echo 'bin/speed.sh: branch1 and branch2 must be different'
   echo 'branch2 can be empty in which case new is used to replace source'
   exit 1
fi
if ! grep "^build_type *= *'release'" bin/install_settings.py > /dev/null
then
   echo 'bin/speed.sh: install_settings.py build_type is not release'
   exit
fi
# -----------------------------------------------------------------------------
if [ "$branch2" != '' ] || [ -d new ]
then
   list='one two'
else
   list='one'
fi
for version in $list
do
   # ------------------------------------------------------------------------
   # checkout branch and determine test name
   # ------------------------------------------------------------------------
   if [ "$version" == 'one' ]
   then
      git reset --hard
      git checkout $branch1
      name="$branch1"
   else
      if [ "$branch2" == '' ]
      then
         echo 'git_new.sh from > /dev/null'
         git_new.sh from > /dev/null
         name='new'
      else
         git checkout $branch2
         name="$branch2"
      fi
   fi
   # ----------------------------------------------------------------------
   # run camke for release version
   # ----------------------------------------------------------------------
   # run cmake
   echo "bin/run_cmake.sh > build.release/$name.log"
   bin/run_cmake.sh > build/$name.log
   #
   if [ "$name" != 'master' ]
   then
      # ------------------------------------------------------------------
      # run installs for this version
      # ------------------------------------------------------------------
      if [ "$get_cppad_mixed" == 'true' ]
      then
         echo "bin/get_cppad_mixed.sh >> build.release/$name.log"
         bin/get_cppad_mixed.sh >> build/$name.log
      fi
   fi
   # ------------------------------------------------------------------------
   # build dismod_at
   # ------------------------------------------------------------------------
   # build dismod_at
   cd build
   make clean
   echo "make dismod_at >> build.release/$name.log"
   make dismod_at >> $name.log
   cd ..
   # ------------------------------------------------------------------------
   # run speed test using this version
   # ------------------------------------------------------------------------
   #
   # create database
   arguments="$random_seed $n_children $quasi_fixed $ode_step_size"
   arguments+=" $n_data $max_fit"
   echo "python3 example/user/speed.py $arguments >> build.release/$name.log"
   python3 example/user/speed.py $arguments >> build/$name.log
   #
   # change into database directory
   pushd build/example/user > /dev/null
   #
   # run timing test
   program="../../devel/dismod_at"
   echo "time $program example.db fit both 0 >& build.release/$name.out"
   ( time $program example.db fit both 0 ) >& ../../$name.out
   #
   # run memory test
   echo "massif.sh $program example.db fit both 0 >& /dev/null"
   massif.sh $program example.db fit both 0 >& /dev/null
   #
   echo "mv massif.out build.release/$name.massif"
   mv massif.out ../../$name.massif
   #
   popd > /dev/null
done
#
# original_branch
git checkout $original_branch
if [ "$name" != "$original_branch" ]
then
   if [ "$get_cppad_mixed" == 'true' ]
   then
      echo "bin/get_cppad_mixed.sh >> build.release/$name.log"
      bin/get_cppad_mixed.sh >> build/$name.log
   fi
fi
# ---------------------------------------------------------------------------
echo 'bin/speed.py: OK'
