# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# BEGIN USER_SETTINGS
# use '1' for verbose make output
verbose_makefile='0'
# commannd used to execute python3 on this machine
python_three_command='python3'
# extra c++ flags used during compliation
extra_cxx_flags='-std=c++11 -Wall'
# prefix where dismod_at will be installed
dismod_at_prefix="$HOME/prefix/dismod_at"
# prefixes where the required packages were installed
eigen_prefix="$HOME/prefix/dismod_at"
ipopt_prefix="$HOME/prefix/dismod_at"
cppad_prefix="$HOME/prefix/dismod_at"
# prefix where suitesparse was installed (use NOTFOUND if not installed)
suitesparse_prefix="$HOME/prefix/suitesparse"
# ----------------------------------------------------------------------------
# Setting for IHME cluster
# suitesparse_prefix="NOTFOUND"
# python_three_command='/usr/local/anaconda3-current/bin/python'
# extra_cxx_flags='-Wall'
# END USER_SETTINGS
# ---------------------------------------------------------------------------
if [ ! -e build ]
then
	mkdir build
fi
cd build
cmake \
	-Wno-dev \
	-D CMAKE_VERBOSE_MAKEFILE=$verbose_makefile \
	-D python_three_command=$python_three_command \
	-D extra_cxx_flags="$extra_cxx_flags" \
	-D dismod_at_prefix="$dismod_at_prefix" \
	-D cppad_prefix="$cppad_prefix" \
	-D ipopt_prefix="$cppad_prefix" \
	-D eigen_prefix="$eigen_prefix" \
	-D suitesparse_prefix="$suitesparse_prefix" \
	..
