#! /bin/bash -e
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
if [ "$0" != "bin/check_include.sh" ]
then
   echo "bin/check_include.sh: must be executed from its parent directory"
   exit 1
fi
# -----------------------------------------------------------------------------
cd include/dismod_at
list=$(ls *.hpp | sed -e '/^temp\./d' -e '/\/temp\./d')
for file in $list
do
   check=`echo $file | tr 'a-z' 'A-Z'`
   check=`echo $check  | sed -e 's|\.|_|' -e 's|^|DISMOD_AT_|'`
   if ! grep "^# ifndef $check" $file > /dev/null
   then
      echo "# ifndef $check"
      echo "missing in file include/dismod_at/$file"
      exit 1
   fi
   if ! grep "^# define $check" $file > /dev/null
   then
      echo "# define $check"
      echo "missing in file include/dismod_at/$file"
      exit 1
   fi
done
echo 'check_include.sh: OK'
exit 0
