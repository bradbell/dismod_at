# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-24 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_hold_out_2.py}
# {xrst_comment_ch #}
#
# hold_out Command: Balancing Sex Covariate Values
# ################################################
#
# Purpose
# *******
# This example shows how to balance
# :ref:`hold_out_command@Balancing@Covariates`
# using the hold_out command.
#
# Integrands
# **********
# For this example there is only one integrand, ``Sincidence`` .
#
# Nodes
# *****
# There are Three nodes, ``europe`` , ``germany`` and ``italy`` .
#
# random_seed
# ***********
# If this is zero, the clock is used to seed the random number generator:
# {xrst_code py}
random_seed = 1234
# {xrst_code}
#
# alpha_true
# **********
# True value of the covariate multiplier used to simulate data:
# {xrst_spell_off}
# {xrst_code py}
alpha_true  = 0.3
# {xrst_code}
# {xrst_spell_on}
# This multiplies the sex covariate and affects *iota* .
#
# iota_avg
# ********
# Average value of *iota* .
# {xrst_spell_off}
# {xrst_code py}
iota_avg    = 0.01
# {xrst_code}
# {xrst_spell_on}
#
# True Iota
# *********
# True value for iota used to simulate the data:
# {xrst_spell_off}
# {xrst_code py}
import math
def iota_true(node, sex) :
   effect  = alpha_true * sex
   if node == 'germany' :
      iota    = 1.2 * iota_avg  * math.exp( effect )
   elif node == 'italy' :
      iota    = 0.8 * iota_avg * math.exp( effect )
   return iota
