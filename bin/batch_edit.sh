#! /bin/bash -e
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
new_directories='
'
rename_files='
	omh/table/smooth_list_table.omh
	devel/table/get_smooth_list.cpp
	example/devel/table/get_smooth_list_xam.cpp
	include/dismod_at/get_smooth_list.hpp
	example/table/smooth_list_table.py
'
spell_files='
	omh/table/rate_table.omh
	omh/table/nslist_pair_table.omh
	example/table/nslist_pair_table.py
	devel/table/get_nslist_pair.cpp
	example/devel/table/get_nslist_pair_xam.cpp
'
no_change_files='
'
#
rename_cmd='s|smooth_list|nslist_pair|'
spell_cmd='s|^$spell|&\n\tnslist|'
#
cat << EOF > junk.sed
s|smooth_list_index\\(  *\\)|nslist_id        \\1|g
s|smooth_list_id|nslist_pair_id|g
s|smooth_list_table|nslist_pair_table|g
#
s|'smooth_list':|'nslist_pair':|g
#
s|smooth_list_index|nslist_id|g
s|smooth_list|nslist_pair|g
s|SMOOTH_LIST|NSLIST_PAIR|g
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
list_all=`bin/ls_files.sh`
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
list='
'
for file in $list
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
cp $HOME/trash/batch_edit.sh bin/batch_edit.sh
echo 'batch_edit.sh: OK'
exit 0
