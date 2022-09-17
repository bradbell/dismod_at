# /usr/bin/bash -e
# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
if [ $0 != 'bin/time_memory.sh' ]
then
   echo 'bin/time_memory.sh: must be executed from its parent directory'
   exit 1
fi
# -----------------------------------------------------------------------------
if [ "$1" == '' ]
then
   echo 'usage: time_memory random_seed'
   echo 'use zero for choosing seed from clock'
   exit 1
fi
random_seed="$1"
# -----------------------------------------------------------------------------
python3 example/user/speed.py $random_seed | tee build/speed/time.out
cd build/speed
../devel/dismod_at example.db start
rm memory.out.*
valgrind --tool=massif ../devel/dismod_at example.db fit both
ms_print massif.out.* > memory.out
new.sh time.out memory.out
cd ../..
ls -R build/speed/new
