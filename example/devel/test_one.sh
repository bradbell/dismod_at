# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
# usage
if [ "$1" == '' ]
then
	echo 'usage: ./test_one.sh dir/file'
	echo 'dir:  is a subdirectory  of the current directory'
	echo 'file: is a *.cpp file containing a test/example'
	exit 1
fi
#
# dir/file
dir_file="$1"
if [ ! -e "$dir_file" ]
then
	echo "Cannot find $dir_file"
	exit 1
fi
#
# fun_name
fun_name=`grep '^bool *[a-zA-Z0-9_]* *( *void *)' $dir_file | \
	sed -e 's|^bool *\([a-zA-Z0-9_]*\) *( *void *) *|\1|'`
#
# junk.cpp
if ! grep '\/\/ summary report' example_devel.cpp > /dev/null
then
	echo 'Cannot find "// summary report" at end of example_devel.cpp'
	exit 1
fi
sed \
	-e '/^\tRUN( *[a-zA-Z0-9_]* *)/d' \
	-e "s|// summary report|RUN($fun_name);\n\t&|" \
	< example_devel.cpp > junk.cpp
#
# compile
echo_eval g++ \
	-g \
	-O0 \
	-I ../../include \
	-I $HOME/prefix/cppad/include \
	junk.cpp \
	$dir_file \
	-L ../../build/devel -ldevel_lib \
	-o junk
#
# run
echo "./junk"
./junk

