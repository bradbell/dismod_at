#! /bin/bash -e
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
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
      && [ "$name" != 'covid_19' ]
      then
         echo "$name is not in example/user/CMakeLists.txt"
         exit 1
      fi
   fi
   #
   # user.omh
   if ! grep "\$rref user_$name" example/user/user.omh > /dev/null
   then
      echo "\$rref user_$name.py\$ is not in example/user/user.omh"
      exit 1
   fi
done
echo 'check_user_xam.sh: OK'
exit 0
