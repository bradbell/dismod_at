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
list='
	Crohns
	t1_diabetes
	Kidney
'
list=$(echo $list | sed -e 's|[\t\n]| |g')
ok='no'
for disease in $list
do
	if [ "$1" == "$disease" ]
	then
		ok='yes'
	fi
done
if [ "$ok" == 'no' ]
then
cat << EOF
usage: bin/db_simplify.sh: disease [new_name]

If new_name is not present, changes db_simplify.py to the specified disease
where disease is in the following list:
	$list
If new_name is presnet, the current diesase must as specified and its name
will be changed to new_name.
EOF
	exit 1
fi
for ext in py sh
do
	if ! git diff --exit-code bin/db_simplify.$ext
	then
		echo "bin/db_simplify $disease $new_name"
		echo "db_simplify.$ext has changes that need to be checked in"
		exit 1
	fi
done
disease="$1"
new_name="$2"
file='bin/db_simplify.py'
#
if [ "$new_name" != '' ]
then
	if ! grep "'$disease'" bin/db_simplify.py
	then
		echo "bin/db_simplify $disease $new_name"
		echo "'$disease' is not in bin/db_simplify.py"
		exit 1
	fi
	echo sed -i bin/db_simplify.py -e "s|'$disease'|'$new_name'|g"
	sed -i bin/db_simplify.py -e "s|'$disease'|'$new_name'|g"
	#
	echo sed -i bin/db_simplify.sh -e "s|\(^[ \t]*\)$disease|\1$new_name|"
	sed -i bin/db_simplify.sh -e "s|^\([ \t]*\)$disease|\1$new_name|"
	#
	if [ -e "ihme_db/$disease" ]
	then
		echo "mv ihme_db/$disease ihme_db/$new_name"
		echo_eval mv ihme_db/$disease ihme_db/$new_name
	fi
	echo 'Check changes to bin/db_simplify.py and bin/db_simplify.sh.'
	echo 'Then commit the changes.'
	exit 0
fi
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
