# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
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
# -----------------------------------------------------------------------------
# determine if debug or release
if grep "cmake_build_type='RELEASE'" ../../bin/run_cmake.sh > /dev/null
then
	build_type='-O3 -DNDEBUG'
else
	build_type='-g -O0 -pg'
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
# devel library flags
file='../../build/devel/libdevel.a'
if [ ! -e "$file" ]
then
	echo "./test_one.sh: Cannot find $file."
	exit 1
fi
lib_dir=`echo $file | sed -e 's|/[^/]*$||'`
lib_name=`echo $file | sed -e 's|.*/lib||' -e 's|[.][^.]*$||'`
dismod_at_lib="-L $lib_dir -l$lib_name"
#
# mixed_cppad library flags (assume same lib_dir)
file='../../build/devel/libmixed_cppad.a'
if [ ! -e "$file" ]
then
	echo "./test_one.sh: Cannot find $file."
	exit 1
fi
lib_name=`echo $file | sed -e 's|.*/lib||' -e 's|[.][^.]*$||'`
dismod_at_lib="$dismod_at_lib -l$lib_name"
#
# mixed_cppad_eigen library flags (assume same lib_dir)
file='../../build/devel/libmixed_cppad_eigen.a'
if [ ! -e "$file" ]
then
	echo "./test_one.sh: Cannot find $file."
	exit 1
fi
lib_name=`echo $file | sed -e 's|.*/lib||' -e 's|[.][^.]*$||'`
dismod_at_lib="$dismod_at_lib -l$lib_name -lmixed_cppad"
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
	-std=c++11 \
	$build_type \
	-I ../../include \
	-I $HOME/prefix/dismod_at/include \
	junk.cpp \
	$dir_file \
	$dismod_at_lib \
	$ipopt_libs \
	-lsqlite3 \
	-lgsl \
	-lgslcblas \
	-o junk
#
# run
echo "./junk"
./junk
#
exit 0
read -p 'Run gprof [y/n] ?' response
if [ "$response" != 'y' ]
then
	if [ -e 'example.db' ]
	then
		echo_eval rm example.db
	fi
	exit 0
fi
#
if ( echo "$build_type" | grep '\-pg' > /dev/null )
then
	echo "gprof junk | sed -f gprof.sed > gprof.out"
	gprof junk | sed -f gprof.sed > gprof.out
fi
#
if [ -e 'example.db' ]
then
	echo_eval rm example.db
fi
