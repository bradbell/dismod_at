# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
# usage
if [ "$1" == '' ]
then
	echo 'usage: ./test_one.sh test_file'
	exit 1
fi
#
# test_file
test_file="$1"
if [ ! -e "$test_file" ]
then
	echo "Cannot find $test_file"
	exit 1
fi
#
# fun_name
fun_name=`grep '^bool *[a-zA-Z0-9_]* *( *void *)' $test_file | \
	sed -e 's|^bool *\([a-zA-Z0-9_]*\) *( *void *) *|\1|'`
echo "fun_name=$fun_name"
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
# devel library flags
lib_file='../../build/devel/libdevel.a'
if [ ! -e "$lib_file" ]
then
	echo "./test_one.sh: Cannot find $lib_file."
	exit 1
fi
lib_dir=`echo $lib_file | sed -e 's|/[^/]*$||'`
lib_name=`echo $lib_file | sed -e 's|.*/lib||' -e 's|[.][^.]*$||'`
dismod_at_lib="-L $lib_dir -l$lib_name"
#
# cppad_mixed library flags (assume same lib_dir)
file='../../build/devel/libcppad_mixed.a'
if [ ! -e "$file" ]
then
	echo "./test_one.sh: Cannot find $file."
	exit 1
fi
lib_name=`echo $file | sed -e 's|.*/lib||' -e 's|[.][^.]*$||'`
dismod_at_lib="$dismod_at_lib -l$lib_name"
#
# cppad_mixed_eigen library flags (assume same lib_dir)
file='../../build/devel/libcppad_mixed_eigen.a'
if [ ! -e "$file" ]
then
	echo "./test_one.sh: Cannot find $file."
	exit 1
fi
lib_name=`echo $file | sed -e 's|.*/lib||' -e 's|[.][^.]*$||'`
dismod_at_lib="$dismod_at_lib -l$lib_name -lcppad_mixed"
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
	-std=c++11 \
	-Wall \
	-I ../../include \
	-I $HOME/prefix/dismod_at/include \
	junk.cpp \
	$test_file \
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
if [ -e 'example.db' ]
then
	echo_eval rm example.db
fi
