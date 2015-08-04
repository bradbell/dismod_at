#! /bin/python3
# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin import_cascade$$ $newlinech #$$
#
# $section Import an IHME Cascade Study$$
#
# $head Syntax$$
# $codei%import_cascade.py %dir%$$
#
# $end
# ---------------------------------------------------------------------------
import sys
import os
# ---------------------------------------------------------------------------
if sys.argv[0] != 'bin/import_cascade.py' :
	msg  = 'bin/import_cascasde.py: must be executed from its parent directory'
	sys.exit(msg)
#
usage = 'bin/import_cascade.py cascade_dir'
n_arg = len(sys.argv)
if n_arg != 2 :
	sys.exit(usage)
#
cascade_dir = sys.argv[1]
if not os.path.isdir( cascade_dir ) :
	msg  = usage + '\n'
	msg += 'import_cascade: ' + cascade_dir + ' is not a directory'
	sys.exit(msg)
#
cascade_file_list = [
	'data.csv',
	'rate_prior.csv',
	'simple_prior.csv',
	'effect_prior.csv',
	'integrand.csv',
	'value.csv'
]
cascade_path_list = list()
for name in cascade_file_list :
	path = os.path.join(cascade_dir, name)
	if not os.path.isfile(path) :
		msg = 'import_cascade: ' + path + ' is not a file'
		sys.exit(msg)
	cascade_path_list.append(path)
# ---------------------------------------------------------------------------
print('import_cascade.py: OK')
