#! /usr/bin/env bash
set -e -u
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
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
if ! grep "^build_type *= *'release'" bin/install_settings.py > /dev/null
then
   echo 'bin/check_all.sh: build_type in bin/install_setings.py is not release'
   exit 1
fi
#
if [ $# == 1 ]
then
   if [ "$1" == --help ]
   then
cat << EOF
bin/check_all.sh flags
possible flags
--help                     print this help message
--debug                    compile for debugging
--verbose_make             generate verbose makefiles
--skip_check_copy          do not check copyright messages
--skip_external_links      do not check documentation external links
--suppress_spell_warnings  do not check for documentation spelling errors
EOF
      exit 0
   fi
fi
#
# debug, verbose_make, skip_external_links, suppress_spell_warnings
debug='no'
verbose_make='no'
skip_check_copy='no'
skip_external_links='no'
suppress_spell_warnings='no'
while [ $# != 0 ]
do
   case "$1" in

      --debug)
      debug='yes'
      ;;


      --verbose_make)
      verbose_make='yes'
      ;;

      --skip_check_copy)
      skip_check_copy='yes'
      ;;

      --skip_external_links)
      skip_external_links='yes'
      ;;

      --suppress_spell_warnings)
      suppress_spell_warnings='yes'
      ;;

      *)
      echo "bin/check_all.sh: command line argument "$1" is not valid"
      exit 1
      ;;

   esac
   shift
done
#
# build_type
if [ "$debug" == 'no' ]
then
   build_type="release"
else
   build_type='debug'
fi
#
# dismod_at_prefix
eval $(bin/install_settings.py | grep ^dismod_at_prefix)
#
if [ "$skip_check_copy" == 'no' ]
then
   bin/check_copy.sh
fi
#
# bin/check_*.sh
# exclude this file and bin/check_install.sh
list=`ls bin/check_*.sh bin/check_*.py | sed \
   -e '/check_all.sh/d' -e '/check_install.sh/d' -e '/check_copy.sh/d'`
for script in $list
do
   $script
done
#
# run_xrst.sh
flags=''
if [ "$skip_external_links" == 'no' ]
then
   flags+=' --external_links'
fi
if [ "$suppress_spell_warnings" == 'yes' ]
then
   flags+=' --suppress_spell_warnings'
fi
bin/run_xrst.sh $flags
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
# check.log output is generalted by bin/user_test.py which checks for warnings
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
