# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# Test a case where there is not data and fit does not agree with mean.
# ------------------------------------------------------------------------
iota_mean     = 0.01
# ------------------------------------------------------------------------
import sys
import os
import subprocess
test_program = 'test/user/no_data.py'
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
# ------------------------------------------------------------------------
def constant_weight_fun(a, t) :
   return 1.0
# note that the a, t values are not used for this case
def fun_iota_parent(a, t) :
   return ('prior_value', 'prior_diff', 'prior_diff')
# ------------------------------------------------------------------------
def example_db (file_name) :
   import copy
   import dismod_at
   import math
   # ----------------------------------------------------------------------
   # age table
   age_list    = [0, 1, 20, 30, 40, 50, 60, 70, 80, 90, 100 ]
   #
   # time table
   time_list   = [ 1960, 1995 ]
   #
   # integrand table
   integrand_table = [
      { 'name':'Sincidence', 'minimum_meas_cv':0.0 }
   ]
   # node table
   node_table = [
      { 'name':'world',         'parent':'' }
   ]
   #
   # weight table: The constant function 1.0 (one age and one time point)
   fun = constant_weight_fun
   weight_table = [
      { 'name':'constant',  'age_id':[0], 'time_id':[0], 'fun':fun }
   ]
   #
   # covariate table:
   covariate_table = list()
   #
   # mulcov table
   mulcov_table = list()
   # ----------------------------------------------------------------------
   # data table:
   data_table = list()
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_diff
         'name':     'prior_diff',
         'density':  'log_gaussian',
         'lower':    None,
         'upper':    None,
         'mean':     0.0,
         'std':      1.0,
         'eta':      1e-6
      },{ # prior_value
         'name':     'prior_value',
         'density':  'log_gaussian',
         'lower':    1e-2 * iota_mean,
         'upper':    1e+2 * iota_mean,
         'mean':     iota_mean,
         'std':      iota_mean,
         'eta':      1e-3 * iota_mean
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   age_id_list = range( len( age_list ) )
   time_id_list = range( len( time_list ) )
   smooth_table = [
      { # smooth_rate_parent
         'name':                     'smooth_rate_parent',
         'age_id':                   age_id_list,
         'time_id':                  time_id_list,
         'mulstd_value_prior_name':  None,
         'mulstd_dage_prior_name':   None,
         'mulstd_dtime_prior_name':  None,
         'fun':                      fun_iota_parent
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [ {
         'name':          'iota',
         'parent_smooth': 'smooth_rate_parent'
   } ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'parent_node_name',       'value':'world'             },
      { 'name':'ode_step_size',          'value':'10.0'              },
      { 'name':'random_seed',            'value':'0'                 },
      { 'name':'rate_case',              'value':'iota_pos_rho_zero' },

      { 'name':'quasi_fixed',            'value':'true'              },
      { 'name':'derivative_test_fixed',  'value':'none'              },
      { 'name':'max_num_iter_fixed',     'value':'50'                },
      { 'name':'print_level_fixed',      'value':'0'                 },
      { 'name':'tolerance_fixed',        'value':'1e-10'             },

      { 'name':'derivative_test_random', 'value':'second-order'      },
      { 'name':'max_num_iter_random',    'value':'50'                },
      { 'name':'print_level_random',     'value':'0'                 },
      { 'name':'tolerance_random',       'value':'1e-10'             }
   ]
   #
   # This entry used to fail on fit both because there are no child iota rates
   option_table.append(
      {'name':'zero_sum_child_rate',     'value':'iota' }
   )
   # ----------------------------------------------------------------------
   # avgint table: empty
   avgint_table = list()
   # ----------------------------------------------------------------------
   # nslist_table:
   nslist_table = dict()
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
      nslist_table,
      rate_table,
      mulcov_table,
      option_table
   )
   # ----------------------------------------------------------------------
   n_smooth  = len( smooth_table )
   return
# ===========================================================================
file_name      = 'example.db'
example_db(file_name)
program        = '../../devel/dismod_at'
for command in [ 'init', 'fit' ] :
   cmd = [ program, file_name, command ]
   if command == 'fit' :
      variables = 'both'
      cmd.append(variables)
   print( ' '.join(cmd) )
   flag = subprocess.call( cmd )
   if flag != 0 :
      sys.exit('The dismod_at ' + command + ' command failed')
# -----------------------------------------------------------------------
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
# -----------------------------------------------------------------------
# Results for fitting with no noise
age_table     = dismod_at.get_table_dict(connection, 'age')
time_table    = dismod_at.get_table_dict(connection, 'time')
var_table     = dismod_at.get_table_dict(connection, 'var')
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
#
n_age  = len( age_table )
n_time = len( time_table )
n_var  = len( var_table )
assert n_var == n_age * n_time
#
parent_node_id = 0
eps            = 1e-4
#
# check parent rates values
iota_rate_id      = 1
max_err           = 0.0;
for var_id in range( n_var ) :
   row    = var_table[var_id]
   value  = fit_var_table[var_id]['fit_var_value']
   assert row['var_type'] == 'rate'
   assert row['node_id'] == parent_node_id
   assert row['rate_id'] == iota_rate_id
   max_err = max(max_err, abs( value / iota_mean - 1.0 ) )
assert abs(value / iota_mean - 1.0 ) <= 1e-10
# -----------------------------------------------------------------------------
print('no_data.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
