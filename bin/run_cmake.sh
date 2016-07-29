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
# &head verbose_makefile&&
# Use 'NO' for normal and 'YES' for verbose make output:
# &codep
verbose_makefile='NO'
# &&
#
# &head build_type&&
# Use either &code 'DEBUG'&& or &code 'RELEASE'&& for the type of this build:
# &codep
build_type='debug'
# &&
# Note that &code run_cmake.sh&& looks for a &icode%dismod_prefix%.debug%&&
# and &icode%dismod_prefix%.release%&& and uses them if they are present.
# Also note that it builds dismod_at in &code build.debug&& or
# &code build.release&& depending on &icode build_type&&.
#
# &head python3_executable&&
# Path to the python3 executable on this machine:
# &codep
python3_executable='/usr/bin/python3'
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
# python3_executable='/usr/local/anaconda3-current/bin/python'
# extra_cxx_flags='-Wall'
# &&
# &end
# ============================================================================
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
while [ "$1" != '' ]
do
	if [ "$1" == '--help' ]
	then
		cat << EOF
usage: bin/run_cmake.sh \\
	[--help] \\
	[--verbose] \\
	[--release]
EOF
		exit 0
	fi
	if [ "$1" == '--verbose' ]
	then
		verbose_makefile='YES'
	elif [ "$1" == '--release' ]
	then
		build_type='release'
	else
		echo "'$1' is an invalid option"
		bin/run_cmake.sh --help
		exit 1
	fi
	shift
done
# ---------------------------------------------------------------------------
if [ -e "$dismod_at_prefix.$build_type" ]
then
	if [ -e "$dismod_at_prefix" ]
	then
		echo_eval rm "$dismod_at_prefix"
	fi
	echo_eval ln -s $dismod_at_prefix.$build_type $dismod_at_prefix
fi
if [ ! -e "build.$build_type" ]
then
	echo_eval mkdir "build.$build_type"
fi
if [ -e build ]
then
	echo_eval rm build
fi
echo_eval ln -s build.$build_type build
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
	-D CMAKE_VERBOSE_MAKEFILE=$verbose_makefile \
	-D CMAKE_BUILD_TYPE=$build_type \
	\
	-D python3_executable=$python3_executable \
	-D extra_cxx_flags="$extra_cxx_flags" \
	\
	-D dismod_at_prefix="$dismod_at_prefix" \
	-D ipopt_prefix="$ipopt_prefix" \
	-D eigen_prefix="$eigen_prefix" \
	-D cppad_prefix="$cppad_prefix" \
	-D suitesparse_prefix="$suitesparse_prefix" \
	..
