# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# $begin simulate_command.py$$ $newlinech #$$
# $spell
#  dismod
# $$
#
# $nospell
$bold This is old dismod documentation:$$ Here is a link to its
$href%http://bradbell.github.io/dismod_at%current documentation%$$.
$$
$section simulate Command: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
import numpy
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
dismod_at.system_command_prc( [program, file_name, 'init'] )
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
number_simulate = 200
dismod_at.system_command_prc(
   [program, file_name, command , str(number_simulate) ]
)
# -----------------------------------------------------------------------
# check the data_sim table
data_table        = dismod_at.get_table_dict(connection, 'data')
data_subset_table = dismod_at.get_table_dict(connection, 'data_subset')
data_sim_table    = dismod_at.get_table_dict(connection, 'data_sim')
assert len(data_table) == 1
assert len(data_sim_table) == number_simulate
#
residual_list = list()
for row in data_sim_table :
   data_sim_value = row['data_sim_value']
   data_subset_id = row['data_subset_id']
   #
   data_id        = data_subset_table[data_subset_id]['data_id']
   meas_std       = data_table[data_id]['meas_std']
   income         = data_table[data_id]['x_0']
   age_lower      = data_table[data_id]['age_lower']
   age_upper      = data_table[data_id]['age_upper']
   assert age_lower == age_upper
   #
   adjusted_omega = omega_world * exp( income_multiplier * income )
   meas_model     = exp( - adjusted_omega * age_lower )
   residual       = (data_sim_value - meas_model) / meas_std
   residual_list.append( residual )
residual_array  = numpy.array( residual_list )
residual_mean   = residual_array.mean()
residual_std    = residual_array.std(ddof=1)
# check that the mean of the residuals is within 2.5 standard deviations
assert abs(residual_mean) <=  2.5 / numpy.sqrt(number_simulate)
# check that the standard deviation of the residuals is near one
assert abs(residual_std - 1.0) <= 2.5 / numpy.sqrt(number_simulate)
# -----------------------------------------------------------------------
print('simulate_command: OK')
# END PYTHON
