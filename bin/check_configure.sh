#! /bin/bash -e
# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
if [ ! -e "bin/check_configure.sh" ]
then
	echo "bin/check_configure.sh: must be executed from its parent directory"
	exit 1
fi
file_list=`git ls-files | sed -n -e '/\.cpp$/p'`
define_list='
	DISMOD_AT_VERSION
	DISMOD_AT_NULL_PTR
'
for file in $file_list
do
	required='no'
	present='no'
	for name in $define_list
	do
		if grep $name $file > /dev/null
		then
			required='yes'
		fi
	done
	if grep '# *include *<dismod_at/configure.hpp>' $file > /dev/null
	then
		present='yes'
	fi
	if [ "$required" == 'yes' ] && [ "$present" == 'no' ]
	then
		echo "missing: # include <dismod_at/configure.hpp>"
		echo "	$file"
		exit 1
	fi
	if [ "$required" == 'no' ] && [ "$present" == 'yes' ]
	then
		echo "unecessary: # include <dismod_at/configure.hpp>"
		echo "	$file"
		exit 1
	fi
done
echo 'check_configure.sh: OK'
exit 0
