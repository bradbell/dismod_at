# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-14 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
# 	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
if [ "$0" != 'bin/table_files.sh' ]
then
	echo 'bin/table_files.sh must be executed from its parent directory'
	exit 1
fi
if [ "$1" == '' ]
then
	echo 'usage: bin/table_files.sh table_name'
	echo 'lists the files that change when a table changes.'
	exit 1
fi
name="$1"
cat << EOF
omh/table/${name}_table.omh:              documentation for this table.
dismod_at/include/get_${name}_table.hpp:  C++ struct for this table.
devel/get_${name}_table.cpp:              read this table in C++
dismod_at/database.py:                    create_database all tables.
example/table/${name}_table.py:           example for this table
example/get_started/get_started.py.in:    calls create_database.
example/devel/get_${name}_table_xam.cpp:  C++ example.
EOF
