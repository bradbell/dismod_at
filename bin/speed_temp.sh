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
branch[one]='fc0a34'
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
#
git reset --hard
#
src_dir='example/user'
out_dir="build/$src_dir"
for version in one two
do
	#
	echo_eval git checkout --quiet ${branch[$version]}
	#
	sed -i bin/run_cmake.sh -e "s|^build_type=.*|build_type='release'|"
	echo "bin/run_cmake.sh > $out_dir/$version.log"
	bin/run_cmake.sh > log.$$
	mv log.$$ $out_dir/$version.log
	#
	for package in cppad cppad_mixed
	do
		echo "bin/install_$package.sh >> $out_dir/$version.log"
		bin/install_$package.sh >> $out_dir/$version.log
	done
	#
	cd build
	make clean
	echo "make dismod_at >> $out_dir/$version.log"
	make dismod_at >> $src_dir/$version.log
	cd ..
	#
	# create database
	random_seed='123'
	n_children='10'
	n_data_per_child='20'
	quasi_fixed='false'
	arguments="$random_seed $n_children $n_data_per_child $quasi_fixed"
	echo "python3 $src_dir/speed.py $arguments >> $out_dir/$version.log"
	python3 $src_dir/speed.py $arguments >> $out_dir/$version.log
	#
	pushd $out_dir > /dev/null
	#
	# timeing test
	program="../../devel/dismod_at"
	echo "time $program example.db fit both 0 >& $out_dir/$version.out"
	( time $program example.db fit both 0 ) >& $version.out
	#
	# memory test
	echo "massif.sh $program example.db fit both 0 >& /dev/null"
	massif.sh $program example.db fit both 0 >& /dev/null
	#
	echo "mv massif.out $out_dir/$version.massif"
	mv massif.out $version.massif
	#
	popd > /dev/null
	#
	# undo changes in bin/run_cmake.sh
	git checkout bin/run_cmake.sh
done
for version in one two
do
	echo_eval tail $out_dir/$version.out
done
