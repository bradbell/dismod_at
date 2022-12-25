#! /bin/bash -e
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
if [ "$0" != "bin/run_xrst.sh" ]
then
   echo "bin/run_xrst.sh: must be executed from its parent directory"
   exit 1
fi
# -----------------------------------------------------------------------------
echo_eval xrst \
   --local_toc \
   --group_list default dev \
   --html_theme sphinx_rtd_theme
#
echo 'run_xrst.sh: OK'
exit 0
