#! /bin/bash -e
# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# =============================================================================
# $OMhelpKeyCharacter=&
# &begin run_cmake.sh&& &newlinech #&&
# &spell
#	cmake makefile cxx std dismod lcppad cholmod xam cpp
#	usr eigen ipopt cppad bools suitesparse devel hpp pthread
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
# Use either 'debug' or 'release' for the type of this build:
# &codep
build_type='debug'
# &&
# Note that if &icode dismod_at_prefix&& ends in &code dismod_at&&,
# &code run_cmake.sh&& will use a link from the prefix to
# &icode%dismod_at_prefix%.debug%&& or
# &icode%dismod_at_prefix%.release%&&
# depending on the choice &icode build_type&&.
#
# &head python3_executable&&
# Path to the python3 executable on this machine:
# &codep
python3_executable='/usr/bin/python3'
# &&
# An alternative might be &code '/usr/local/anaconda3-current/bin/python'&&.
#
# &head extra_cxx_flags&&
# Extra C++ flags used during compilation:
# &codep
extra_cxx_flags='-std=c++11 -Wall -DCPPAD_DEBUG_AND_RELEASE'
# &&
# An alternative might be $code '-Wall'.
#
# &head dismod_at_prefix&&
# Prefix where dismod_at will be installed:
# &codep
dismod_at_prefix="$HOME/prefix/dismod_at"
# &&
#
# &head Prefixes&&
# Prefixes where the
# &cref/special requirements/install_unix/Special Requirements/&&
# will be installed:
# &codep
eigen_prefix="$HOME/prefix/dismod_at/eigen"
ipopt_prefix="$HOME/prefix/dismod_at"
cppad_prefix="$HOME/prefix/dismod_at"
suitesparse_prefix="$HOME/prefix/dismod_at"
# &&
# Note that the Eigen package is installed in a special prefix
# so that we can suppress warnings coming from its include files
# (without suppressing warnings from other include files).
#
# &subhead Debug and Release&&
# If &icode dismod_at_prefix&& ends in &code dismod_at&&,
# all the prefixes must be the same,
# except for the Eigen prefix which has an extra sub-directory at the end.
# In this case,
# &code bin/run_cmake.sh&& can be used to switch between a debug and release
# version of &code dismod_at&&
# (depending on &cref/build_type/run_cmake.sh/build_type/&&).
# The debug version will have an extra &code .debug&& and the release version
# will have an extra &code .release&& at the end of the prefix.
# In addition, a soft link will be used to map the prefix to either
# the debug or release version.
# Note that the same technique will be used to map the &code build&&
# directory to the debug or release version.
# Also note that if you are using both a debug and release, both versions
# of the &cref/special requirements/install_unix/Special Requirements/&&
# will need to be installed.
#
# &head Choosing C++ Compiler&&
# Which c++ compiler should cmake use (empty means cmake will choose it).
# &codep
cmake_cxx_compiler=''
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
switch_build_type='no'
switch_link_type='no'
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
	[--verbose] \\
	[--switch_build_type]
	[--switch_link_type]
EOF
		exit 0
	fi
	if [ "$1" == '--verbose' ]
	then
		verbose_makefile='yes'
	elif [ "$1" == '--switch_build_type' ]
	then
		switch_build_type='yes'
	elif [ "$1" == '--switch_link_type' ]
	then
		switch_link_type='yes'
	else
		echo "'$1' is an invalid option"
		bin/run_cmake.sh --help
		exit 1
	fi
	shift
done
# final build_type
if [ "$switch_build_type" == 'yes' ]
then
	if [ "$build_type" == 'debug' ]
	then
		build_type='release'
	else
		build_type='debug'
	fi
fi
# final link_type
link_type="$build_type"
if [ "$switch_link_type" == 'yes' ]
then
	if [ "$build_type" == 'debug' ]
	then
		link_type='release'
	else
		link_type='debug'
	fi
fi
# --------------------------------------------------------------------------
libdir=`bin/libdir.sh`
export PKG_CONFIG_PATH="$ipopt_prefix/$libdir/pkgconfig"
# --------------------------------------------------------------------------
if echo "$dismod_at_prefix" | grep '/dismod_at$' > /dev/null
then
	bin/build_type.sh run_cmake $dismod_at_prefix $link_type
fi
# --------------------------------------------------------------------------
if [ ! -e build ]
then
	echo_eval mkdir build
fi
echo_eval cd build
if [ -e 'CMakeFiles' ]
then
	rm -r CMakeFiles
fi
if [ -e 'CMakeCache.txt' ]
then
	rm CMakeCache.txt
fi
if [ "$cmake_cxx_compiler" == '' ]
then
	compiler=''
else
	compiler="-D CMAKE_CXX_COMPILER=$cmake_cxx_compiler"
fi
cmake \
	-Wno-dev \
	-D CMAKE_VERBOSE_MAKEFILE=$verbose_makefile \
	-D CMAKE_BUILD_TYPE=$build_type \
	$compiler \
	\
	-D python3_executable=$python3_executable \
	-D extra_cxx_flags="$extra_cxx_flags" \
	-D cmake_libdir="$libdir" \
	\
	-D dismod_at_prefix="$dismod_at_prefix" \
	-D ipopt_prefix="$ipopt_prefix" \
	-D eigen_prefix="$eigen_prefix" \
	-D cppad_prefix="$cppad_prefix" \
	-D suitesparse_prefix="$suitesparse_prefix" \
	-D system_specific_library_list="$system_specific_library_list" \
	..
# --------------------------------------------------------------------------
echo 'run_cmake.sh: OK'
