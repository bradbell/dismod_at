#! /bin/bash -e
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
if [ "$0" != "bin/check_build_dir.sh" ]
then
   echo "bin/check_build_dir.sh: must be executed from its parent directory"
   exit 1
fi
# -----------------------------------------------------------------------------
if git grep 'build/example/user' test/user
then
   echo 'check_build_dir.sh: build/example/user occurs in test/user/*'
   exit 1
fi
if git grep 'build/test/user' example/user
then
   echo 'check_build_dir.sh: build/test/user occurs in example/user/*'
   exit 1
fi
echo 'check_build_dir.sh: OK'
exit 0
