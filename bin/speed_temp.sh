#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# cppad_mixed: C++ Laplace Approximation of Mixed Effects Models
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# This scipt demonstrates that changing cppad-20171215 to cppad-20180503
# uses less mmeory and does not affect speed.
declare -A branch
branch[one]='417a42'
branch[two]='master'
# -----------------------------------------------------------------------------
if [ "$0" != 'bin/speed_temp.sh' ]
then
	echo 'bin/speed_temp.sh must be run from its parent directory'
	exit 1
fi
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
# temporary change to bin/run_cmake.sh to build release version
sed -i bin/run_cmake.sh -e "s|^build_type=.*|build_type='release'|"
bin/run_cmake.sh
#
for version in one two
do
	build_dir='build/devel'
	src_dir='example/user'
	program="dismod_at.$version"
	if [ ! -e $build_dir/$program ] || [ ! -e $build_dir/dismod_at ]
	then
		echo_eval git checkout ${branch[$version]}
		#
		echo_eval bin/install_cppad.sh
		echo_eval bin/install_cppad_mixed.sh
		#
		pushd $build_dir
		echo_eval make dismod_at
		echo_eval cp dismod_at $program
		popd
	fi
	#
	# create database
	if [ $version == 'one' ]
	then
		random_seed='123'
		n_children='10'
		n_data_per_child='20'
		quasi_fixed='false'
		arguments="$random_seed $n_children $n_data_per_child $quasi_fixed"
		echo "python3 $src_dir/speed.py $arguments > build/$src_dir/speed.out"
		python3 $src_dir/speed.py $arguments > build/$src_dir/speed.out
	fi
	#
	dir="build/$src_dir"
	pushd $dir
	#
	# timeing test
	echo "time ../../devel/$program example.db fit both 0 >& $dir/$version.out"
	( time ../../devel/$program example.db fit both 0 ) >& $version.out
	#
	# memory test
	echo "massif.sh ../../devel/$program example.db fit both 0 >& /dev/null"
	massif.sh ../../devel/$program example.db fit both 0 >& /dev/null
	#
	echo_eval mv massif.out $version.massif
	#
	popd
done
# undo changes in bin/run_cmake.sh
git checkout bin/run_cmake.sh
