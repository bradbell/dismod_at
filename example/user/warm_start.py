# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_warm_start.py}
# {xrst_spell
#     iter
#     num
#     stderr
# }
# {xrst_comment_ch #}
#
# Continuing a Fit Using Ipopt Warm Start
# #######################################
#
# Option Table
# ************
# In the option table defined below,
# *max_num_iter_fixed*  = 5 .
# This fit will terminate when
# the maximum number of iterations is reached.
# The corresponding warning is suppressed by setting
# *warn_on_stderr* = ``false`` .
# The second fit will start where the first left off.
# To see this, set *print_level_fixed*  = 5 (in the option table) and
# :ref:`run this example<user_example@Run One Example>` .
#
# Fixed Trace Table
# *****************
# This example uses the :ref:`trace_fixed_table-name`
# to check the number of iterations used.
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_warm_start.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# values used to simulate data
iota_true                 = 0.01
# ------------------------------------------------------------------------
import sys
import os
import copy
test_program = 'example/user/warm_start.py'
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
# change into the build/example/user directory
if not os.path.exists('build/example/user') :
   os.makedirs('build/example/user')
os.chdir('build/example/user')
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def example_db (file_name) :
   # note that the a, t values are not used for this case
   def fun_iota(a, t) :
      return ('prior_iota', None, None)
   def fun_chi(a, t) :
      return (chi_true, None, None)
   # ----------------------------------------------------------------------
   # age table:
   age_list    = [ 0.0, 5.0, 15.0, 35.0, 50.0, 75.0, 90.0, 100.0 ]
   #
   # time table:
   time_list   = [ 1990.0, 2000.0, 2010.0, 2200.0 ]
   #
   # integrand table:
   integrand_table = [
       { 'name':'Sincidence' }
   ]
   #
   # node table:
   node_table = [ { 'name':'world', 'parent':'' } ]
   #
   # weight table:
   weight_table = list()
   #
   # covariate table:
   covariate_table = list()
   #
   # mulcov table:
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
   # values that are the same for all data rows
   row = {
      'integrand':   'Sincidence',
      'meas_value':  iota_true,
      'density':     'gaussian',
      'meas_std':    iota_true / 10.,
      'weight':      '',
      'hold_out':     False,
      'age_lower':    50.0,
      'age_upper':    50.0,
      'time_lower':   2000.,
      'time_upper':   2000.,
      'node':         'world',
      'subgroup':     'world',
   }
   data_table.append( copy.copy(row) )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_iota
         'name':     'prior_iota',
         'density':  'uniform',
         'lower':    iota_true / 10.,
         'upper':    iota_true * 10.,
         'mean':     iota_true * 2.0,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   name           = 'smooth_iota'
   fun            = fun_iota
   smooth_table = [
      {  'name':name,
         'age_id':[0],
         'time_id':[0],
         'fun':fun
      }
   ]
   # ----------------------------------------------------------------------
   # rate table:
   rate_table = [
      {  'name':          'iota',
         'parent_smooth': 'smooth_iota',
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'rate_case',              'value':'iota_pos_rho_zero'   },
      { 'name':'parent_node_name',       'value':'world'               },
      { 'name':'warn_on_stderr',         'value':'false'               },

      { 'name':'quasi_fixed',            'value':'false'               },
      { 'name':'max_num_iter_fixed',     'value':'5'                   },
      { 'name':'print_level_fixed',      'value':'0'                   },
      { 'name':'tolerance_fixed',        'value':'1e-8'                },

      { 'name':'max_num_iter_random',    'value':'50'                  },
      { 'name':'print_level_random',     'value':'0'                   },
      { 'name':'tolerance_random',       'value':'1e-10'               }
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
# ===========================================================================
# Create database
file_name = 'example.db'
example_db(file_name)
#
# first fit command
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
#
# trace_fixed table
connection        = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
trace_fixed_table = dismod_at.get_table_dict(connection, 'trace_fixed')
connection.close()
# trace includes iteration zero
assert( len(trace_fixed_table) == 6 )
#
# warm start second fit
dismod_at.system_command_prc(
   [ program, file_name, 'fit', 'fixed', 'warm_start'
])
# -----------------------------------------------------------------------
# read database
connection        = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
var_table         = dismod_at.get_table_dict(connection, 'var')
rate_table        = dismod_at.get_table_dict(connection, 'rate')
fit_var_table     = dismod_at.get_table_dict(connection, 'fit_var')
log_table         = dismod_at.get_table_dict(connection, 'log' )
trace_fixed_table = dismod_at.get_table_dict(connection, 'trace_fixed')
connection.close()
#
# second fit should converge in 2 iteations
assert( len(trace_fixed_table) <= 3 )
#
# check that we a warning (maximum number iterations during first fit)
warning_count = 0
for row in log_table :
   if row['message_type'] == 'warning' :
      warning_count += 1
assert warning_count in [ 1, 2]
#
assert len(var_table) == 1
fit_value = fit_var_table[0]['fit_var_value']
var_row   = var_table[0]
rate_id   = var_row['rate_id']
rate_name = rate_table[rate_id]['rate_name']
assert rate_name == 'iota'
rel_err   = fit_value / iota_true - 1.0
if abs( rel_err ) > 1e-6 :
   print( "iota rel_err = ", rel_err)
# -----------------------------------------------------------------------------
print('warm_start.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
