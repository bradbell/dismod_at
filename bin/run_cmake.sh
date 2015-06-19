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
verbose_makefile='0'                # use '0' for no and '1' for yes
extra_cxx_flags='-std=c++11 -Wall'  # extra c++ flags
#
# prefix where the corresponding required packages are installed
eigen_prefix="$HOME/prefix/dismod_at"
ipopt_prefix="$HOME/prefix/dismod_at"
cppad_prefix="$HOME/prefix/dismod_at"
# prefix where the corresponding optional packages are installed
# (use NOTFOUND if the package is not installed)
# suitesparse_prefix="$HOME/prefix/suitesparse"
suitesparse_prefix="NOTFOUND"
# END USER_SETTINGS
# ---------------------------------------------------------------------------
if [ ! -e build ]
then
	mkdir build
fi
cd build
cmake \
	-D CMAKE_VERBOSE_MAKEFILE=$verbose_makefile \
	-D extra_cxx_flags="$extra_cxx_flags" \
	-D cppad_prefix="$cppad_prefix" \
	-D ipopt_prefix="$cppad_prefix" \
	-D eigen_prefix="$eigen_prefix" \
	-D suitesparse_prefix="$suitesparse_prefix" \
	..
