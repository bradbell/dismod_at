# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_hold_out_1.py}
# {xrst_spell
#     exp
# }
# {xrst_comment_ch #}
#
# Using hold_out in Data, Subset Data, and Option Tables
# ######################################################
#
# Purpose
# *******
# This example shows how to use hold_out in the
# :ref:`data_table<data_table@hold_out>` ,
# :ref:`option_table<option_table@hold_out_integrand>` , and
# :ref:`hold_out_command-name` .
#
# Integrands
# **********
# For this example there are two integrand,
# ``Sincidence`` and ``prevalence`` .
#
# Nodes
# *****
# The node table is set up so that there are lots of child nodes
# (with no data. This makes sure that the data being fit gets
# evenly distributed between the nodes that do have data and
# reaches :ref:`hold_out_command@max_fit` .
#
# Data
# ****
#
# prevalence
# ==========
# All of the prevalence data is zero, but it is held out using
# :ref:`option_table@hold_out_integrand` .
#
# Sincidence
# ==========
# There are many incidence data points.
# The first Sincidence data value is zero and it is held out using
# the data table :ref:`data_table@hold_out` equal to one.
# The other Sincidence data are
# the true value for incidence and have the data table hold_out
# equal to zero.
# There are four data points for each node, except the root node n0.
# The :ref:`hold_out_command-name` is used to randomly select a subset of these
# points to be held out using max_fit equal to two time the number of nodes.
#
# Model
# *****
# There is only one rate *iota* and it is constant in age and time.
# The corresponding model for the Sincidence data is *iota* .
# The corresponding mode for the prevalence data is
# 1 ``- exp`` ( *iota* * *age* ) .
#
# Fit
# ***
# Because the zero prevalence data and zero incidence data is held out,
# the fitting value for *iota* is very close to the true value
# for incidence.
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_hold_out_1.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# values used to simulate data
iota_true    = 0.01
n_child_node = 10
# ------------------------------------------------------------------------
import sys
import os
import copy
import math
test_program = 'example/user/hold_out_1.py'
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
   # ----------------------------------------------------------------------
   # age table:
   age_list    = [ 0.0, 5.0, 15.0, 35.0, 50.0, 75.0, 90.0, 100.0 ]
   #
   # time table:
   time_list   = [ 1990.0, 2000.0, 2010.0, 2200.0 ]
   #
   # integrand table:
   integrand_table = [
      { 'name':'Sincidence' },
      { 'name':'prevalence' }
   ]
   #
   # node table:
   node_table = [ { 'name':'n0', 'parent':'' } ]
   for node_id in range(1, n_child_node+1, 1) :
      node_table.append( { 'name':f'n{node_id}', 'parent':'n0' } )
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
   #
   # values that are the same for all data points
   row = {
      'density':     'gaussian',
      'meas_std':    iota_true / 10.,
      'weight':      '',
      'age_lower':    50.0,
      'age_upper':    50.0,
      'time_lower':   2000.,
      'time_upper':   2000.,
      'node':         'world',
      'subgroup':     'world',
   }
   #
   # prevalence data point with value 0.0 and held out
   # using option table hold_out command
   row['node']       = 'n0'
   row['integrand']  = 'prevalence'
   row['meas_value'] = 0.0
   row['hold_out']   = False;
   data_table.append( copy.copy(row) )
   #
   # Sincidence data point with value 0.0 and data_table hold_out 1
   row['node']       = 'n0'
   row['integrand']  = 'Sincidence'
   row['hold_out']   = True
   row['meas_value'] = 0.0
   data_table.append( copy.copy(row) )
   #
   # Sincidence data points with value iota_true and data_table hold_out 0
   # Note that every child node has four data points.
   row['integrand']  = 'Sincidence'
   row['hold_out']   = False
   row['meas_value'] = iota_true
   for node_id in range(1, n_child_node + 1, 1) :
      row['node'] = f'n{node_id}'
      for k in range(4) :
         data_table.append( copy.copy(row) )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      { # prior_iota
         'name':     'prior_iota',
         'density':  'uniform',
         'lower':    iota_true / 10.0,
         'upper':    iota_true * 10.0,
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
      { 'name':'parent_node_name',       'value':'n0'                  },
      { 'name':'warn_on_stderr',         'value':'false'               },

      { 'name':'quasi_fixed',            'value':'false'               },
      { 'name':'max_num_iter_fixed',     'value':'50'                  },
      { 'name':'print_level_fixed',      'value':'0'                   },
      { 'name':'tolerance_fixed',        'value':'1e-8'                },

      { 'name':'max_num_iter_random',    'value':'50'                  },
      { 'name':'print_level_random',     'value':'0'                   },
      { 'name':'tolerance_random',       'value':'1e-10'               },
   ]
   # hold out all prevalence data no matter what hold_out in data table
   option_table.append(
      { 'name':'hold_out_integrand',     'value':'prevalence'          }
   )
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
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
#
integrand = 'Sincidence'
max_fit   = 2 * n_child_node
dismod_at.system_command_prc(
   [ program, file_name, 'hold_out', integrand, str(max_fit) ]
)
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
# -----------------------------------------------------------------------
# read database
connection            = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
var_table             = dismod_at.get_table_dict(connection, 'var')
fit_var_table         = dismod_at.get_table_dict(connection, 'fit_var')
data_table            = dismod_at.get_table_dict(connection, 'data')
data_subset_table     = dismod_at.get_table_dict(connection, 'data_subset')
integrand_table       = dismod_at.get_table_dict(connection, 'integrand')
fit_data_subset_table = dismod_at.get_table_dict(connection, 'fit_data_subset')
connection.close()
#
# only one varable in this model, iota
assert len(var_table) == 1
assert len(fit_var_table) == 1
#
# all data points are in the data_sebset table, so data_subset_id is
# the same as data_id (see data subset table documentation).
assert len(data_subset_table) == len(data_table)
assert len(fit_data_subset_table) == len(data_table)
#
# check that max_fit Sincidence values are not held out
count_fit = 0
for subset_row in data_subset_table :
   data_id         = subset_row['data_id']
   data_row        = data_table[data_id]
   integrand_id    = data_row['integrand_id']
   integrand_name  = integrand_table[integrand_id]['integrand_name']
   if integrand_name == 'Sincidence' :
      hold_out    = data_row['hold_out'] == 1 or subset_row['hold_out'] == 1
      if not hold_out :
         count_fit += 1
assert count_fit == max_fit
#
# check fitted value for iota
iota_fit = fit_var_table[0]['fit_var_value']
assert abs( 1.0 - iota_fit / iota_true ) < 1e-6
#
# check residuals for non-zero data
for (subset_id, fit_row) in enumerate(fit_data_subset_table) :
   data_row          = data_table[data_id]
   meas_value        = data_row['meas_value']
   meas_std          = data_row['meas_std']
   weighted_residual = fit_row['weighted_residual']
   integrand_id      = data_row['integrand_id']
   integrand_name    = integrand_table[integrand_id]['integrand_name']
   if integrand_name == 'prevalence' :
      age      = data_row['age_lower']
      model    = 1.0 - math.exp( - iota_fit * age )
   else :
      model    = iota_fit

   check    = ( meas_value - model ) / meas_std
   assert( 1.0 - weighted_residual / check ) < 1e-6
#
# -----------------------------------------------------------------------------
print('hold_out_1.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
