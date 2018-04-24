# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-18 University of Washington
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
# $section simulate Command: Example and Test$$
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
from math import exp
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
# check the data_sim table
data_table      = dismod_at.get_table_dict(connection, 'data')
data_sim_table  = dismod_at.get_table_dict(connection, 'data_sim')
assert len(data_table) == 1
assert len(data_sim_table) == 1
data_row = data_table[0]
sim_row  = data_sim_table[0]
#
# There is only on simulation so
assert sim_row['simulate_index'] == 0
# For this case, data_subset_id is same as data_id
assert sim_row['data_subset_id'] == 0
# There are no measurement standard deviation covariates, so
assert abs( sim_row['data_sim_delta'] / data_row['meas_std'] - 1.0 ) < 1e-7
# Compute the model value for the measurement (with no noise)
# Note that age_lower is equal age_upper
age            = data_row['age_lower']
income         = data_row['x_0']
adjusted_omega = omega_world * exp(income_multiplier * income)
model_value    = exp( - adjusted_omega * age )
# check if model value is within 3 standard deviations of the simulated value
data_sim_value = sim_row['data_sim_value']
data_sim_delta = sim_row['data_sim_delta']
assert abs( model_value - data_sim_value ) < 3.0 * data_sim_delta
assert abs( model_value - data_sim_value ) > data_sim_delta * 1e-3
# -----------------------------------------------------------------------
print('simulate_command: OK')
# END PYTHON
