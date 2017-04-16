# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin truth_command.py$$ $newlinech #$$
# $spell
#	dismod
# $$
#
# $section dismod_at truth: Example and Test$$
#
# $code
# $srcfile%
#	example/get_started/truth_command.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
import subprocess
import distutils.dir_util
from math import exp
# ---------------------------------------------------------------------------
# check execution is from distribution directory
example = 'example/get_started/truth_command.py'
if sys.argv[0] != example  or len(sys.argv) != 1 :
	usage  = 'python3 ' + example + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
#
# import dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
	sys.path.insert(0, local_dir)
import dismod_at
#
# import get_started_db example
sys.path.append( os.getcwd() + '/example/get_started' )
import get_started_db
#
# change into the build/example/get_started directory
distutils.dir_util.mkpath('build/example/get_started')
os.chdir('build/example/get_started')
# ---------------------------------------------------------------------------
# create get_started.db
get_started_db.get_started_db()
# -----------------------------------------------------------------------
program        = '../../devel/dismod_at'
file_name      = 'get_started.db'
for command in [ 'init', 'start', 'fit', 'truth' ] :
	cmd = [ program, file_name, command ]
	if command == 'start' :
		cmd.append('prior_mean')
	if command == 'fit' :
		variables = 'both'
		cmd.append(variables)
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# connect to database
new        = False
connection = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# get variable and predict tables
fit_var_table  = dismod_at.get_table_dict(connection, 'fit_var')
truth_table    = dismod_at.get_table_dict(connection, 'truth_var')
assert len(fit_var_table) == len(truth_table)
#
for var_id in range( len(fit_var_table) ) :
	fit_row     = fit_var_table[var_id]
	truth_row   = truth_table[var_id]
	fit_value   = fit_row['variable_value']
	truth_value = truth_row['truth_var_value']
	assert abs( fit_value / truth_value - 1.0 ) < 1e-10
# -----------------------------------------------------------------------
print('truth_command: OK')
# END PYTHON
