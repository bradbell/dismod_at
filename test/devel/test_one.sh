# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
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
	echo 'usage: ./test_one.sh file'
	exit 1
fi
#
# file
file="$1"
if [ ! -e "$file" ]
then
	echo "Cannot find $file"
	exit 1
fi
#
# fun_name
fun_name=`grep '^bool *[a-zA-Z0-9_]* *( *void *)' $file | \
	sed -e 's|^bool *\([a-zA-Z0-9_]*\) *( *void *) *|\1|'`
#
# junk.cpp
if ! grep '\/\/ summary report' test_devel.cpp > /dev/null
then
	echo 'Cannot find "// summary report" at end of test_devel.cpp'
	exit 1
fi
sed \
	-e '/^\tRUN( *[a-zA-Z0-9_]* *)/d' \
	-e "s|// summary report|RUN($fun_name);\n\t&|" \
	< test_devel.cpp > junk.cpp
#
# dismod_at library flags
lib_file='../../build/devel/libdevel_lib.a'
if [ ! -e "$lib_file" ]
then
	echo "./test_one.sh: Cannot find $lib_file."
	exit 1
fi
lib_dir=`echo $lib_file | sed -e 's|/[^/]*$||'`
lib_name=`echo $lib_file | sed -e 's|.*/lib||' -e 's|[.][^.]*$||'`
dismod_at_lib="-L $lib_dir -l$lib_name"
#
# libarary flags necessary to use ipopt
ipopt_libs=`pkg-config --libs ipopt`
#
# build library
pushd ../../build
make $lib_name
popd
#
# compile
echo_eval g++ \
	-g \
	-O0 \
	-I ../../include \
	-I $HOME/prefix/dismod_at/include \
	junk.cpp \
	$file \
	$dismod_at_lib \
	$ipopt_libs \
	-lsqlite3 \
	-o junk
#
# run
echo "./junk"
./junk
#
if [ -e 'example.db' ]
then
	echo_eval rm example.db
fi
