#! /bin/bash -e
# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
#           Copyright (C) 2014-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
# 
# This program is distributed under the terms of the 
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
if [ "$0" != "bin/check_copyright.sh" ]
then
	echo "bin/check_copyright.sh: must be executed from its parent directory"
	exit 1
fi
# -----------------------------------------------------------------------------
list=`find .  \
	\( -name '*.omh' \) -or \
	\( -name '*.cpp' \) -or \
	\( -name '*.hpp' \) -or \
	\( -name '*.py' \) -or \
	\( -name '*.sh' \)  | \
	sed -e '\/junk\./d' -e '\/build\//d' 
`
for file in $list
do
	text='Copyright (C) 2014-.. University of Washington' 
	if ! grep "$text" $file > /dev/null
	then
		echo "copyright missing: use following command to correct this"
		echo "	bin/add_copyright.sh $file"
		exit 1
	fi
done
#
echo 'bin/check_copyright.sh: OK'

