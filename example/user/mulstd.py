# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_mulstd.py}
# {xrst_comment_ch #}
#
# Estimating Smoothing Standard Deviation Multiplies
# ##################################################
#
# Purpose
# *******
# This example uses a smoothing standard deviation multiplier
# :ref:`lambda<model_variables@Fixed Effects, theta@Smoothing Standard Deviation Multipliers, lambda>`
# to determine the standard deviation of the random effects.
#
# Problem Parameters
# ******************
# The following values are used to simulate and model the data:
# {xrst_literal
#     begin problem parameters
#     end problem parameters
# }
#
# Age and Time Values
# *******************
# The age and time values do not matter for this problem
# because all the functions are constant in age and time.
# This can be seen by the fact that all of the smoothing has one age
# and one time point.
#
# Variables
# *********
#
# Parent
# ======
# A constant value used to model
# :ref:`iota<avg_integrand@Rate Functions@iota_i(a,t)>`
# for the parent node.
#
# Children
# ========
# A fixed value is used for each of the
# :ref:`model_variables@Random Effects, u@Child Rate Effects`
# so that this example passes its test without having a lot of children.
# You could try increasing the number of children and simulating
# the rate random effect for each child.
#
# Data Table
# **********
# For this example, all the data is
# :ref:`avg_integrand@Integrand, I_i(a,t)@Sincidence`
# with a known standard deviation.
#
# Rate Table
# **********
# The :ref:`rate_table-name` only specifies that *iota* for the parent
# and children are non-zero.
#
# Prior Table
# ***********
#
# Parent
# ======
# The prior for the parent node *iota* is uniform with lower limit 1e-4,
# upper limit 1.0 and mean 0.1.
# Note that the mean is not really the mean of this uniform distribution
# and it is only used to get the initial starting and scaling point
# for the optimization; see :ref:`init_command-name` .
#
# Children
# ========
# The prior for the child nodes *iota* is Gaussian
# with mean zero and standard deviation one.
# This is so that the actual standard deviation is *lambda*  * 1
# which is equal to *lambda* .
#
# Fitting
# *******
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_mulstd.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
# begin problem parameters
iota_world      = 1e-2     # true iota for parent
iota_effect_std = +0.5     # true iota effect standard deviation
meas_cv         = 0.05     # coefficient of variation for data points
n_data          = 400      # total number of data points
random_seed     = 0        # if zero, seed off the clock
# end problem parameters
# ------------------------------------------------------------------------
effect_child_1 = + iota_effect_std
effect_child_2 = - iota_effect_std
import time
if random_seed == 0 :
   random_seed = int( time.time() )
#
import sys
import os
import copy
import math
import random
test_program = 'example/user/mulstd.py'
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
def example_db (file_name) :
   import dismod_at
   def fun_rate_parent(a, t) :
      return ('prior_iota_parent', None, None)
   def fun_rate_child(a, t) :
      return ('prior_iota_child', None, None)
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
   node_table = [
      { 'name':'world',   'parent':'' },
      { 'name':'child_1', 'parent':'world' },
      { 'name':'child_2', 'parent':'world' }
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
   # data table: same order as list of integrands
   data_table = list()
   row = {
      'density':     'gaussian',
      'weight':      '',
      'hold_out':     False,
      'time_lower':   2000.0,
      'time_upper':   2000.0,
      'age_lower':    50.0,
      'age_upper':    50.0,
      'integrand':    'Sincidence',
   }
   random.seed(random_seed)
   for data_id in range( n_data ):
      if data_id % 3 == 0 :
         row['node'] = 'world'
         iota        = iota_world
      if data_id % 3 == 1 :
         row['node'] = 'child_1'
         iota        = iota_world * math.exp(effect_child_1)
      if data_id % 3 == 2 :
         row['node'] = 'child_2'
         iota        = iota_world * math.exp(effect_child_2)
      meas_std          = meas_cv * iota
      meas_value        = random.gauss(iota, meas_std)
      row['meas_std']   = meas_std
      row['meas_value'] = meas_value
      #
      data_table.append( copy.copy(row) )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_iota_parent
         'name':     'prior_iota_parent',
         'density':  'uniform',
         'lower':    1e-4,
         'upper':    1.0,
         'mean':     0.1,
      },{ # prior_iota_child
         'name':     'prior_iota_child',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      1.0,
      },{ # prior_effect_std
         'name':     'prior_effect_std',
         'density':  'uniform',
         'upper':     2.0,
         'lower':     0.02,
         'mean':      1.0,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   smooth_table = [
      { # smooth_rate_parent
         'name':                     'smooth_rate_parent',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0 ],
         'fun':                      fun_rate_parent
      },{ # smooth_rate_child
         'name':                     'smooth_rate_child',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0 ],
         'fun':                      fun_rate_child,
         'mulstd_value_prior_name':  'prior_effect_std'
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [
      {
         'name':          'iota',
         'parent_smooth': 'smooth_rate_parent',
         'child_smooth':  'smooth_rate_child',
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'parent_node_name',       'value':'world'             },
      { 'name':'rate_case',              'value':'iota_pos_rho_zero' },

      { 'name':'quasi_fixed',            'value':'false'             },
      { 'name':'max_num_iter_fixed',     'value':'100'               },
      { 'name':'print_level_fixed',      'value':'0'                 },
      { 'name':'tolerance_fixed',        'value':'1e-11'             },
      { 'name':'zero_sum_child_rate',    'value':'iota'              },
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
# Create database and run init, start, fit with just fixed effects
file_name = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
#
# connect to database and get density table
connection      = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
node_table      = dismod_at.get_table_dict(connection, 'node')
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
rate_var_true = {
   'world':iota_world, 'child_1':effect_child_1, 'child_2':effect_child_2
}
for var_id in range( len(var_table  ) ):
   fit_var_value = fit_var_table[var_id]['fit_var_value']
   var_type      = var_table[var_id]['var_type']
   if var_type == 'rate' :
      node_id    = var_table[var_id]['node_id']
      node_name  = node_table[node_id]['node_name']
      true_value = rate_var_true[node_name]
   else :
      assert var_type == 'mulstd_value'
      true_value = iota_effect_std
   relerr = fit_var_value / true_value - 1.0
   if abs(relerr) > .01 :
      print('random_seed = ', random_seed)
      assert(false)
# -----------------------------------------------------------------------
print('mulstd.py: OK')
# END PYTHON
