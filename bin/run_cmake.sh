#! /bin/bash -e
# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
# BEGIN USER_SETTINGS
# use '0' for normal and '1' for verbose make output
cmake_verbose_makefile='0'
#
# use either 'DEBUG' or 'RELEASE' for the type of this build
cmake_build_type='DEBUG'
#
# commannd used to execute python3 on this machine
python_three_command='python3'
#
# extra c++ flags used during compliation
extra_cxx_flags='-std=c++11 -Wall'
#
# prefix where dismod_at will be installed
dismod_at_prefix="$HOME/prefix/dismod_at"
#
# prefixes where the required packages were installed
eigen_prefix="$HOME/prefix/dismod_at"
ipopt_prefix="$HOME/prefix/dismod_at"
cppad_prefix="$HOME/prefix/dismod_at"
#
# Use a second order method when optimizing the fixed effects (YES/NO).
# The default is NO because the Newton method requires a lot more memory.
mixed_cppad_newton="NO"
#
# If YES, use sets of indices for sparsity pattersn. If NO use arrays of bools.
mixed_cppad_set_sparsity="NO"
#
# sub-directory of dismod_at_prefix where mixed_cppad libraries are installed
#	-lmixed_cppad -lmixed_cppad_eigen -lmixed_cppad
# The eigen part of the library is separate so different flags can be used
# to compile the part of the code that uses eigen.
# If you do not need to install mixed_cppad, use NOTFOUND for this setting.
mixed_cppad_libdir='lib64'
#
# Prefix where optional package was installed (use NOTFOUND if not installed).
# This is only required by example/devel/mixed_cppad/cholmod_xam.cpp.
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
	echo_eval mkdir build
fi
echo_eval cd build
cmake \
	-Wno-dev \
	-D CMAKE_VERBOSE_MAKEFILE=$cmake_verbose_makefile \
	-D CMAKE_BUILD_TYPE=$cmake_build_type \
	\
	-D python_three_command=$python_three_command \
	-D extra_cxx_flags="$extra_cxx_flags" \
	-D dismod_at_prefix="$dismod_at_prefix" \
	-D cppad_prefix="$cppad_prefix" \
	-D ipopt_prefix="$cppad_prefix" \
	-D eigen_prefix="$eigen_prefix" \
	\
	-D mixed_cppad_newton="$mixed_cppad_newton" \
	-D mixed_cppad_set_sparsity="$mixed_cppad_set_sparsity" \
	-D mixed_cppad_libdir="$mixed_cppad_libdir" \
	-D suitesparse_prefix="$suitesparse_prefix" \
	..
