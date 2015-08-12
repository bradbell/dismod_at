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
# $begin predict_command.py$$ $newlinech #$$
# $spell
#	dismod
# $$
#
# $section dismod_at predict: Example and Test$$
#
# $code
# $verbatim%
#	example/get_started/predict_command.py
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
example = 'example/get_started/predict_command.py'
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
cmd  = [ program, command, file_name ]
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
var_dict  = dismod_at.get_table_dict(connection, 'var')
# -----------------------------------------------------------------------
# create a truth_var table with variables values to use during simulation
tbl_name = 'truth_var'
col_name = [ 'truth_var_value' ]
col_type = [ 'real'        ]
row_list = list()
for var_id in range( len(var_dict) ) :
	variable_row  = var_dict[var_id]
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
# simulate, sample, and then predict
program        = '../../devel/dismod_at'
for command in [ 'simulate', 'sample', 'predict' ] :
	cmd  = [ program, command, file_name ]
	print( ' '.join(cmd) )
	flag = subprocess.call( cmd )
	if flag != 0 :
		sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# check the predict table
var_dict     = dismod_at.get_table_dict(connection, 'var')
sample_dict  = dismod_at.get_table_dict(connection, 'sample')
predict_dict = dismod_at.get_table_dict(connection, 'predict')
#
# rate variables
parent_node_id = 0
child_node_id  = 1
check_tol      = 1e-2
n_rate         = 5;
check          = list()
for rate_id in range(n_rate) :
	integrand_id = rate_id
	count        = 0
	for var_id in range( len(var_dict) ) :
		row   = var_dict[var_id]
		match = row['var_type'] == 'rate'
		match = match and row['rate_id'] == rate_id
		match = match and row['node_id'] == parent_node_id
		if match :
			count         += 1
			var_value      = sample_dict[var_id]['var_value']
			avg_integrand  = predict_dict[integrand_id]['avg_integrand']
			err            = avg_integrand / var_value - 1.0
			assert abs(err) <= check_tol
	# two time points, one age point, for each rate
	assert count == 2

print('predict_command: OK')
# END PYTHON
