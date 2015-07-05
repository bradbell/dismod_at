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
# $begin fit_command.py$$ $newlinech #$$
# $spell
#	dismod
# $$
#
# $section dismod_at fit: Example and Test$$
#
# $code
# $verbatim%
#	example/get_started/fit_command.py
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
example = 'example/get_started/fit_command.py'
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
command        = 'fit'
cmd  = [ program, command, file_name ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at fit command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# check variable table
variable_dict  = dismod_at.get_table_dict(connection, 'variable')
#
# mulstd variables
for smooth_id in range( n_smooth ) :
	for variable_type in [ 'mulstd_value', 'mulstd_dage', 'mulstd_dtime' ] :
		count = 0
		for row in variable_dict :
			match = row['variable_type'] == variable_type
			match = match and row['smooth_id'] == smooth_id
			if match :
				count += 1
				assert row['variable_value'] == 1.0
		assert count == 1
#
# rate variables
parent_node_id = 0
child_node_id  = 1
check_tol      = 1e-3
n_rate         = 5;
for rate_id in range(n_rate) :
	for node_id in [ parent_node_id, child_node_id ] :
		count = 0
		for row in variable_dict :
			match = row['variable_type'] == 'rate'
			match = match and row['rate_id'] == rate_id
			match = match and row['node_id'] == node_id
			if match :
				count += 1
				check  = rate_true[rate_id]
				if node_id == 0 :
					# parent node
					err = row['variable_value'] / check - 1.0
				else :
					# child node
					err = row['variable_value'] / check
				assert abs(err) <= check_tol
		# number of point in smoothing for all rates
		assert count == 2
# -----------------------------------------------------------------------
print('fit_command: OK')
# END PYTHON
