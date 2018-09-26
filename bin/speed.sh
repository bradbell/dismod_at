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
# parameters that can be changed
# If branch 2 is empty, use new directory to replace source code
random_seed='123'
n_children='20'
n_data_per_child='20'
quasi_fixed='false'
branch1='master'
branch2=''
install_cppad='yes'
install_cppad_mixed='yes'
# -----------------------------------------------------------------------------
if [ "$0" != 'bin/speed.sh' ]
then
	echo 'bin/speed.sh must be run from its parent directory'
	exit 1
fi
current=`git branch | sed -n -e '/^\*/p' | sed -e 's|^\* *||'`
if [ "$current" != 'master' ]
then
	echo 'bin/speed.sh must be run from master branch'
	exit 1
fi
if [ "$branch1" == '' ]
then
	echo 'bin/speed.sh: branch1 cannot be empty'
	exit 1
fi
if [ "$branch1" == "$branch2" ]
then
	echo 'bin/speed.sh: branch1 and branch2 must be different'
	echo 'branch2 can be empty in which case new is used to replace soruce'
	exit 1
fi
# -----------------------------------------------------------------------------
output_file_list=''
for version in one two
do
	if [ "$version" == 'one' ]
	then
		git reset --hard
		git checkout $branch1
		name="$branch1"
	else
		if [ "$branch2" == '' ]
		then
			echo 'git_new.sh from > /dev/null'
			git_new.sh from > /dev/null
			name='new'
		else
			git checkout $branch2
			name="$branch2"
		fi
	fi
	#
	# build release version
	sed -i bin/run_cmake.sh -e "s|^build_type=.*|build_type='release'|"
	#
	# run cmake
	echo "bin/run_cmake.sh > build/$name.log"
	bin/run_cmake.sh > build/$name.log
	#
	# install cppad
	if [ "$install_cppad" == 'yes' ]
	then
		echo "bin/install_cppad.sh >> build/$name.log"
		bin/install_cppad.sh >> build/$name.log
	fi
	#
	# install cppad_mixed
	if [ "$install_cppad_mixed" == 'yes' ]
	then
		echo "bin/install_cppad_mixed.sh >> build/$name.log"
		bin/install_cppad_mixed.sh >> build/$name.log
	fi
	#
	# build dismod_at
	cd build
	make clean
	echo "make dismod_at >> build/$name.log"
	make dismod_at >> $name.log
	cd ..
	#
	# create database
	arguments="$random_seed $n_children $n_data_per_child $quasi_fixed"
	echo "python3 example/user/speed.py $arguments >> build/$name.log"
	python3 example/user/speed.py $arguments >> build/$name.log
	#
	# change into database directory
	pushd build/example/user > /dev/null
	#
	# run timeing test
	program="../../devel/dismod_at"
	echo "time $program example.db fit both 0 >& build/$name.out"
	( time $program example.db fit both 0 ) >& ../../$name.out
	#
	# run memory test
	echo "massif.sh $program example.db fit both 0 >& /dev/null"
	massif.sh $program example.db fit both 0 >& /dev/null
	#
	echo "mv massif.out build/$name.massif"
	mv massif.out ../../$name.massif
	#
	popd > /dev/null
	#
	# undo changes in bin/run_cmake.sh
	git checkout bin/run_cmake.sh
done
