# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin set_command.py$$ $newlinech #$$
# $spell
#	var
# $$
#
# $section set Command: Example and Test$$
#
# $head truth_var Table$$
# The $cref set_command$$ in this example creates a
# truth_var table.
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
# ---------------------------------------------------------------------------
# check execution is from distribution directory
example = 'example/get_started/set_command.py'
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
program        = '../../devel/dismod_at'
file_name      = 'get_started.db'
dismod_at.system_command_prc( [program, file_name, 'init'] )
dismod_at.system_command_prc( [program, file_name, 'fit', 'both'] )
# -----------------------------------------------------------------------
# connect to database
new        = False
connection = dismod_at.create_connection(file_name, new)
var_table     = dismod_at.get_table_dict(connection, 'var')
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
connection.close()
#
# check the fit
for var_id in range( len(var_table) ) :
	var_row     = var_table[var_id]
	fit_row     = fit_var_table[var_id]
	var_type    = var_row['var_type']
	if var_type == 'mulcov_rate_value' :
		income_multiplier = fit_row['fit_var_value']
		assert income_multiplier == -1e-3
	elif var_type == 'rate' :
		omega_world = fit_row['fit_var_value']
		assert abs( omega_world / 1e-2 - 1.0 ) < 1e-6
	else :
		assert False
#
# set truth_var = fit_var, start_var = truth_var, scale_var = start_var
system_cmd_list = [
	[ program, file_name, 'set', 'truth_var', 'fit_var' ],
	[ program, file_name, 'set', 'start_var', 'truth_var' ],
	[ program, file_name, 'set', 'scale_var', 'start_var' ]
]
for system_cmd in system_cmd_list :
	dismod_at.system_command_prc( system_cmd )
#
new             = False
connection      = dismod_at.create_connection(file_name, new)
truth_var_table = dismod_at.get_table_dict(connection, 'truth_var')
start_var_table = dismod_at.get_table_dict(connection, 'start_var')
scale_var_table = dismod_at.get_table_dict(connection, 'scale_var')
for var_id in range( len(var_table) ) :
	#
	fit_row     = fit_var_table[var_id]
	check       = fit_row['fit_var_value']
	#
	truth_row   = truth_var_table[var_id]
	value       = truth_row['truth_var_value']
	assert abs( value / check - 1.0 ) < 1e-6
	#
	start_row   = start_var_table[var_id]
	value       = start_row['start_var_value']
	assert abs( value / check - 1.0 ) < 1e-6
	#
	scale_row   = scale_var_table[var_id]
	value       = scale_row['scale_var_value']
	assert abs( value / check - 1.0 ) < 1e-6
#
# -----------------------------------------------------------------------
print('set_command: OK')
# END PYTHON
