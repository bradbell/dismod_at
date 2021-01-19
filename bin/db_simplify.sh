#! /bin/bash -e
# -----------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -----------------------------------------------------------------------------
if [ "$0" != "bin/db_simplify.sh" ]
then
	echo "bin/db_simplify.sh: must be executed from its parent directory"
	exit 1
fi
if [ "$1" != 'Diabetes' ] && [ "$1" != 'Chrons' ] && [ "$1" != 'Kidney' ]
then
	echo 'usage: bin/db_simplify.sh: disease'
	echo 'Changes db_simplify.py to the specified disease'
	echo 'where disease is Diabetes, Chrons, or Kidney'
	exit 1
fi
if [ "$(git status -s)" != '' ]
then
	echo 'db_simplify: "git stauts -s" is not empty'
	exit 1
fi
disease="$1"
file='bin/db_simplify.py'
#
found='no'
count=0
while [ "$found" == 'no' ]
do
	count=$(expr $count + 1)
	if [ "$count" == 100 ]
	then
		exit "Cannot find previous version for $disease"
		exit 1
	fi
	if git show master~$count:$file | \
		grep "disease_specific_name *= *'$disease'" > /dev/null
	then
		found='yes'
	fi
done
#
begin_user='# BEGIN: Settings that User Can Change'
if ! grep "$begin_user" $file > /dev/null
then
	echo 'db_simplify.sh: begin_user Error'
	exit 1
fi
#
end_user='# END: Settings that User Can Change'
if ! grep "$end_user" $file > /dev/null
then
	echo 'db_simplify.sh: end_user Error'
	exit 1
fi
#
# Before begin_user
sed -e "/$begin_user/,\$d" $file > junk.py
#
# begin_user to end_user
git show master~$count:$file | \
	sed -n -e "/$begin_user/,/$end_user/p" >> junk.py
#
# seciton after end_user
sed -e "1,/$end_user/d" $file >> junk.py
#
echo "$disease verison of $file is in ./junk.py."
echo 'Execute the following command and test the result:'
echo 'cp junk.py bin/db_simplify.py'
echo
echo 'db_simplify.sh: OK'
exit 0
