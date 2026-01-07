# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
omega_world_mean  = 1e-2
omega_world_upper = omega_world_mean * 5.0
omega_world_lower = omega_world_mean / 5.0
number_sample     = 100
# ------------------------------------------------------------------------
import sys
import os
import subprocess
import copy
import csv
from math import exp
test_program  = 'test/user/censor_asymptotic.py'
check_program = sys.argv[0].replace('\\', '/')
if check_program != test_program  or len(sys.argv) != 1 :
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
# Note that the a, t values are not used for this example
def fun_rate_world(a, t) :
   return ('prior_rate_world', None, 'prior_world_diff')
# ------------------------------------------------------------------------
def example_db (file_name) :
   # ----------------------------------------------------------------------
   # age table
   age_list    = [    0.0, 100.0 ]
   #
   # time table
   time_list   = [ 1995.0, 2015.0 ]
   #
   # integrand table
   integrand_table = list()
   #
   # node table:
   node_table = [ { 'name':'world',  'parent':'' } ]
   #
   # weight table
   weight_table = list()
   #
   # covariate table: no covariates
   covariate_table = list()
   #
   # mulcov table
   mulcov_table = list()
   #
   # avgint table: same order as list of integrands
   avgint_table = list()
   #
   # nslist_dict:
   nslist_dict = dict()
   #
   # data table:
   data_table = list()
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_rate_world
         'name':     'prior_rate_world',
         'density':  'gaussian',
         'mean':     omega_world_mean,
         'std':      omega_world_upper - omega_world_lower,
         'lower':    omega_world_lower,
         'upper':    omega_world_upper,
      },{ # prior_world_diff
         'name':     'prior_world_diff',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      omega_world_mean * 3.0
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   last_time_id = len(time_list) - 1
   smooth_table = [
      { # smooth_rate_world
         'name':                     'smooth_rate_world',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0, last_time_id ],
         'fun':                       fun_rate_world
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [
      {
         'name':          'omega',
         'parent_smooth': 'smooth_rate_world',
         'child_smooth':  None,
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'parent_node_name',         'value':'world'             },
      { 'name':'rate_case',                'value':'iota_zero_rho_zero'},

      { 'name':'quasi_fixed',            'value':'true'              },
      { 'name':'derivative_test_fixed',  'value':'first-order'       },
      { 'name':'max_num_iter_fixed',     'value':'100'               },
      { 'name':'print_level_fixed',      'value':'0'                 },
      { 'name':'tolerance_fixed',        'value':'1e-11'             },

      { 'name':'derivative_test_random', 'value':'second-order'      },
      { 'name':'max_num_iter_random',    'value':'100'               },
      { 'name':'print_level_random',     'value':'0'                 },
      { 'name':'tolerance_random',       'value':'1e-11'             }
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
# -----------------------------------------------------------------------
# Create database and run init, start, fit with just fixed effects
file_name = 'example.db'
example_db(file_name)
program   = '../../devel/dismod_at'
#
# init
cmd       = [ program, file_name, 'init' ]
result    = dismod_at.system_command_prc(cmd, return_stderr = True)
if result.returncode != 0 :
   print( result.stderr )
   sys.exit('The dismod_at init command failed')
#
# fit both
cmd       = [ program, file_name, 'fit', 'both' ]
result    = dismod_at.system_command_prc(cmd, return_stderr = True)
if result.returncode != 0 :
   print( result.stderr )
   sys.exit('The dismod_at fit command failed')
#
# sample censor_asymptotic
cmd = [
   program, file_name, 'sample', 'censor_asymptotic', 'both', str(number_sample)
]
dismod_at.system_command_prc(cmd, return_stderr = True)
#
# var_table, sample_table
connection     = dismod_at.create_connection(file_name, new = False)
rate_table     = dismod_at.get_table_dict(connection, 'rate')
var_table      = dismod_at.get_table_dict(connection, 'var')
sample_table   = dismod_at.get_table_dict(connection, 'sample')
connection.close()
for row in var_table :
   rate_id = row['rate_id']
   rate_name = rate_table[rate_id]['rate_name']
   assert rate_name == 'omega'
for row in sample_table:
   var_value = row['var_value']
   if not omega_world_lower <= var_value :
      assert False, f'not {omega_world_lower} <= {var_value}'
   if not var_value  <= omega_world_upper :
      assert False, f'not {var_value} < {omega_world_lower}'


# -----------------------------------------------------------------------
print('censor_asymptotic.py: OK')
# END PYTHON
