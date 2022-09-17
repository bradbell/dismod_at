# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin sample_command.py$$ $newlinech #$$
# $spell
#	dismod
# $$
#
# $section sample Command: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
import subprocess
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
if not os.path.exists('build/example/get_started') :
    os.makedirs('build/example/get_started')
os.chdir('build/example/get_started')
# ---------------------------------------------------------------------------
# create get_started.db
get_started_db.get_started_db()
# -----------------------------------------------------------------------
# create the var table
program        = '../../devel/dismod_at'
file_name      = 'get_started.db'
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
tbl_name          = 'truth_var'
col_name          = [ 'truth_var_value' ]
col_type          = [ 'real'        ]
row_list          = list()
omega_world       = 2e-2
income_multiplier = -1e-3
for var_id in range( len(var_table) ) :
	var_row  = var_table[var_id]
	var_type = var_row['var_type']
	if var_type == 'mulcov_rate_value' :
		truth_var_value = income_multiplier
	elif var_type == 'rate' :
		truth_var_value = omega_world
	else :
		assert False
	truth_row = [ truth_var_value ]
	row_list.append( truth_row )
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
# -----------------------------------------------------------------------
# simulate command
program         = '../../devel/dismod_at'
command         = 'simulate'
number_simulate = '1'
cmd = [ program, file_name, command , number_simulate ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at simulate command failed')

# -----------------------------------------------------------------------
# sample command
option_table  = dismod_at.get_table_dict(connection, 'option')
program       = '../../devel/dismod_at'
command       = 'sample'
method        = 'simulate'
variables     = 'both'
number_sample = '1'
cmd = [ program, file_name, command, method , variables, number_sample  ]
print( ' '.join(cmd) )
flag = subprocess.call( cmd )
if flag != 0 :
	sys.exit('The dismod_at sample command failed')
# -----------------------------------------------------------------------
# check the sample table
var_table    = dismod_at.get_table_dict(connection, 'var')
sample_table = dismod_at.get_table_dict(connection, 'sample')
#
assert int(number_sample) == 1
assert len(sample_table) == len(var_table)
for var_id in range( len(var_table) ) :
	var_row    = var_table[var_id]
	sample_row = sample_table[var_id]
	var_type   = var_row['var_type']
	var_value  = sample_row['var_value']
	if var_type == 'mulcov_rate_value' :
		assert var_value == income_multiplier
	elif var_type == 'rate' :
		assert abs( var_value / omega_world  - 1.0 ) < 0.5
#
# -----------------------------------------------------------------------
print('sample_command: OK')
# END PYTHON
