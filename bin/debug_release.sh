#! /bin/bash -e
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
ipopt='Ipopt-3.12.6'
cppad_mixed='cppad_mixed-20161016'
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
\\tstd::cout << "Factory: app = " << Ipopt::GetRawPtr(app) << std::endl;\\
\\tstd::cout << "quasi_fixed_ = " << quasi_fixed_ << std::endl;|
b done
#
: two
/^\\tstatus *= *app->Initialize();/! b three
s|^|\\tstd::cout << "Call app->Initilaize\\\\n";\\
|
s|$|\\
\\tstd::cout << "Init: app = " << Ipopt::GetRawPtr(app) << std::endl;|
b done
#
: three
/virtual ApplicationReturnStatus Initialize(/! b four
s|virtual *||
b done
#
: four
/^\\t\\t"accept_after_max_steps", 2\$/! b five
N
s|$|\\
\\tstd::cout << "max_steps: app = " << Ipopt::GetRawPtr(app) << std::endl;|
b done
#
:five
/^\\t\\t\\t"hessian_approximation", "limited-memory");\$/! b six
s|$|\\
\\t\\tstd::cout << "hessian: app = " << Ipopt::GetRawPtr(app) << std::endl;|
b done
#
:six
/^  bool OptionsList::SetStringValue(/! b seven
N
N
N
N
s|\$|\\
    std::cout << "Begin OptionsList::SetStringValue" << std::endl;|
b done
#
: seven
: done
EOF
# -----------------------------------------------------------------------------
# modify source code
source_list="
	build/external/$ipopt/Ipopt/src/Interfaces/IpIpoptApplication.hpp
	build/external/$ipopt/Ipopt/src/Interfaces/IpIpoptApplication.cpp
	build/external/$ipopt/Ipopt/src/Common/IpOptionsList.cpp
	build/external/$cppad_mixed/src/optimize_fixed.cpp
"
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
dir_list="
	build/external/$ipopt/build
	build/external/$cppad_mixed/build
	build
"
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
