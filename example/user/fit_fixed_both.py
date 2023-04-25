# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_fit_fixed_both.py}
# {xrst_spell
#     exp
# }
# {xrst_comment_ch #}
#
# Fit Fixed First Then Both
# #########################
#
# Purpose
# *******
# This example demonstrates using the commands
#
# | |tab| ``dismod_at`` *database* ``fit fixed``
# | |tab| ``dismod_at`` *database* ``set start_var fit_var``
# | |tab| ``dismod_at`` *database* ``fit both``
#
# This stabilizes the optimization when the ``init`` command
# :ref:`init_command@start_var_table` is
# far from the optimal fixed effects; see
# :ref:`fit_command-name` .
#
# Discussion
# **********
# The following describes the model and data for this example:
#
# #. The :ref:`age_table-name` has values
#    ``0.0`` , ``50.0`` , ``100.0`` .
#    The :ref:`time_table-name` has values
#    ``1995.0`` , ``2005.0`` , ``2015.0`` .
# #. The parent node is North America, the child nodes are
#    Canada and the United States.
# #. The only :ref:`model_variables-name` in this example are
#    :ref:`rate_table@rate_name@iota` for the parent and the
#    corresponding random effects for two children.
#    These rates are modeled as constant with respect to age and
#    linear between time 1995 and 2015.
#    The true iota is
#
#    .. csv-table::
#        :widths: auto
#
#        0.01,North America
#        0.01 * exp(+0.5),United States
#        0.01 * exp(-0.5),Canada
#
#    Note that the random effect for the United States is +0.5
#    and for Canada it is -0.5.
# #. There are three measurements, one for each node.
#    All the measurements are at age 50 and time 2000
#    (there is no age or time interval for the data points).
#    The measurement value is exactly equal to the true value of *iota*
#    for the corresponding node.
#    The measurement noise is modeled to have a 10 percent coefficient
#    of variation (even though there is no noise in the actual measurements).
# #. The prior for North America is a uniform with mean equal to
#    the true value for North America divided by 100. This makes the fixed effect
#    in :ref:`init_command@start_var_table` a poor starting
#    value.
#    The prior for Canada and the United States is a Gaussian with mean zero and
#    standard deviation 100.
#    The large standard deviation is so that it does not have much effect.
# #. The prior for the difference in *iota* between time 1995
#    and time 2015 for the children (parent) is a Gaussian (log Gaussian)
#    with mean zero and standard deviation 0.1.
# #. The init command is sets
#    :ref:`init_command@start_var_table` equal to the
#    prior mean.
#    The prior mean for North America (the fixed effect) is far from its
#    true values and doing a fit fixed obtains a better starting point
#    for the fit both.
#
# {xrst_code py}

