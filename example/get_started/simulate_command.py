# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-16 University of Washington
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
# $srcfile%
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
# create the var table
program        = '../../devel/dismod_at'
command        = 'init'
cmd = [ program, file_name, command ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at init command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# get the variable information
var_table  = dismod_at.get_table_dict(connection, 'var')
# -----------------------------------------------------------------------
# create a truth_var table with variables values to use during simulation
tbl_name = 'truth_var'
col_name = [ 'truth_var_value' ]
col_type = [ 'real'        ]
row_list = list()
for var_id in range( len(var_table) ) :
	variable_row  = var_table[var_id]
	var_type = variable_row['var_type']
	if var_type in [ 'mulstd_value', 'mulstd_dage', 'mulstd_dtime' ] :
		assert False
	elif var_type == 'rate' :
		rate_id   = variable_row['rate_id']
		truth_var_value = 5e-3 * (rate_id + 1)
	else :
		assert False
	truth_row = [ truth_var_value ]
	row_list.append( truth_row )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
# -----------------------------------------------------------------------
# simulate command
program        = '../../devel/dismod_at'
command        = 'simulate'
cmd = [ program, file_name, command ]
if command == 'simulate' :
	number_simulate = '1'
	cmd.append(number_simulate)
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at simulate command failed')
# -----------------------------------------------------------------------
# check the simulate table
data_table      = dismod_at.get_table_dict(connection, 'data')
data_subset_dict = dismod_at.get_table_dict(connection, 'data_subset')
simulate_dict    = dismod_at.get_table_dict(connection, 'simulate')
for simulate_id in range( len(simulate_dict) ) :
	row = simulate_dict[simulate_id];
	simulate_index  = row['simulate_index']
	data_subset_id  = row['data_subset_id']
	data_id         = data_subset_dict[data_subset_id]['data_id']
	meas_value      = row['meas_value']
	meas_std        = data_table[data_id]['meas_std']
	if data_id == len(data_table) - 1 :
		rate_id = 0       # in original data set this is outlyer at then
	else :
		rate_id = data_id # for this example case
	truth_var_value  = 5e-3 * (rate_id + 1)
	assert simulate_index == 0
	assert meas_value != data_table[data_id]['meas_value']
	assert meas_value != truth_var_value
	assert abs( meas_value - truth_var_value ) < 4.0 * meas_std
print('simulate_command: OK')
# END PYTHON
