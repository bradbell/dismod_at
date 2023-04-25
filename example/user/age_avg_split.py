# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_age_avg_split.py}
# {xrst_comment_ch #}
#
# Non-uniform Age Average Grid
# ############################
#
# Purpose
# *******
# This example demonstrate one reason for using the
# :ref:`option_table@Age Average Grid@age_avg_split` option to create an
# :ref:`age_avg_table@Age Average Grid`
# that is not uniformly spaced.
#
# Variables
# *********
# For this case there is only one rate *omega* for the parent node
# and there are no other :ref:`model_variables-name` .
#
# Prior
# *****
# The prior for *omega* is mean ``0.1`` and standard deviation
# ``.01`` for ages ``0.0`` and ``0.9`` .
# The prior for *omega* is mean ``0.01`` and standard deviation
# ``0.001`` for ages ``1.1`` and ``100.0`` .
#
# Data
# ****
# There is no data for this model; i.e., the prior is the only
# information available.
#
# Fit
# ***
# A fit is done, and this should make :ref:`fit_var_table-name`
# equal to the mean of the prior.
#
# Predict
# *******
# A predict is done for other cause mortality for two cases.
# The first case averages over the age interval ``[ 0.0, 0.9 ]`` .
# The second case averages over the age interval ``[ 1.1, 100.0 ]`` .
#
# ode_step_size
# *************
# The :ref:`option_table@Age Average Grid@ode_step_size` is
# ``50.0`` for this example.
# If there were no *age_avg_split* , a linear approximation
# would be used from age 0.0 to age 50.
#
# age_avg_split
# *************
# The age average grid is split at age ``1.0`` ; i.e.,
# The age average grid points are ``0.0`` , ``1.0`` , ``50.0`` , and
# ``100.0`` .
# A piecewise linear function of age is used between these grid points.
# (Note everything is constant w.r.t time for this case.)
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_age_avg_split.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# true values used to simulate data
omega_0_1    = 1e-1
omega_1_100  = 1e-2
# ------------------------------------------------------------------------
import sys
import os
import copy
test_program = 'example/user/age_avg_split.py'
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
# Note that the a, t values are used for this example
def example_db (file_name) :
   #
   def fun_omega_parent(a, t) :
      if  a <= 1.0 :
         return ('prior_0_1', 'prior_none', 'prior_none')
      else :
         return ('prior_1_100', 'prior_none', 'prior_none')
   #
   def omega_true(a) :
      if a < 1.0 :
         return omega_0_1
      else :
         return omega_1_100
   #
   # ----------------------------------------------------------------------
   # age table
   age_list    = [ 0.0, 0.9, 1.1, 100.0 ]
   #
   # time table
   time_list   = [ 1995.0, 2015.0 ]
   #
   # integrand table
   integrand_table = [
      { 'name':'mtother' }
   ]
   #
   # node table: world
   node_table = [ { 'name':'world',         'parent':'' } ]
   #
   # weight table:
   weight_table = list()
   #
   # covariate table:
   covariate_table = list()
   #
   # mulcov table
   mulcov_table = list()
   #
   # avgint table
   avgint_table = list()
   row = {
      'integrand':  'mtother',
      'node':       'world',
      'subgroup':   'world',
      'weight':     '',
   }
   row['age_lower'] = 0.0
   row['age_upper'] = 0.9
   row['time_lower'] = 2000.0
   row['time_upper'] = 2000.0
   avgint_table.append( copy.copy(row) )
   row['age_lower'] = 1.1
   row['age_upper'] = 100.0
   row['time_lower'] = 2000.0
   row['time_upper'] = 2000.0
   avgint_table.append( copy.copy(row) )
   #
   # nslist_dict:
   nslist_dict = dict()
   #
   # data table:
   data_table = list()
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      {  # prior_none
         'name':     'prior_none',
         'density':  'uniform',
         'mean':     0.0,
      },{ # prior_0_1
         'name':     'prior_0_1',
         'density':  'gaussian',
         'mean':     omega_0_1,
         'std':      1e-1 * omega_0_1
      },{ # prior_1_100
         'name':     'prior_1_100',
         'density':  'gaussian',
         'mean':     omega_1_100,
         'std':      1e-1 * omega_1_100
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   #
   smooth_table = [
      { # smooth_omega_parent
         'name':                     'smooth_omega_parent',
         'age_id':                   [0, 1, 2, 3],
         'time_id':                  [0, 1],
         'fun':                      fun_omega_parent
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [
      {
         'name':          'omega',
         'parent_smooth': 'smooth_omega_parent'
      }
   ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'parent_node_name',       'value':'world'               },
      { 'name':'ode_step_size',          'value':'50.0'                },
      { 'name':'age_avg_split',          'value':'1.0'                 },
      { 'name':'random_seed',            'value':'0'                   },
      { 'name':'rate_case',              'value':'iota_zero_rho_zero'  }
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
file_name = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init'] )
dismod_at.system_command_prc([ program, file_name, 'fit', 'both'] )
dismod_at.system_command_prc([ program, file_name, 'predict', 'fit_var'] )
# -----------------------------------------------------------------------
# connect to database
connection      = dismod_at.create_connection(
   file_name, new = False, readonly = False
)
# -----------------------------------------------------------------------
# Results for fitting with no noise
var_table     = dismod_at.get_table_dict(connection, 'var')
predict_table = dismod_at.get_table_dict(connection, 'predict')
#
parent_node_id = 1
eps            = 1e-4
#
# check rates values
tolerance         = 1e-10
for row in predict_table :
   avgint_id = row['avgint_id']
   if avgint_id == 0 :
      value_true = omega_0_1
   else :
      value_true = omega_1_100
   value      = row['avg_integrand']
   assert(abs(1.0 - value / value_true) < tolerance)
# -----------------------------------------------------------------------------
print('age_avg_split.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
