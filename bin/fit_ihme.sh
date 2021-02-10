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
if [ "$1" == '' ]
then
	echo 'usage: bin/fit_ihme.sh disease'
	echo 'were disease is "all" or in the following list:'
	echo $disease_list
	exit 1
fi
if [ "$1" != '' ]
then
	disease_list="$1"
fi
number_sample='20'
random_seed='0'
for disease in $disease_list
do
	for which_fit in no_ode yes_ode students
	do
		if [ "$which_fit" == 'no_ode' ]
		then
			echo_eval bin/fit_ihme.py ihme_db $disease $which_fit $random_seed
		else
			echo_eval bin/fit_ihme.py ihme_db $disease $which_fit
		fi
			echo_eval bin/fit_ihme.py ihme_db $disease $which_fit sample $number_sample
	done
done
#
echo 'fit_ihme_all.sh: OK'
exit 0
