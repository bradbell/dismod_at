#! /bin/bash -e
# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# =============================================================================
# $OMhelpKeyCharacter=&
# &begin run_cmake.sh&& &newlinech #&&
# &spell
#	libdir
#	cmake
#	makefile
#	cxx
#	std
#	dismod
#	lcppad
#	cholmod
#	xam
#	cpp
#	usr
#	eigen
#	ipopt
#	cppad
#	bools
#	devel
#	hpp
#	pthread
#	checkpointing
#	Wshadow
#	Wconversion
#	Wpedantic
# &&
#
# &section bin/run_cmake.sh: User Configuration Options&&
#
# &head verbose_makefile&&
# Use 'no' for normal and 'yes' for verbose make output:
# &codep
verbose_makefile='no'
# &&
#
# &head build_type&&
# Use either &code debug&& or &code release&& for the type of this build:
# &codep
build_type='release'
# &&
#
# &head dismod_at_prefix&&
# Prefix where dismod_at will be installed:
# &codep
dismod_at_prefix="$HOME/prefix/dismod_at"
# &&
# If &icode dismod_at_prefix&& ends in &code dismod_at&&,
# &code run_cmake.sh&& will use a soft link from this prefix to
# &icode%dismod_at_prefix%.debug%&& or
# &icode%dismod_at_prefix%.release%&&
# depending on the choice for &icode build_type&&.
#
# &head Debug and Release&&
# If a soft link is used for the install,
# the same technique will be used to map the &code build&&
# directory to the debug or release version.
# If you are using both a debug and release versions of dismod_at,
# both versions of the
# &cref/special requirements/install_unix/Special Requirements/&&
# will need to be installed.
#
# &head python3_executable&&
# Path to the python3 executable on this machine:
# &codep
python3_executable='/usr/bin/python3'
# &&
# In the past, removing the &code #&& characters in front of the following
# command would work for the IHME cluster:
# &codei%
# # python3_executable='/usr/local/anaconda3-current/bin/python'
# %&&
#
# &head Choosing C++ Compiler&&
# Which c++ compiler should cmake use (empty means cmake will choose it).
# &codep
cmake_cxx_compiler=''
# &&
#
# &head extra_cxx_flags&&
# Extra C++ flags used during compilation:
# &codep
extra_cxx_flags='-std=c++11 -Wpedantic -Wall -Wshadow -Wconversion'
# &&
# An alternative might be &code -Wall&&.
#
# &head cmake_libdir&&
# Sub-directory of each prefix where libraries are installed.
# &codep
cmake_libdir='lib64'
# &&
#
# &head system_specific_library_list&&
# List of libraries that are needed for a particular system. For example,
# if when you build &code dismod_at&& the &code pthread&& library is
# required by your system, then include it here.
# Libraries in the list can be separated by spaces or semi-colons.
# &codep
system_specific_library_list=''
# &&
# &end
# ============================================================================
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
if [ "$0" != 'bin/run_cmake.sh' ]
then
	echo 'bin/run_cmake.sh: must be executed from its parent directory'
	exit 1
fi
while [ "$1" != '' ]
do
	if [ "$1" == '--help' ]
	then
		cat << EOF
usage: bin/run_cmake.sh \\
	[--help] \\
	[--debug] \\
	[--verbose] \\
	[--clang]
EOF
		exit 0
	fi
	if [  "$1" == '--debug' ]
	then
		build_type='debug'
	elif [ "$1" == '--verbose' ]
	then
		verbose_makefile='yes'
	elif [ "$1" == '--clang' ]
	then
		cmake_cxx_compiler='clang++'
	else
		echo "'$1' is an invalid option"
		bin/run_cmake.sh --help
		exit 1
	fi
	shift
done
# --------------------------------------------------------------------------
export PKG_CONFIG_PATH="$dismod_at_prefix/$cmake_libdir/pkgconfig"
# --------------------------------------------------------------------------
if echo "$dismod_at_prefix" | grep '/dismod_at$' > /dev/null
then
	bin/build_type.sh run_cmake $dismod_at_prefix $build_type
fi
# --------------------------------------------------------------------------
if [ ! -e build ]
then
	echo_eval mkdir build
fi
echo_eval cd build
if [ -e 'CMakeCache.txt' ]
then
	rm CMakeCache.txt
fi
if [ "$cmake_cxx_compiler" == '' ]
then
	compiler=''
else
	compiler="-D CMAKE_CXX_COMPILER=$cmake_cxx_compiler"
	if [ "$cmake_cxx_compiler" == 'clang++' ]
	then
		compiler="$compiler -D CMAKE_C_COMPILER=clang"
	fi
fi
cmake \
	-Wno-dev \
	-D CMAKE_VERBOSE_MAKEFILE=$verbose_makefile \
	-D CMAKE_BUILD_TYPE=$build_type \
	$compiler \
	\
	-D python3_executable=$python3_executable \
	-D extra_cxx_flags="$extra_cxx_flags" \
	-D cmake_libdir="$cmake_libdir" \
	\
	-D dismod_at_prefix="$dismod_at_prefix" \
	-D system_specific_library_list="$system_specific_library_list" \
	..
# --------------------------------------------------------------------------
echo 'run_cmake.sh: OK'