# {xrst_code}
# {xrst_spell_on}
#
# Parent Node
# ***********
# For this example the parent node is europe and we are only fitting
# fixed effects, so the variation between germany and italy is noise in
# the model.
#
# Data
# ****
# For each node, the data comes in pairs with both sexes represented equally.
# The problem with the data is that we are only fitting fixed effects.
# Thus the variation in the node for each data point is a confounding
# covariate when we sub-sample without balancing the sex covariate.
#
# Model
# *****
# There is only one rate *iota* and it is constant as a function
# of age and time. In addition, there is one covariate multiplier
# for income.
#
# fitting
# *******
# For this example we are only fitting the fixed effects,
# so the variation between germany and italy is a confounding covariate.
#
# hold_out
# ********
# This example uses the version of the hold_out command that includes
# :ref:`balancing covariates<hold_out_command@Balancing@Covariates>` .
# Note that balancing the sex covariate leads to much more accurate
# estimates of the covariate multiplier *alpha* .
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_hold_out_2.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# ------------------------------------------------------------------------
import sys
import os
import csv
import copy
import math
test_program = 'example/user/hold_out_2.py'
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
   def fun_alpha(a, t) :
      return ('prior_alpha', None, None)
   # ----------------------------------------------------------------------
   # age table:
   age_list    = [ 0.0, 100.0 ]
   #
   # time table:
   time_list   = [ 1990.0, 2200.0 ]
   #
   # integrand table:
   integrand_table = [
       { 'name':'Sincidence' }
   ]
   #
   # node table:
   node_table = [
      { 'name':'europe',  'parent':'' },
      { 'name':'germany', 'parent':'europe' },
      { 'name':'italy',   'parent':'europe' },
   ]
   #
   # weight table:
   weight_table = list()
   #
   # covariate table:
   covariate_table = [
      {  'name':           'sex',
         'reference':      0.0,
         'max_difference': 0.6,
      }
   ]
   #
   # mulcov table:
   # use weight_ for covariate name to avoid confusion with other weight
   # in data table (this is a problem with create_database).
   mulcov_table = [ {
         'covariate': 'sex',
         'type':      'rate_value',
         'effected':  'iota',
         'smooth':    'smooth_alpha',
         'group':     'world'
   } ]
   #
   # avgint table: empty
   avgint_table = list()
   #
   # nslist_dict:
   nslist_dict = dict()
   # ----------------------------------------------------------------------
   # data table:
   data_table = list()
   row = {
      'integrand':   'Sincidence',
      'hold_out':    False,
      'density':     'gaussian',
      'weight':      '',
      'age_lower':    50.0,
      'age_upper':    50.0,
      'time_lower':   2000.,
      'time_upper':   2000.,
      'subgroup':     'world',
      'meas_std':    iota_avg / 10.0,
   }
   n_repeat = 10
   for i in range(n_repeat) :
      # sample twice as often from germany so that original data not balenced
      for node in [ "germany", "germany", "italy" ] :
         for sex in [ -0.5, +0.5 ] :
            meas_value = iota_true(node, sex)
            row['sex']        = sex
            row['node']       = node
            row['meas_value'] = meas_value
            data_table.append( copy.copy(row) )
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      {  # prior_iota
         'name':     'prior_iota',
         'density':  'uniform',
         'lower':    iota_avg / 10.0,
         'upper':    iota_avg * 10.0,
         'mean':     iota_avg * 2.0,
      },{ # prior_alpha
         'name':     'prior_alpha',
         'density':  'uniform',
         'lower':    - abs(alpha_true) * 10.0,
         'upper':    + abs(alpha_true) * 10.0,
         'mean':     0.0,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   smooth_table = [
      {  # smooth_iota
         'name':      'smooth_iota',
         'age_id':    [0],
         'time_id':   [0],
         'fun':      fun_iota
      },{  # smooth_alpha
         'name':      'smooth_alpha',
         'age_id':    [0],
         'time_id':   [0],
         'fun':      fun_alpha
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
      { 'name':'parent_node_name',       'value':'europe'              },
      { 'name':'random_seed',            'value':str(random_seed)      },

      { 'name':'quasi_fixed',            'value':'false'               },
      { 'name':'max_num_iter_fixed',     'value':'50'                  },
      { 'name':'print_level_fixed',      'value':'0'                   },
      { 'name':'tolerance_fixed',        'value':'1e-9'                },

      { 'name':'max_num_iter_random',    'value':'50'                  },
      { 'name':'print_level_random',     'value':'0'                   },
      { 'name':'tolerance_random',       'value':'1e-10'               },
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
integrand_name = 'Sincidence'
max_fit        = '10'
cov_name       = 'sex'
cov_value_1    = '-0.5'
cov_value_2    = '+0.5'
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
command = [ program, file_name, 'hold_out', integrand_name, max_fit ]
if True :
   # If you change True above to False, you will do not balance the
   # sex covariter and the rel_error test for alpha will fail
   command += [ cov_name, cov_value_1, cov_value_2 ]
dismod_at.system_command_prc(command)
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
# -----------------------------------------------------------------------
#
# var_table, fit_var_table
connection            = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
var_table             = dismod_at.get_table_dict(connection, 'var')
fit_var_table         = dismod_at.get_table_dict(connection, 'fit_var')
connection.close()
#
# check var_table and fit_var_table
assert len(var_table) == 2
assert len(fit_var_table) == 2
for var_id in range( len(var_table) ) :
   var_type       = var_table[var_id]['var_type']
   fit_var_value  = fit_var_table[var_id]['fit_var_value']
   if var_type == 'mulcov_rate_value' :
      # relative error for alpha
      rel_error  = 1.0 - fit_var_value / alpha_true
      assert abs(rel_error) < 1e-5
   else :
      # relative error for iota
      assert var_type == 'rate'
      rel_error = 1.0 - fit_var_value / iota_avg
      assert abs(rel_error) < 0.1
#
# data_table, data_subset_table, integrand_table
connection            = dismod_at.create_connection(
   file_name, new = False, readonly = True
)
data_table            = dismod_at.get_table_dict(connection, 'data')
data_subset_table     = dismod_at.get_table_dict(connection, 'data_subset')
integrand_table       = dismod_at.get_table_dict(connection, 'integrand')
connection.close()
#
# hold_in_count
hold_in_count = 0
for subset_row in data_subset_table :
   data_id        = subset_row['data_id']
   data_row       = data_table[data_id]
   integrand_id   = data_row['integrand_id']
   integrand_name = integrand_table[integrand_id]['integrand_name']
   assert integrand_name == 'Sincidence'
   if data_row['hold_out'] == 0 and subset_row['hold_out'] == 0 :
      hold_in_count += 1
assert hold_in_count == int(max_fit)
#
# -----------------------------------------------------------------------------
print('hold_out_2.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
