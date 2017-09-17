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
'
spell_files='
'
no_change_files='
'
#
rename_cmd='s|speed/simulated.py|example/user/speed.py|'
spell_cmd='s|^$spell|&\n\tnslist|'
#
cat << EOF > junk.sed
s|\$cref/ode grid/glossary/Ode Grid/\\\$|\$cref ode_grid\$|
#
s|\$cref/ode time grid/glossary/Ode Grid/Time, t_j/\\\$|\$cref/ode time grid/ode_grid/Time, t_j/\$|
s|\$cref/ode age grid/glossary/Ode Grid/Age, a_i/\\\$|\$cref/ode age grid/ode_grid/Age, a_i/\$|
#
s|\$cref/t_[ij]/glossary/Ode Grid/Time, t_j/\\\$|\$cref/t_j/ode_grid/Time, t_j/\$|
s|\$cref/a_i/glossary/Ode Grid/Age, a_i/\\\$|\$cref/a_i/ode_grid/Age, a_i/\$|
#
s|\$cref/bilinear interpolation/glossary/Bilinear Interpolant/\$\\\$|\$cref bilinear\$\$ interpolation|
s|\$cref/bilinear interpolant/glossary/Bilinear Interpolant/\$\\\$|\$cref bilinear\$\$ interpolant|
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
cp $HOME/trash/batch_edit.sh bin/batch_edit.sh
echo 'batch_edit.sh: OK'
exit 0
