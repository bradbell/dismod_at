#! /bin/bash -e
# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-16 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# =============================================================================
# $OMhelpKeyCharacter=&
# &begin run_cmake.sh&& &newlinech #&&
# &spell
#	cmake makefile cxx std dismod libdir lcppad cholmod xam cpp
#	usr eigen ipopt cppad bools suitesparse devel hpp
# &&
#
# &section bin/run_cmake.sh: User Configuration Options&&
#
# &head cmake_verbose_makefile&&
# Use 'NO' for normal and 'YES' for verbose make output:
# &codep
cmake_verbose_makefile='NO'
# &&
#
# &head cmake_build_type&&
# Use either 'DEBUG' or 'RELEASE' for the type of this build:
# &codep
cmake_build_type='DEBUG'
# &&
#
# &head python_three_command&&
# Command used to execute python3 on this machine:
# &codep
python_three_command='python3'
# &&
#
# &head extra_cxx_flags&&
# Extra C++ flags used during compilation:
# &codep
extra_cxx_flags='-std=c++11 -Wall'
# &&
#
# &head dismod_at_prefix&&
# Prefix where dismod_at will be installed:
# &codep
dismod_at_prefix="$HOME/prefix/dismod_at"
# &&
#
# &head Other Prefixes&&
# Prefixes where the required packages were installed:
# &codep
eigen_prefix="$HOME/prefix/dismod_at/eigen"
ipopt_prefix="$HOME/prefix/dismod_at"
cppad_prefix="$HOME/prefix/dismod_at"
suitesparse_prefix="$HOME/prefix/dismod_at"
# &&
#
# &head IHME Cluster Settings&&
# Here are some example changes that are used for the IHME cluster
# &codep
# python_three_command='/usr/local/anaconda3-current/bin/python'
# extra_cxx_flags='-Wall'
# &&
# &end
# ============================================================================
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
if [ "$1" == '--help' ]
then
	cat << EOF
usage: bin/run_cmake.sh \\
	[--help] \\
	[--verbose] \\
EOF
	exit 0
else
	user_option="$1"
fi
if [ "$user_option" == '--verbose' ]
then
	cmake_verbose_makefile='YES'
elif [ "$user_option" != '' ]
then
	echo "'$1' is an invalid option"
	bin/run_cmake.sh --help
	exit 1
fi
# ---------------------------------------------------------------------------
if [ ! -e build ]
then
	echo_eval mkdir build
fi
echo_eval cd build
if [ -e 'CMakeCache.txt' ]
then
	rm CMakeCache.txt
fi
cmake \
	-Wno-dev \
	-D CMAKE_VERBOSE_MAKEFILE=$cmake_verbose_makefile \
	-D CMAKE_BUILD_TYPE=$cmake_build_type \
	\
	-D python_three_command=$python_three_command \
	-D extra_cxx_flags="$extra_cxx_flags" \
	\
	-D dismod_at_prefix="$dismod_at_prefix" \
	-D ipopt_prefix="$ipopt_prefix" \
	-D eigen_prefix="$eigen_prefix" \
	-D cppad_prefix="$cppad_prefix" \
	-D suitesparse_prefix="$suitesparse_prefix" \
	..
