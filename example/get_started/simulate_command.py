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
# $begin simulate_command.py$$ $newlinech #$$
# $spell
#	dismod
# $$
#
# $section dismod_at simulate: Example and Test$$
#
# $code
# $verbatim%
#	example/get_started/simulate_command.py
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
example = 'example/get_started/simulate_command.py'
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
# create the variable table
program        = '../../devel/dismod_at'
command        = 'variable'
cmd  = [ program, command, file_name ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at variable command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# get the variable information
variable_dict  = dismod_at.get_table_dict(connection, 'variable')
# -----------------------------------------------------------------------
# create a truth table with variables values to use during simulation
tbl_name = 'truth'
col_name = [ 'variable_id', 'truth_value' ]
col_type = [ 'integer',     'real'      ]
row_list = list()
for variable_id in range( len(variable_dict) ) :
	variable_row  = variable_dict[variable_id]
	variable_type = variable_row['variable_type']
	if variable_type in [ 'mulstd_value', 'mulstd_dage', 'mulstd_dtime' ] :
		truth_value = 1.0
	elif variable_type == 'rate' :
		rate_id   = variable_row['rate_id']
		truth_value = 5e-3 * (rate_id + 1)
	else :
		assert False
	truth_row = [ variable_id, truth_value ]
	row_list.append( truth_row )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
# -----------------------------------------------------------------------
# simulate command
program        = '../../devel/dismod_at'
command        = 'simulate'
cmd  = [ program, command, file_name ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at simulate command failed')
# -----------------------------------------------------------------------
# check the sim_meas table
data_dict      = dismod_at.get_table_dict(connection, 'data')
sim_meas_dict  = dismod_at.get_table_dict(connection, 'sim_meas')
for sim_meas_id in range( len(sim_meas_dict) ) :
	row = sim_meas_dict[sim_meas_id];
	data_id    = row['data_id']
	meas_value = row['meas_value']
	meas_std   = data_dict[data_id]['meas_std']
	rate_id      = data_id # for this example database
	truth_value  = 5e-3 * (rate_id + 1)
	assert meas_value != data_dict[data_id]['meas_value']
	assert meas_value != truth_value
	assert abs( meas_value - truth_value ) < 2.0 * meas_std
print('simulate_command: OK')
# END PYTHON
