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
# ---------------------------------------------------------------------------
if [ "$0" != "bin/debug_release.sh" ]
then
	echo "bin/debug_release.sh: must be executed from its parent directory"
	exit 1
fi
# -----------------------------------------------------------------------------
cat << EOF > junk.sed
/^ *IpoptApplication::Initialize(bool allow_clobber)/! b one
N
s|\\n\\( *\\){|&  std::cout << "Begin IpoptApplication::Initialize\\\\n";\\
\\1 |
b done
#
: one
/app = IpoptApplicationFactory()/! b two
s|$|\\
\\tstd::cout << "app = " << Ipopt::GetRawPtr(app) << std::endl;|
b done
#
: two
/^\\tstatus *= *app->Initialize();/! b three
s|^|\\tstd::cout << "Call app->Initilaize\\\\n";\\
|
s|^|\\tstd::cout << "app = " << Ipopt::GetRawPtr(app) << std::endl;\\
|
s|$|\\
\\tstd::cout << "Done app->Initilaize\\\\n";|
b done
#
: three
/virtual ApplicationReturnStatus Initialize(/! b four
s|virtual *||
b done
#
: four
: done
EOF
# -----------------------------------------------------------------------------
# modify source code
source_list='
	build/external/Ipopt-3.12.6/Ipopt/src/Interfaces/IpIpoptApplication.hpp
	build/external/Ipopt-3.12.6/Ipopt/src/Interfaces/IpIpoptApplication.cpp
	build/external/cppad_mixed-20161018/src/optimize_fixed.cpp
'
for src in $source_list
do
	dir=`echo $src | sed -e 's|/[^/]*$||'`
	file=`echo $src | sed -e 's|^.*/||'`
	if [ ! -e "$dir/bak" ]
	then
		mkdir "$dir/bak"
	fi
	if [ ! -e "$dir/bak/$file" ]
	then
		echo_eval cp $dir/$file $dir/bak/$file
	fi
	sed -f junk.sed "$dir/bak/$file" > junk.tmp
	if ! diff "$dir/$file" junk.tmp > /dev/null
	then
		echo "sed -f junk.sed -i $dir/$file"
		mv junk.tmp "$dir/$file"
	fi
done
read -p 'debug_release.sh: hit return to continue' text
# -----------------------------------------------------------------------------
# rebuild
start_dir=`pwd`
dir_list='
	build/external/Ipopt-3.12.6/build
	build/external/cppad_mixed-20161018/build
	build
'
for build_dir in $dir_list
do
	echo_eval cd $build_dir
	echo_eval make install
	echo_eval cd $start_dir
done
# -----------------------------------------------------------------------------
echo 'Debugging print statements have been added as follows:'
for src in $source_list
do
	echo '--------------------------------------------------------------------'
	dir=`echo $src | sed -e 's|/[^/]*$||'`
	file=`echo $src | sed -e 's|^.*/||'`
	echo "$src"
	if diff "$dir/bak/$file" "$dir/$file"
	then
		echo 'debug_release.sh: Error: no changes to this file'
		exit 1
	fi
done
echo '--------------------------------------------------------------------'
echo 'debug_release.sh: OK'
# END BASH
