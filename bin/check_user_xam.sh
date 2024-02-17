#! /bin/bash -e
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
if [ "$0" != "bin/check_user_xam.sh" ]
then
   echo "bin/check_user_xam.sh: must be executed from its parent directory"
   exit 1
fi
# -----------------------------------------------------------------------------
list=$(ls example/user/*.py )
for file in $list
do
   name=$(echo $file | sed -e 's|^.*/||' -e 's|\.py$||')
   #
   # CMakeLists.txt
   if ! grep "^   $name\$" example/user/CMakeLists.txt > /dev/null
   then
      if [ "$name" != 'speed' ] \
      && [ "$name" != 'diabetes' ] \
      && [ "$name" != 'new_diabetes' ] \
      && [ "$name" != 'covid_19' ]
      then
         echo "$name is not in example/user/CMakeLists.txt"
         exit 1
      fi
   fi
   #
   # user.xrst
   if [ "$name" != 'new_diabetes' ]
   then
   if ! grep ":ref:.user_$name.py-title" example/user/user.xrst > /dev/null
   then
      echo ":ref:\`user_$name.py-title\` is not in example/user/user.xrst"
      exit 1
   fi
   fi
done
echo 'check_user_xam.sh: OK'
exit 0
