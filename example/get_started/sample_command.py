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
# $begin sample_command.py$$ $newlinech #$$
# $spell
#	dismod
# $$
#
# $section dismod_at sample: Example and Test$$
#
# $code
# $srcfile%
#	example/get_started/sample_command.py
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
example = 'example/get_started/sample_command.py'
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
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at simulate command failed')
# -----------------------------------------------------------------------
# sample command
program        = '../../devel/dismod_at'
command        = 'sample'
method         = 'simulate'
cmd = [ program, file_name, command, method ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at sample command failed')
# -----------------------------------------------------------------------
# check the sample table
var_table       = dismod_at.get_table_dict(connection, 'var')
simulate_dict  = dismod_at.get_table_dict(connection, 'simulate')
sample_dict    = dismod_at.get_table_dict(connection, 'sample')
#
# rate variables
parent_node_id    = 0
child_node_id     = 1
check_tol         = 1e-5
n_rate            = 5;
parent_rate_lower = 1e-4;
for rate_id in range(n_rate) :
	for node_id in [ parent_node_id, child_node_id ] :
		count = 0
		for var_id in range( len(var_table) ) :
			row   = var_table[var_id]
			match = row['var_type'] == 'rate'
			match = match and row['rate_id'] == rate_id
			match = match and row['node_id'] == node_id
			if match :
				count += 1
				sample_id      = rate_id
				check          = simulate_dict[sample_id]['meas_value']
				check          = max(check, parent_rate_lower)
				variable_value = sample_dict[var_id]['var_value']
				if node_id == 0 :
					# parent node
					err = variable_value / check - 1.0
				else :
					# child node
					err = variable_value / check
				assert abs(err) <= check_tol
		# number of point in smoothing for all rates
		assert count == 2
print('sample_command: OK')
# END PYTHON
