# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2014-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
if [ ! -e build ]
then
	mkdir build
fi
cd build
cmake \
	-D extra_cxx_flags='-std=c++11 -Wall' \
	-D cppad_prefix=$HOME/prefix/cppad \
	..
