# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin start_command.py$$ $newlinech #$$
# $spell
#	dismod
# $$
#
# $section dismod_at start: Example and Test$$
#
# $code
# $verbatim%
#	example/get_started/start_command.py
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
# ---------------------------------------------------------------------------
# check execution is from distribution directory
example = 'example/get_started/start_command.py'
if sys.argv[0] != example  or len(sys.argv) != 1 :
	usage  = 'python3 ' + example + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
#
# import dismod_at
sys.path.append( os.getcwd() + '/python' )
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
# create input tables
file_name              = 'example.db'
(n_smooth, rate_true)  = get_started_db.get_started_db(file_name)
# -----------------------------------------------------------------------
program        = '../../devel/dismod_at'
for command in [ 'init', 'start' ] :
	cmd  = [ program, command, file_name ]
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# get variable and start_var tables
var_dict       = dismod_at.get_table_dict(connection, 'var')
start_var_dict = dismod_at.get_table_dict(connection, 'start_var')
#
# mulstd variables
for smooth_id in range( n_smooth ) :
	for var_type in [ 'mulstd_value', 'mulstd_dage', 'mulstd_dtime' ] :
		count = 0
		for var_id in range( len(var_dict) ) :
			row   = var_dict[var_id]
			match = row['var_type'] == var_type
			match = match and row['smooth_id'] == smooth_id
			if match :
				count += 1
				variable_value = start_var_dict[var_id]['start_var_value']
				assert variable_value == 1.0
		assert count == 0
#
# rate variables are equal to their prior means
parent_node_id = 0
child_node_id  = 1
n_rate         = 5
for rate_id in range(n_rate) :
	for node_id in [ parent_node_id, child_node_id ] :
		count = 0
		for var_id in range( len(var_dict) ) :
			row   = var_dict[var_id]
			match = row['var_type'] == 'rate'
			match = match and row['rate_id'] == rate_id
			match = match and row['node_id'] == node_id
			if match :
				count += 1
				start_var_value  = start_var_dict[var_id]['start_var_value']
				if node_id == parent_node_id :
					assert start_var_value == 1e-1
				else :
					assert start_var_value == 0.0
		# number of point in smoothing for all rates
		assert count == 2
# -----------------------------------------------------------------------
print('start_command: OK')
# END PYTHON
