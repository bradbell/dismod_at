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
import csv
sys.path.append( os.path.join( os.getcwd(), 'python' ) )
import dismod_at
# ---------------------------------------------------------------------------
# cascade_dir:       directory name (not path) where cascade input files are
# cascade_path_dict: path to cascade input files
#
if sys.argv[0] != 'bin/import_cascade.py' :
	msg  = 'bin/import_cascasde.py: must be executed from its parent directory'
	sys.exit(msg)
#
usage = 'bin/import_cascade.py cascade_path'
n_arg = len(sys.argv)
if n_arg != 2 :
	sys.exit(usage)
#
cascade_path = sys.argv[1]
if not os.path.isdir( cascade_path ) :
	msg  = usage + '\n'
	msg += 'import_cascade: ' + cascade_path + ' is not a directory'
	sys.exit(msg)
#
cascade_dir       = os.path.basename(cascade_path)
#
cascade_name_list = [
	'data', 'rate_prior', 'simple_prior', 'effect_prior', 'integrand', 'value'
]
cascade_path_dict = dict()
for name in cascade_name_list :
	path = os.path.join(cascade_path, name + '.csv')
	if not os.path.isfile(path) :
		msg = 'import_cascade: ' + path + ' is not a file'
		sys.exit(msg)
	cascade_path_dict[name] = path
# ---------------------------------------------------------------------------
# Create database_connection (database that is output by this program)
if not os.path.isdir('build') :
	print('mkdir build')
	os.mkdir('build')
new = True
file_name        = os.path.join('build', cascade_dir + '.db')
db_connection    = dismod_at.create_connection(file_name, new)
# ---------------------------------------------------------------------------
# Create cascade_data_dict (data for each cascade input file)
#
cascade_data_dict = dict()
for name in cascade_path_dict :
	path      = cascade_path_dict[name]
	file_ptr  = open(path)
	reader    = csv.DictReader(file_ptr)
	#
	cascade_data_dict[name] = list()
	for row in reader :
		cascade_data_dict[name].append(row)
# ---------------------------------------------------------------------------
print('import_cascade.py: OK')
