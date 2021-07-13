#! /bin/bash -e
# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-21 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# test install of python module and executable
# ---------------------------------------------------------------------------
disease_list=$(
	git ls-files python/dismod_at/ihme |
	sed -e 's|.*/||' -e 's|\.py||'
)
if [ "$2" == '' ]
then
	echo 'usage: bin/fit_ihme.sh disease random_seed'
	echo 'were disease is "all" or in the following list:'
	echo $disease_list
	exit 1
fi
if [ "$1" != 'all' ]
then
	disease_list="$1"
fi
for disease in $disease_list
do
	if [ -e "ihme_db/$disease" ]
	then
		echo_eval rm -r ihme_db/$disease
	fi
done
random_seed="$2"
number_sample='20'
for disease in $disease_list
do
	fit_ok='yes'
	for which_fit in no_ode yes_ode students
	do
		base_cmd="bin/fit_ihme.py ihme_db $disease $which_fit"
		if [ "$which_fit" == 'no_ode' ]
		then
			cmd="$base_cmd $random_seed"
		else
			cmd="$base_cmd"
		fi
		if [ "$fit_ok" == 'yes' ]
		then
			# do this fit
			date
			echo "$cmd"
			if $cmd
			then
				echo "$cmd: OK"
			else
				echo "$cmd: Error"
				fit_ok='no'
			fi
		fi
		if [ "$fit_ok" == 'yes' ]
		then
			# statistics for this fit
			cmd="$base_cmd sample $number_sample"
			if $cmd
			then
				echo "$cmd: OK"
			else
				echo "$cmd: Error"
			fi
		fi
	done
done
#
echo 'fit_ihme_all.sh: OK'
exit 0
