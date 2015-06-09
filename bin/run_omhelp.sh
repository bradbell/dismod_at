#! /bin/bash -e
# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
if [ "$0" != 'bin/run_omhelp.sh' ]
then
	echo 'bin/run_omhelp.sh must be run from its parent directory'
	exit 1
fi
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
	echo $*
	eval $*
}
# -----------------------------------------------------------------------------
echo_eval rm -rf doc
echo_eval mkdir doc
echo_eval cd doc
#
for xml_flag in '-xml' ''
do
	if [ "$xml_flag" == '-xml' ]
	then
		ext='xml'
	else
		ext='htm'
	fi
	for print_flag in '-printable' ''
	do
		cmd="omhelp ../doc.omh -debug -noframe $xml_flag $print_flag"
		echo "$cmd > omhelp.$ext.log"
		if ! $cmd  > ../omhelp.$ext.log
		then
			cat ../omhelp.$ext.log
			exit 1
		fi
		#
		if grep '^OMhelp Warning:' ../omhelp.$ext.log
		then
			exit 1
		fi
	done
done
echo 'run_omhelp.sh: OK'
exit 0
