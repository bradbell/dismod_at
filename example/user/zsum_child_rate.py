# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_zsum_child_rate.py}
# {xrst_comment_ch #}
#
# Constrain Sum of Child Rate Effect to Zero
# ##########################################
#
# See Also
# ********
# :ref:`user_zsum_mulcov_rate.py-name` ,
# :ref:`user_zsum_mulcov_meas.py-name`
#
# Purpose
# *******
# This example demonstrates using
# The :ref:`option_table@Zero Sum Constraints@zero_sum_child_rate`
# to improve the speed and accuracy of estimation of the fixed effects.
#
# Problem Parameters
# ******************
# {xrst_literal
#     begin problem parameters
#     end problem parameters
# }
#
# Data Simulation
# ***************
#
# north_america
# =============
# The true rates for the parent region ``north_america``
# (used for simulating data) are the
# *iota_parent* and *rho_parent* problem parameters.
#
# canada
# ======
# The true
# :ref:`child rate effect<model_variables@Random Effects, u@Child Rate Effects>`
# for ``canada`` is *rate_effect_child* .
#
# united_states
# =============
# The true child rate effect
# for the ``united_states`` is ``-`` *rate_effect_child* .
#
# mexico
# ======
# No data is included for mexico, hence it is automatically constrained to have
# zero child rate random effects.
#
# Nodes
# *****
# There are just four nodes for this example,
# The parent node, ``north_america`` , and the three child nodes
# ``united_states`` , ``canada`` and ``mexico`` .
#
# Model Variables
# ***************
# The non-zero :ref:`model_variables-name` for this example are
# :ref:`rate_table@rate_name@iota` and *rho* .
# Both the parent and child rates use a grid with one point in age
# and two points in time. Thus there are six model variables for each rate,
# two for the parent rates and four for the child rate effects.
# The resulting rates will be constant
# in age and constant in time except between the two time grid points
# where it is linear with respect to time.
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_zsum_child_rate.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
# begin problem parameters
number_data       = 50
iota_parent       = 1e-2
rho_parent        = 2e-2
rate_effect_child = 0.2;
measurement_cv    = 0.01
# end problem parameters
# ------------------------------------------------------------------------
import sys
import os
import copy
import math
import random
import time
test_program  = 'example/user/zsum_child_rate.py'
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
python_seed = int( time.time() )
random.seed( python_seed )
# ------------------------------------------------------------------------
# Note that the a, t values are not used for this example
def example_db (file_name) :
   def fun_rate_child(a, t) :
      return ('prior_rate_child', None, 'prior_gauss_diff')
   def fun_rate_parent(a, t) :
      return ('prior_rate_parent', None, 'prior_gauss_diff')
   import dismod_at
   # ----------------------------------------------------------------------
   # age list
   age_list    = [ 0.0,   50.0,  100.0 ]
   #
   # time list
   time_list   = [ 1990.0, 2010.0 ]
   #
   # integrand table
   integrand_table = [
      { 'name':'Sincidence' },
      { 'name':'remission' }
   ]
   #
   # node table: north_america -> (united_states, canada)
   node_table = [
      { 'name':'north_america', 'parent':''              },
      { 'name':'united_states', 'parent':'north_america' },
      { 'name':'canada',        'parent':'north_america' },
      { 'name':'mexico',        'parent':'north_america'}
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
   # avgint table: same order as list of integrands
   avgint_table = list()
   #
   # nslist_dict:
   nslist_dict = dict()
   # ----------------------------------------------------------------------
   # data table
   data_table = list()
   # write out data
   row = {
      'density':     'gaussian',
      'weight':      '',
      'hold_out':     False,
      'age_lower':    50.0,
      'age_upper':    50.0,
      'subgroup':     'world',
   }
   for data_id in range(number_data) :
      if data_id % 3 == 0 :
         row['node']       = 'north_america'
         row['data_name']  = 'na_' + str( data_id / 3 )
         effect_true       = 0.0
      if data_id % 3 == 1 :
         row['node']       = 'united_states'
         row['data_name']  = 'us_' + str( data_id / 3 )
         effect_true       = - rate_effect_child
      if data_id % 3 == 2 :
         row['node']       = 'canada'
         row['data_name']  = 'ca_' + str( data_id / 3 )
         effect_true       = + rate_effect_child
      if data_id % 2 == 0 :
         row['time_lower'] = 1990.0
         row['time_upper'] = 1990.0
      else :
         row['time_lower'] = 2010.0
         row['time_upper'] = 2010.0
      #
      if data_id < number_data / 2 :
         iota_true         = math.exp(effect_true) * iota_parent
         row['integrand']  = 'Sincidence'
         row['meas_std']   = iota_true * measurement_cv
         noise             = iota_true * random.gauss(0.0, measurement_cv)
         row['meas_value'] = iota_true + noise
      else :
         rho_true          = math.exp(effect_true) * rho_parent
         row['integrand']  = 'remission'
         row['meas_std']   = rho_true * measurement_cv
         noise             = rho_true * random.gauss(0.0, measurement_cv)
         row['meas_value'] = rho_true + noise
      #
      data_table.append( copy.copy(row) )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_rate_parent
         'name':     'prior_rate_parent',
         'density':  'uniform',
         'lower':    min(iota_true, rho_true) / 100.0,
         'upper':    max(iota_true, rho_true) * 100.0,
         'mean':     (iota_true + rho_true),
      },{ # prior_rate_child
         'name':     'prior_rate_child',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      100.0, # very large so like uniform distribution
      },{ # prior_gauss_diff
         'name':     'prior_gauss_diff',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      100.0, # very large so like uniform distribution
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   smooth_table = [
      { # smooth_rate_child
         'name':                     'smooth_rate_child',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0, 1 ],
         'fun':                      fun_rate_child
      },{ # smooth_rate_parent
         'name':                     'smooth_rate_parent',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0, 1 ],
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
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'parent_node_name',       'value':'north_america'     },
      { 'name':'zero_sum_child_rate',    'value':'iota rho'          },
      { 'name':'random_seed',            'value':'0'                 },
      { 'name':'ode_step_size',          'value':'10.0'              },
      { 'name':'rate_case',              'value':'iota_pos_rho_pos'  },

      { 'name':'quasi_fixed',            'value':'true'          },
      { 'name':'derivative_test_fixed',  'value':'first-order'   },
      { 'name':'max_num_iter_fixed',     'value':'100'           },
      { 'name':'print_level_fixed',      'value':'0'             },
      { 'name':'tolerance_fixed',        'value':'1e-12'         },

      { 'name':'derivative_test_random', 'value':'second-order'  },
      { 'name':'max_num_iter_random',    'value':'100'           },
      { 'name':'print_level_random',     'value':'0'             },
      { 'name':'tolerance_random',       'value':'1e-10'         }
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
# Create database and run init, start, fit with zero sum for random effects
file_name = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
# -----------------------------------------------------------------------
# connect to database
connection      = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
# -----------------------------------------------------------------------
# check the zero random effects solution
#
# get variable and fit_var tables
var_table     = dismod_at.get_table_dict(connection, 'var')
fit_var_table = dismod_at.get_table_dict(connection, 'fit_var')
rate_table    = dismod_at.get_table_dict(connection, 'rate')
node_table    = dismod_at.get_table_dict(connection, 'node')
time_table    = dismod_at.get_table_dict(connection, 'time')
#
# for node = north_america, canada, united_states, mexico
#  for time = 1990, 2010
#     for rate = iota, rho :
n_var = len(var_table)
assert n_var == 4 * 2 * 2
#
# initialize sum of random effects for each rate and time
sum_random = {
   'iota' : [ 0.0, 0.0 ],
   'rho'  : [ 0.0, 0.0 ]
}
# check of values uses the fact that the data density is Gaussian
count_random = 0
ok           = True
for var_id in range( n_var ) :
   var_type = var_table[var_id]['var_type']
   assert( var_type == 'rate' )
   #
   rate_id   = var_table[var_id]['rate_id']
   rate_name = rate_table[rate_id]['rate_name']
   #
   node_id   = var_table[var_id]['node_id']
   node_name = node_table[node_id]['node_name']
   #
   # note there are only two time_id values in time_table
   time_id   = var_table[var_id]['time_id']
   time      = time_table[time_id]['time']
   #
   value     = fit_var_table[var_id]['fit_var_value']
   #
   if node_name == 'north_america' :
      if rate_name == 'iota' :
         relerr = value / iota_parent - 1.0
      else :
         relerr = value / rho_parent - 1.0
   elif node_name == 'canada' :
      relerr = value / rate_effect_child  - 1.0
   elif node_name == 'mexico' :
      relerr = value - 0.0
   else :
      assert node_name == 'united_states'
      relerr = - value / rate_effect_child  - 1.0
   if abs(relerr) > 0.1 :
      print('node_name, relerr=', node_name, relerr)
      print('python_seed = ', python_seed)
      assert False
   if node_name != 'north_america' :
      sum_random[rate_name][time_id] += value
      count_random += 1
assert count_random == 3 * 2 * 2
for rate in [ 'iota', 'rho' ] :
   for time_id in [ 0 , 1 ] :
      if( abs( sum_random[rate][time_id] ) ) > 1e-9 :
         print('rate, sum random = ', rate, sum_random[rate][time_id] )
         print('python_seed = ', python_seed)
         assert False
#
# -----------------------------------------------------------------------
print('zsum_child_rate.py: OK')
# END PYTHON
