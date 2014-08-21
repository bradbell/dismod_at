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
if [ "$1" == '' ]
then
	echo 'usage: bin/search.sh grep_string'
	exit 1
fi
grep -r -l "$1" . | sed \
	-e '/\/build\//d' \
	-e '/\/new\//d' \
	-e '/\/doc\//d' \
	-e '/\/\.svn\//d' \
	-e '/junk\./d' \
	-e '/\.pyc$/d' \
