#! /usr/bin/env python3
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-16 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
import sys
import os
#
sys.path.append( os.path.join( os.getcwd(), 'python' ) )
import dismod_at
# ---------------------------------------------------------------------------
usage  = 'dismodat.py database command'
usage += '\nwhere db2csv is the only command so far'
if len(sys.argv) != 3 :
	sys.exit(usage)
if sys.argv[2] != 'db2csv' :
	sys.exit(usage)
#
database_file_arg = sys.argv[1]
dismod_at.db2csv_command(database_file_arg)
print('dismodat.py: OK')
