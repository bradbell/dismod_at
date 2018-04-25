#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-18 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
new_directories='
'
rename_files='
	example/user/fit_fixed.py
'
spell_files='
'
no_change_files='
'
#
rename_cmd='s|fit_fixed.py|fit_fixed_both.py|'
# spell_cmd='s|^$spell|&\n\tsim|'
spell_cmd='s|^# $spell|&\n#\tsim|'
#
cat << EOF > junk.sed
s|fit_fixed|fit_fixed_both|g
EOF
# -----------------------------------------------------------------------------
if [ "$0" != "bin/batch_edit.sh" ]
then
	echo "bin/batch_edit.sh: must be executed from its parent directory"
	exit 1
fi
# -----------------------------------------------------------------------------
cp bin/batch_edit.sh $HOME/trash/batch_edit.sh
git reset --hard
# ----------------------------------------------------------------------------
# new directories
for dir in $new_directories
do
	if [ ! -e $dir ]
	then
		mkdir $dir
	fi
done
# ----------------------------------------------------------------------------
# rename files
for old in $rename_files
do
	new=`echo $old | sed -e "$rename_cmd"`
	echo_eval git mv $old $new
done
# ----------------------------------------------------------------------------
list_all=`git ls-files`
for file in $list_all
do
	if [ "$file" != 'bin/batch_edit.sh' ]
	then
		sed -f junk.sed $file > junk.$$
		if diff $file junk.$$ > /dev/null
		then
			rm junk.$$
		else
			echo "sed -f junk.sed -i $file"
			if [ -x "$file" ]
			then
				mv junk.$$ $file
				chmod +x $file
			else
				mv junk.$$ $file
			fi
		fi
	fi
done
# ----------------------------------------------------------------------------
# files that have spelling changes
for file in $spell_files
do
	sed -e "$spell_cmd" -i $file
done
# ----------------------------------------------------------------------------
# files that should not change at all
for file in $no_change_files
do
	echo_eval git checkout $file
done
# ----------------------------------------------------------------------------
# files that have hand edited and stored using 'git_new.sh to'
git_new='yes'
for name in add_list mod_list rm_list
do
	if [ ! -e new/$name ]
	then
		git_new='no'
	fi
done
if [ "$git_new" == 'yes' ]
then
	git_new.sh from
fi
# ----------------------------------------------------------------------------
if ! check_copyright.sh >> /dev/null
then
	echo 'check_copyright.sh: some changes'
else
	echo 'check_copyright.sh: no changes'
fi
# ----------------------------------------------------------------------------
cp $HOME/trash/batch_edit.sh bin/batch_edit.sh
echo 'batch_edit.sh: OK'
exit 0
