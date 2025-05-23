# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_diff_constraint.py}
# {xrst_comment_ch #}
#
# Fitting with Constraints on Differences in Age and Time
# #######################################################
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_diff_constraint.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
import copy
test_program  = 'example/user/diff_constraint.py'
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
# change into the build/example/user directory
if not os.path.exists('build/example/user') :
   os.makedirs('build/example/user')
os.chdir('build/example/user')
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def example_db (file_name) :
   # note that the a, t values are not used for this case
   def fun_rate_child(a, t) :
      return ('prior_gauss_zero',   'prior_gauss_zero',  'prior_gauss_zero')
   def fun_rate_parent(a, t) :
      return ('prior_value_parent', 'prior_diff_parent', 'prior_diff_parent')
   # ----------------------------------------------------------------------
   # age table
   age_list    = [    0.0,   50.0,   100.0 ]
   #
   # time table
   time_list   = [ 1995.0, 2005.0,  2015.0 ]
   #
   # integrand table
   integrand_table = [
      { 'name':'Sincidence' },
      { 'name':'remission' },
      { 'name':'mtexcess' },
      { 'name':'mtother' }
   ]
   #
   # node table: world         -> north_america
   #             north_america -> (united_states, canada)
   node_table = [
      { 'name':'world',         'parent':'' },
      { 'name':'north_america', 'parent':'world' },
      { 'name':'united_states', 'parent':'north_america' },
      { 'name':'canada',        'parent':'north_america' }
   ]
   #
   # weight table:
   weight_table = list()
   #
   # covariate table: no covriates
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
   # data table: same order as list of integrands
   data_table = list()
   # values that are the same for all data rows
   row = {
      'node':        'world',
      'subgroup':    'world',
      'density':     'gaussian',
      'weight':      '',
      'hold_out':     False,
      'time_lower':   1995.0,
      'time_upper':   1995.0,
      'age_lower':    0.0,
      'age_upper':    0.0
   }
   # values that change between rows: (one data point for each integrand)
   for integrand_id in range( len(integrand_table) ) :
      rate_id           = integrand_id
      meas_value        = 0.05
      meas_std          = 0.2 * meas_value
      integrand         = integrand_table[integrand_id]['name']
      row['meas_value'] = meas_value
      row['meas_std']   = meas_std
      row['integrand']  = integrand
      # data_id = rate_id = integand_id
      data_table.append( copy.copy(row) )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_gauss_zero
         'name':     'prior_gauss_zero',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      0.01,
      },{ # prior_value_parent
         'name':     'prior_value_parent',
         'density':  'uniform',
         'lower':    0.01,
         'upper':    1.00,
         'mean':     0.1,
      },{ # prior_diff_parent
         'name':     'prior_diff_parent',
         'density':  'gaussian',
         'lower':    0.01,
         'upper':    1.0,
         'mean':     0.01,
         'std':      0.01,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   middle_age_id  = 1
   middle_time_id = 1
   last_age_id    = 2
   last_time_id   = 2
   smooth_table = [
      {   # smooth_rate_child
         'name':                     'smooth_rate_child',
         'age_id':                   [ 0, last_age_id ],
         'time_id':                  [ 0, last_time_id ],
         'fun':                      fun_rate_child
      },{ # smooth_rate_parent
         'name':                     'smooth_rate_parent',
         'age_id':                   [ 0, last_age_id ],
         'time_id':                  [ 0, last_time_id ],
         'fun':                       fun_rate_parent
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [
      {
         'name':          'iota',
         'parent_smooth': 'smooth_rate_parent',
         'child_smooth':  'smooth_rate_child',
      },{
         'name':          'rho',
         'parent_smooth': 'smooth_rate_parent',
         'child_smooth':  'smooth_rate_child',
      },{
         'name':          'chi',
         'parent_smooth': 'smooth_rate_parent',
         'child_smooth':  'smooth_rate_child',
      },{
         'name':          'omega',
         'parent_smooth': 'smooth_rate_parent',
         'child_smooth':  'smooth_rate_child',
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'parent_node_name',       'value':'world'        },
      { 'name':'ode_step_size',          'value':'10.0'         },
      { 'name':'random_seed',            'value':'0'            },
      { 'name':'rate_case',              'value':'iota_pos_rho_pos' },

      { 'name':'quasi_fixed',            'value':'true'         },
      { 'name':'derivative_test_fixed',  'value':'first-order'  },
      { 'name':'max_num_iter_fixed',     'value':'100'          },
      { 'name':'print_level_fixed',      'value':'0'            },
      { 'name':'tolerance_fixed',        'value':'1e-10'        },

      { 'name':'derivative_test_random', 'value':'second-order' },
      { 'name':'max_num_iter_random',    'value':'100'          },
      { 'name':'print_level_random',     'value':'0'            },
      { 'name':'tolerance_random',       'value':'1e-10'        }
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
# ===========================================================================
file_name = 'example.db'
example_db(file_name)
#
program  = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
# -----------------------------------------------------------------------
# connect to database
connection      = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
# -----------------------------------------------------------------------
# get parent rate variable values
var_table     = dismod_at.get_table_dict(connection, 'var')
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
#
middle_age_id  = 1
middle_time_id = 1
last_age_id    = 2
last_time_id   = 2
parent_node_id = 0
n_rate         = 5
tol            = 1e-8
for rate_id in range(n_rate) :
   rate_value = dict()
   count      = 0
   for var_id in range( len(var_table) ) :
      row   = var_table[var_id]
      match = row['var_type'] == 'rate'
      match = match and row['rate_id'] == rate_id
      match = match and row['node_id'] == parent_node_id
      if match :
         age_id  = row['age_id']
         time_id = row['time_id']
         if age_id not in rate_value :
            rate_value[age_id] = dict()
         rate_value[age_id][time_id] = \
            fit_var_table[var_id]['fit_var_value']
         #
         assert fit_var_table[var_id]['lagrange_value'] == 0.0
         if age_id == last_age_id :
            assert fit_var_table[var_id]['lagrange_dage'] == 0.0
         else :
            # lower limit is active, so multiplier is less than zero
            assert fit_var_table[var_id]['lagrange_dage'] < 0.0
         if time_id == last_time_id :
            assert fit_var_table[var_id]['lagrange_dtime'] == 0.0
         else :
            # lower limit is active, so multiplier is less than zero
            assert fit_var_table[var_id]['lagrange_dtime'] < 0.0

         count += 1
   if rate_id == 0 :
      # no pini variables because its parent and child smoothings are null
      assert count == 0
   else :
      # other rates
      assert count == 4
      assert ( rate_value[0][0] / 0.05 - 1.0 ) < tol
      #
      diff  = rate_value[last_age_id][0] - rate_value[0][0]
      assert diff - 0.01 > - tol             # due to contraint
      assert abs(diff / 0.01 - 1.0) < 1e-3   # due to smoothing objective
      #
      diff  = rate_value[0][last_time_id] - rate_value[0][0]
      assert diff - 0.01 > - tol
      assert abs(diff / 0.01 - 1.0) < 1e-3
      #
      diff  = rate_value[last_age_id][0] - rate_value[0][0]
      assert diff - 0.01 > - tol
      assert abs(diff / 0.01 - 1.0) < 1e-3
      #
      diff  = rate_value[last_age_id][last_time_id] \
         - rate_value[last_age_id][0]
      assert diff - 0.01 > - tol
      assert abs(diff / 0.01 - 1.0) < 1e-3
# -----------------------------------------------------------------------------
print('diff_constraint.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
