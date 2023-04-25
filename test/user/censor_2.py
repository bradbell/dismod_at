# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# BEGIN PYTHON
# begin problem parameters
iota_true     = 0.05      # value used to simulate data
n_data        = 500       # number of simulated data points
random_seed   = 0         # if zero, seed off the clock
density_name  = 'cen_log_laplace'  # can be any of the censored distributions
# end problem parameters
# ------------------------------------------------------------------------
import time
#
import sys
import os
import subprocess
import copy
import numpy
import scipy.stats
import math
test_program = 'test/user/censor_2.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
   usage  = 'python3 ' + test_program + '\n'
   usage += 'where python3 is the python 3 program on your system\n'
   usage += 'and working directory is the dismod_at distribution directory\n'
   sys.exit(usage)
print(test_program)
#
# import dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
   sys.path.insert(0, local_dir)
import dismod_at
#
# change into the build/test/user directory
if not os.path.exists('build/test/user') :
   os.makedirs('build/test/user')
os.chdir('build/test/user')
# ----------------------------------------------------------------------------
if random_seed == 0 :
   random_seed = int( time.time() )
numpy.random.seed(random_seed)
# ----------------------------------------------------------------------------
# run a system command
def system_command(command) :
   print( ' '.join(command) )
   flag = subprocess.call( command )
   if flag != 0 :
      sys.exit('command failed: flag = ' + str(flag))
   return
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def example_db (file_name) :
   def constant_weight_fun(a, t) :
      return 1.0
   def fun_iota_parent(a, t) :
      return ('prior_iota_parent', None, None)
   # ----------------------------------------------------------------------
   # age table
   age_list    = [    0.0, 50.0,    100.0 ]
   #
   # time table
   time_list   = [ 1995.0, 2005.0, 2015.0 ]
   #
   # integrand table
   integrand_table = [
      { 'name':'Sincidence' }
   ]
   #
   # node table: world -> north_america
   #             north_america -> (united_states, canada)
   node_table = [
      { 'name':'world',         'parent':'' },
   ]
   #
   # weight table: The constant function 1.0 (one age and one time point)
   fun = constant_weight_fun
   weight_table = [
      { 'name':'constant',  'age_id':[1], 'time_id':[1], 'fun':fun }
   ]
   #
   # covariate table:
   covariate_table = list()
   #
   # mulcov table
   mulcov_table = list()
   #
   # avgint table: empty
   avgint_table = list()
   #
   # nslist_dict:
   nslist_dict = dict()
   # ----------------------------------------------------------------------
   # data table:
   data_table = list()
   # Values that are the same for all data rows
   # (because the simultion censors the data)
   meas_std   = iota_true;
   eta        = iota_true / 2.0 # large so get zeros for log scaled cases
   row = {
      'node':        'world',
      'subgroup':    'world',
      'integrand':   'Sincidence',
      'density':     density_name,
      'weight':      'constant',
      'hold_out':     False,
      'age_lower':    0.0,
      'age_upper':    0.0,
      'time_lower':   1995.0,
      'time_upper':   1995.0,
      'eta':          eta
   }
   sigma = meas_std
   mu    = iota_true
   if density_name in [ 'cen_log_gaussian', 'cen_log_laplace' ] :
      sigma  = math.log(iota_true + eta + meas_std)
      sigma -= math.log(iota_true + eta)
      mu     = math.log(iota_true + eta )
   scale = sigma / math.sqrt(2.0)
   for data_id in range( n_data ) :
      # simulate the data for distribution without censoring
      if density_name in [ 'cen_laplace', 'cen_log_laplace' ] :
         meas_value  = numpy.random.laplace(mu, scale)
      else :
         meas_value  = numpy.random.normal(mu, sigma)
      # check for log transform case
      if density_name in [ 'cen_log_gaussian', 'cen_log_laplace' ] :
         meas_value = math.exp(meas_value) - eta
      # censor the data; i.e., replace negative values by zero values.
      meas_value        = max(meas_value, 0.0)
      row['meas_value'] = meas_value
      #
      delta    = meas_std
      if density_name in [ 'cen_log_gaussian', 'cen_log_laplace' ] :
         # sigma = log(meas_value + eta + delta) - log(meas_value + eta)
         delta = (math.exp(sigma) - 1.0) * (meas_value + eta)
      row['meas_std'] = delta
      #
      data_table.append( copy.copy(row) )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_iota_parent
         'name':     'prior_iota_parent',
         'density':  'uniform',
         'lower':    0.001,
         'upper':    1.00,
         'mean':     0.1,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   smooth_table = [
      { # smooth_iota_parent
         'name':                     'smooth_iota_parent',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0 ],
         'fun':                      fun_iota_parent
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [
      {
         'name':          'iota',
         'parent_smooth': 'smooth_iota_parent',
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'random_seed',            'value':str(random_seed)    },
      { 'name':'parent_node_name',       'value':'world'             },
      { 'name':'rate_case',              'value':'iota_pos_rho_zero' },

      { 'name':'quasi_fixed',            'value':'false'        },
      { 'name':'max_num_iter_fixed',     'value':'100'          },
      { 'name':'print_level_fixed',      'value':'0'            },
      { 'name':'tolerance_fixed',        'value':'1e-13'        },
   ]
   # ----------------------------------------------------------------------
   # subgroup_table
   subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
   # ----------------------------------------------------------------------
   # create database
   dismod_at.create_database(
      file_name,
      age_list,
      time_list,
      integrand_table,
      node_table,
      subgroup_table,
      weight_table,
      covariate_table,
      avgint_table,
      data_table,
      prior_table,
      smooth_table,
      nslist_dict,
      rate_table,
      mulcov_table,
      option_table
   )
   # ----------------------------------------------------------------------
   return
# ----------------------------------------------------------------------------
# Fit to estimate iota from censored data
# ----------------------------------------------------------------------------
#
file_name = 'example.db'
example_db(file_name)
#
# init and fit fixed
program = '../../devel/dismod_at'
system_command([ program, file_name, 'init' ])
system_command([ program, file_name, 'fit', 'fixed' ])
#
# connect to database
connection      = dismod_at.create_connection(
   file_name, new = False, readonly = False
)
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
# check result of the fit fixed
assert len(var_table) == 1
first_estimate = fit_var_table[0]['fit_var_value']
if abs( 1.0 - first_estimate / iota_true ) > 1e-1 :
   print("random_seed = ", random_seed)
   print("first_estimate = ", first_estimate)
   print(1.0 - first_estimate / iota_true)
   print(first_estimate, iota_true)
   assert False
# -----------------------------------------------------------------------------
# Simulate a censored data set and fit it
# ----------------------------------------------------------------------------
# set truth_var table to contain iota_true
tbl_name = 'truth_var'
col_name = [ 'truth_var_value' ]
col_type = [ 'real' ]
row_list = [ [iota_true] ]
dismod_at.create_table(connection, tbl_name, col_name, col_type, row_list)
#
# simulate a new data set and sample using it
system_command([ program, file_name, 'set', 'avgint', 'data' ])
system_command([ program, file_name, 'simulate', '1' ])
system_command([ program, file_name, 'fit', 'fixed', '0' ])
#
# check result of the second fit fixed
connection       = dismod_at.create_connection(
   file_name, new = False, readonly = False
)
fit_var_table    = dismod_at.get_table_dict(connection, 'fit_var')
second_estimate  = fit_var_table[0]['fit_var_value']
if abs( 1.0 - second_estimate / iota_true ) > 1e-1 :
   print("random_seed = ", random_seed)
   print("second_estimate = ", second_estimate)
   print(1.0 - second_estimate / iota_true)
   assert False
#
# check that the estimates were different; i,e., used the second data set
if abs(first_estimate - second_estimate) / iota_true < 5e-3 :
   print("random_seed = ", random_seed)
   print( (first_estimate - second_estimate) / iota_true )
   assert False
#
# check all the simulated data values were non-negative
data_sim_table = dismod_at.get_table_dict(connection, 'data_sim')
## for row in data_sim_table :
   ## assert row['data_sim_value'] >= 0.0
# -----------------------------------------------------------------------------
print('censor_2.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
