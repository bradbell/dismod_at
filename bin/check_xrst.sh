#! /bin/bash -e
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
if [ "$0" != "bin/check_xrst.sh" ]
then
   echo "bin/check_xrst.sh: must be executed from its parent directory"
   exit 1
fi
# -----------------------------------------------------------------------------
# index_page_name
index_page_name=$(\
   sed -n -e '/^ *--index_page_name*/p' .readthedocs.yaml | \
   sed -e 's|^ *--index_page_name *||' \
)
#
# number_jobs
if [ $(nproc) == '1' ] || [ $(nproc) == '2' ]
then
   number_jobs=1
else
   let number_jobs="$(nproc) - 1"
fi
#
# build
if [ ! -e build ]
then
   if [ -e build.debug ]
   then
      ln -s build.release build
   else
      if [ ! -e build.release ]
      then
         mkdir build.release
      fi
      ln -s build.release build
   fi
fi
#
# build/html
if [ -e build/html ]
then
   echo_eval rm -r build/html
fi
echo_eval xrst \
   --local_toc \
   --number_jobs $number_jobs \
   --group_list default dev \
   --html_theme sphinx_rtd_theme \
   --index_page_name $index_page_name
#
echo 'check_xrst.sh: OK'
exit 0