#
# {xrst_code}
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_fit_fixed_both.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
iota_parent_true            = 1e-2
united_states_random_effect = +0.5
# ------------------------------------------------------------------------
import sys
import os
import copy
from math import exp
test_program = 'example/user/fit_fixed_both.py'
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
   def fun_iota_child(a, t) :
      return ('prior_iota_child', None, 'prior_child_diff')
   def fun_iota_parent(a, t) :
      return ('prior_iota_parent', None, 'prior_parent_diff')
   import dismod_at
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
   # avgint table:
   avgint_table = list()
   #
   # nslist_dict:
   nslist_dict = dict()
   # ----------------------------------------------------------------------
   # data table:
   data_table = list()
   # write out data
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
   row['node']        = 'north_america'
   row['meas_value']  = iota_parent_true
   row['meas_std']    = row['meas_value'] * 1e-1
   data_table.append( copy.copy(row) )
   row['node']        = 'united_states'
   row['meas_value']  = iota_parent_true * exp(+ united_states_random_effect)
   row['meas_std']    = row['meas_value'] * 1e-1
   data_table.append( copy.copy(row) )
   row['node']        = 'canada'
   row['meas_value']  = iota_parent_true * exp(- united_states_random_effect)
   data_table.append( copy.copy(row) )
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_iota_parent
         'name':     'prior_iota_parent',
         'density':  'uniform',
         'lower':    iota_parent_true * 1e-2,
         'upper':    iota_parent_true * 1e+2,
         'mean':     iota_parent_true * 2.0
      },{ # prior_iota_child
         'name':     'prior_iota_child',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      100.0, # very large so like a uniform distribution
      },{ # prior_parent_diff
         'name':     'prior_parent_diff',
         'density':  'log_gaussian',
         'mean':     0.0,
         'std':      0.1,
         'eta':      1e-8
      },{ # prior_child_diff
         'name':     'prior_child_diff',
         'density':  'gaussian',
         'mean':     0.0,
         'std':      0.1,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   last_time_id   = 2
   smooth_table = [
      { # smooth_iota_child
         'name':                     'smooth_iota_child',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0, last_time_id ],
         'fun':                      fun_iota_child
      },{ # smooth_iota_parent
         'name':                     'smooth_iota_parent',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0, last_time_id ],
         'fun':                       fun_iota_parent
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [
      {
         'name':          'iota',
         'parent_smooth': 'smooth_iota_parent',
         'child_smooth':  'smooth_iota_child',
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'parent_node_name',       'value':'north_america' },

      { 'name':'quasi_fixed',            'value':'true'          },
      { 'name':'derivative_test_fixed',  'value':'first-order'   },
      { 'name':'max_num_iter_fixed',     'value':'100'           },
      { 'name':'tolerance_fixed',        'value':'1e-11'         },

      { 'name':'derivative_test_random', 'value':'second-order'  },
      { 'name':'max_num_iter_random',    'value':'100'           },
      { 'name':'tolerance_random',       'value':'1e-11'         }
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
# Create database and run init, start, fit with just fixed effects
file_name = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
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
#
# one age and two times for each of north_america, canada, unites_states
n_var = len(var_table)
assert n_var == 6
#
# check that all the random effects are zero after a fit fixed
for var_id in range( n_var ) :
   var_type = var_table[var_id]['var_type']
   assert( var_type == 'rate' )
   #
   rate_id = var_table[var_id]['rate_id']
   assert( rate_table[rate_id]['rate_name'] == 'iota' )
   #
   value   = fit_var_table[var_id]['fit_var_value']
   #
   node_id  = var_table[var_id]['node_id']
   parent    = node_table[node_id]['node_name'] == 'north_america'
   if parent :
      # check that result of fit fixed in much better than prior mean
      assert value / iota_parent_true < 2.0
      assert 0.5 < value / iota_parent_true
   else :
      canada         = node_table[node_id]['node_name'] == 'canada'
      united_states  = node_table[node_id]['node_name'] == 'united_states'
      assert canada or united_states
      assert value == 0.0
# -----------------------------------------------------------------------
# Copy results of fit fixed to start table
cmd = '../../devel/dismod_at example.db set start_var fit_var'
dismod_at.system_command_prc([ program, file_name, 'set', 'start_var', 'fit_var' ])
#
# Fit both fixed and random effects
dismod_at.system_command_prc([ program, file_name, 'fit', 'both' ])
# -----------------------------------------------------------------------
# check the non-zero random effects solution
#
# get solution from fit_var table
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
for var_id in range( n_var ) :
   var_type = var_table[var_id]['var_type']
   assert( var_type == 'rate' )
   #
   rate_id = var_table[var_id]['rate_id']
   assert( rate_table[rate_id]['rate_name'] == 'iota' )
   #
   value   = fit_var_table[var_id]['fit_var_value']
   #
   node_id  = var_table[var_id]['node_id']
   parent   = node_table[node_id]['node_name'] == 'north_america'
   if parent :
      err = value / iota_parent_true - 1.0
      assert( abs(err) < 1e-5 )
   else :
      canada         = node_table[node_id]['node_name'] == 'canada'
      united_states  = node_table[node_id]['node_name'] == 'united_states'
      if united_states :
         child_optimal = united_states_random_effect
      if canada :
         child_optimal = - united_states_random_effect
      err = value / child_optimal - 1.0
      assert( abs(err) < 1e-4 )
# -----------------------------------------------------------------------
print('fit_fixed_both.py: OK')
# END PYTHON
