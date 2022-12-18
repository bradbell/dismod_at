# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# Case where fixed effects are equality constrained (this used to crash ipopt).
# ----------------------------------------------------------------------------
# true values used to simulate data
iota_true            = 0.05
# ------------------------------------------------------------------------
import sys
import os
import subprocess
test_program = 'test/user/fixed_eq_con.py'
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
# prior for parent rates
def fun_rate_parent(a, t) :
   return ('prior_rate_parent', 'prior_gauss_zero', 'prior_gauss_zero')
# ------------------------------------------------------------------------
def example_db (file_name) :
   import copy
   import dismod_at
   import math
   # ----------------------------------------------------------------------
   # age table
   age_list    = [    0.0, 100.0 ]
   #
   # time table
   time_list   = [ 1900.0, 2015.0 ]
   #
   # integrand table
   integrand_table = list()
   #
   # node_table
   node_table = [
      { 'name':'world',         'parent':'' },
   ]
   #
   # weight table: The constant function 1.0 (one age and one time point)
   weight_table = list()
   #
   # covariate table:
   covariate_table = list()
   #
   # mulcov table
   mulcov_table = list()
   #
   # data table:
   data_table = list()
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_gauss_zero
         'name':     'prior_gauss_zero',
         'density':  'gaussian',
         'lower':    None,
         'upper':    None,
         'mean':     0.0,
         'std':      0.1,
         'eta':      None
      },{ # prior_rate_parent
         'name':     'prior_rate_parent',
         'density':  'uniform',
         'lower':    iota_true,
         'upper':    iota_true,
         'mean':     iota_true,
         'std':      None,
         'eta':      None
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   smooth_table = [
      { # smooth_rate_parent
         'name':                     'smooth_rate_parent',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0 ],
         'mulstd_value_prior_name':  None,
         'mulstd_dage_prior_name':   None,
         'mulstd_dtime_prior_name':  None,
         'fun':                      fun_rate_parent
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [
      {
         'name':          'iota',
         'parent_smooth': 'smooth_rate_parent',
         'child_smooth':  None,
         'child_nslist':  None
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'parent_node_name',       'value':'world'             },
      { 'name':'rate_case',              'value':'iota_pos_rho_zero' },
      { 'name':'quasi_fixed',            'value':'true'              },
   ]
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
var_table     = dismod_at.get_table_dict(connection, 'var')
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
#
parent_node_id = 0
iota_rate_id   = 1
eps            = 1e-7
#
# check parent rates values
iota_rate_id      = 1
assert len(var_table) == 1
for var_id in range( len(var_table) ) :
   row   = var_table[var_id]
   value = fit_var_table[var_id]['fit_var_value']
   assert row['node_id'] == parent_node_id
   assert row['rate_id'] == iota_rate_id
   assert value == iota_true
# -----------------------------------------------------------------------------
print('no_variable.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
