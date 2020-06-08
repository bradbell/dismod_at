#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
if [ "$0" != "bin/old_external.sh" ]
then
	echo 'bin/old_external.sh: must be run from parent directory'
	exit 1
fi
if [ "$1" == '' ]
then
	echo 'bin/old_external.sh git_hash'
	echo 'assumes current version of build/external corresponds to master branch'
	echo 'moves it to build/external.master and creates old version of build/external'
	exit 1
fi
git_hash="$1"
for branch in master $git_hash
do
	if [ -e "build/external.$branch" ]
	then
		echo "bin/old_external.sh: build/external.$branch exists"
		echo 'prehaps a previous run of bin/old_external.sh failed'
		exit 1
	fi
done
if ! git branch | grep '^* *master$' > /dev/null
then
	echo 'bin/old_external.sh: must be executed from master branch'
	exit 1
fi
# -----------------------------------------------------------------------------
# build_type
cmd=`grep '^build_type=' bin/run_cmake.sh`
eval $cmd
if [ "$build_type" != 'debug' ] && [ "$build_type" != 'release' ]
then
	echo 'bin/old_extrnal.sh: build_type in bin/run_cmake.sh is not debug or release'
	exit 1
fi
# -----------------------------------------------------------------------------
# dismod_at_prefix
cmd=`grep '^dismod_at_prefix=' bin/run_cmake.sh`
eval $cmd
if [ -e "$dismod_at_prefix" ]
then
	if [ ! -L "$dismod_at_prefix" ]
	then
		echo "bin/old_extrnal.sh: dismod_at_preifx=$dismod_at_prefix"
		echo 'is not a symbolic link'
		exit 1
	fi
	echo_eval rm "$dismod_at_prefix"
fi
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
echo_eval git checkout --quiet "$git_hash"
echo_eval mv build/external build/external.master
echo_eval bin/example_install.sh
# -----------------------------------------------------------------------------
cat << EOF
Use the following commands to restore master version of externals
	git checkout master
	mv build/external build/external.$git_hash
	mv build/external.master build/extranal
	bin/example_install.sh
EOF
# -----------------------------------------------------------------------------
echo 'bin/old_external.sh: OK'
exit 0
