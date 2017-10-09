#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Parameters that can be changed
random_seed='1234'
n_children='20'
n_data_per_child='40'
quasi_fixed='false'
# ---------------------------------------------------------------------------
# convert to release build
sed -i bin/run_cmake.sh \
	-e 's|^build_type=.*|build_type=release|'
bin/run_cmake.sh
# ---------------------------------------------------------------------------
# create database
cd build
echo_eval make dismod_at
cd ..
echo_eval python3 example/user/speed.py \
	$random_seed $n_children $n_data_per_child $quasi_fixed
# ---------------------------------------------------------------------------
# locations relative to build directory
database='example/user/example.db'
program='devel/dismod_at'
#
for atomic in yes no
do
	# install cppad_mixed with this setting for atomic cholesky
	sed -i bin/install_cppad_mixed.sh \
		-e "s|^use_atomic_cholesky=.*|use_atomic_cholesky=$atomic|"
	#
	bin/install_cppad_mixed.sh
	cd build
	make dismod_at
	#
	# timing test
	echo "(time $program $database fit both 2> time.$atomic"
	(time $program $database fit both 0) 2> time.$atomic
	#
	# memory test
	echo_eval massif.sh $program $database fit both 0
	echo_eval mv massif.out massif.$atomic
	#
	cd ..
done
