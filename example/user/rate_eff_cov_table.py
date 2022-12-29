# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin user_rate_eff_cov_table.py}
# {xrst_comment_ch #}
#
# Example Using The Node Covariate Table
# ######################################
#
# Purpose
# *******
# This example demonstrates using the
# :ref:`rate_eff_cov_table-name` .
#
# True Value of Variables
# ***********************
# The values of the unknown variables that is used to
# simulate the data are
# {xrst_literal
#     BEGIN_TRUE_VALUE
#     END_TRUE_VALUE
# }
#
# Integrand
# *********
# There is only one integrand in this example,
# :ref:`avg_integrand@Integrand, I_i(a,t)@prevalence` .
#
# Node Tables
# ***********
# The node table for this example is
# ::
#
#                   world
#                  /     \
#     north_america       south_america
#
# Subgroup Table
# **************
# For this example there is only one subgroup (the world).
#
# Covariates
# **********
# There are two covariates in this example, *sex* and *normalized_income* .
# Sex is used as the :ref:`option_table@splitting_covariate` .
# Normalized income changes with age, node, and sex.
#
# Covariate Multipliers
# *********************
# There is one covariate multiplier in this example.
# It multiples *normalized_income* and effects the rate iota.
# Previous values for the covariate affect previous values for iota,
# which in turn affects the value of prevalence at the measurement time.
#
# Simulated Data
# **************
# The data is simulated using the true value for the variables,
# and the covariate effects mentioned above. No noise is added to the data,
# but it is modeled as having a ten percent coefficient of variation.
#
# Rate Variables
# **************
# There is one non-zero rate for this example iota
# and the no effect model for iota is constant and equal to
# ``iota_no_effect`` .
#
# Covariate Multipliers Variables
# *******************************
# There is one covariate multiplier for this example
# and it is constant and affects iota
# (but the value of the covariate, normalized age, is not constant).
#
# Source Code
# ***********
# {xrst_literal
#     BEGIN PYTHON
#     END PYTHON
# }
#
# {xrst_end user_rate_eff_cov_table.py}
# ---------------------------------------------------------------------------
# BEGIN PYTHON
# BEGIN_TRUE_VALUE
iota_no_effect     = 0.03
mulcov_true        = 0.9
def normalized_income(age, node_name, sex) :
   if node_name == 'north_america' and sex == 'male' :
      return age / 100.0
   return 0.1 * age / 100.0
def iota_true(age, time, node_name, sex) :
   effect  = mulcov_true * normalized_income(age, node_name, sex)
   return iota_no_effect * math.exp(effect)
# END_TRUE_VALUE
# ------------------------------------------------------------------------
import sys
import os
import copy
import math
test_program = 'example/user/rate_eff_cov_table.py'
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
   def fun_no_effect_iota(a, t) :
      return (iota_no_effect,  None, None)
   def fun_mulcov(a, t) :
      return ('mulcov_value_prior', None, None)
   def fun_weight_north_america_male(a, t) :
      return normalized_income(a, 'north_america', 'male')
   def fun_weight_other(a, t) :
      return normalized_income(a, '', '')
   # ----------------------------------------------------------------------
   # age table
   age_list    = [    0.0,  50.0,   100.0 ]
   #
   # time table
   time_list   = [ 1985.0, 2000.0, 2015.0 ]
   #
   # integrand table
   integrand_table = [ { 'name':'prevalence' } ]
   #
   # node table: world -> (north_america, south_america)
   node_table = [
      { 'name':'world',         'parent':'' },
      { 'name':'north_america', 'parent':'world' },
      { 'name':'south_america', 'parent':'world' },
   ]
   #
   # subgroup_table
   subgroup_table = [ { 'subgroup':'world', 'group':'world' } ]
   #
   # weight table:
   weight_table = [ {
      'name'    : 'normalized_income_north_america_male',
      'age_id'  : [0, 2],
      'time_id' : [0, 2],
      'fun'     : fun_weight_north_america_male,
      },{
      'name'    : 'normalized_income_other',
      'age_id'  : [0, 2],
      'time_id' : [0, 2],
      'fun'     : fun_weight_other,
   } ]
   #
   # covariate table:
   covariate_table = [
      {'name':'normalized_income', 'reference':0.0},
      {'name':'sex',               'reference':0.0},
   ]
   #
   # mulcov table
   # income has been scaled the same as sex so man use same smoothing
   mulcov_table = [
      {  # income effects north american incidence
         'covariate': 'normalized_income',
         'type':      'rate_value',
         'effected':  'iota',
         'group':     'world',
         'smooth':    'smooth_mulcov'
      }
   ]
   # ----------------------------------------------------------------------
   # sex_name2value
   sex_name2value = { 'female' : -0.5, 'both' : 0.0, 'male' : +0.5 }
   # ----------------------------------------------------------------------
   # data table:
   age  = 50.0
   time = 2000.0
   fun  = lambda age, time : iota_true(age, time, 'north_america', 'male')
   meas_value = dismod_at.average_integrand(
      rate           = { 'iota' : fun },
      integrand_name = 'prevalence',
      grid           =  { 'age': [age], 'time' : [time] },
      abs_tol        = 1e-5,
   )
   meas_std = meas_value / 10.0
   row = {
      'node':              'north_america',
      'subgroup':          'world',
      'density':           'gaussian',
      'weight':            '',
      'hold_out':           False,
      'time_lower':         time,
      'time_upper':         time,
      'age_lower':          age,
      'age_upper':          age,
      'normalized_income':  normalized_income(age, 'north_america', 'male'),
      'sex':                sex_name2value['male'],
      'integrand':          'prevalence',
      'meas_value':         meas_value,
      'meas_std':           meas_std,
      'density':            'gaussian',
   }
   data_table = [ row ]
   #
   # ----------------------------------------------------------------------
   # prior_table
   prior_table = [
      {  # mulcov_value_prior
         'name':     'mulcov_value_prior',
         'density':  'uniform',
         'mean':     0.0,
         'lower':    -2.0,
         'upper':    +2.0,
      }
   ]
   # ----------------------------------------------------------------------
   # smooth table
   smooth_table = [
      { # smooth_iota
         'name':                     'smooth_iota',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0 ],
         'fun':                      fun_no_effect_iota,
      },{ # smooth_mulcov
         'name':                     'smooth_mulcov',
         'age_id':                   [ 0 ],
         'time_id':                  [ 0 ],
         'fun':                      fun_mulcov
      }
   ]
   # ----------------------------------------------------------------------
   # rate table
   rate_table = [ {
         'name':          'iota',
         'parent_smooth': 'smooth_iota',
         'child_smooth':  None,
   } ]
   # ----------------------------------------------------------------------
   # option_table
   option_table = [
      { 'name':'splitting_covariate',    'value':'sex'          },
      #
      { 'name':'parent_node_name',       'value':'world'        },
      { 'name':'ode_step_size',          'value':'5.0'          },
      { 'name':'random_seed',            'value':'0'            },
      { 'name':'rate_case',              'value':'iota_pos_rho_zero' },
      #
      { 'name':'quasi_fixed',            'value':'true'         },
      { 'name':'derivative_test_fixed',  'value':'first-order'  },
      { 'name':'max_num_iter_fixed',     'value':'100'          },
      { 'name':'print_level_fixed',      'value':'0'            },
      { 'name':'tolerance_fixed',        'value':'1e-8'         },
      #
      { 'name':'derivative_test_random', 'value':'second-order' },
      { 'name':'max_num_iter_random',    'value':'100'          },
      { 'name':'print_level_random',     'value':'0'            },
      { 'name':'tolerance_random',       'value':'1e-10'        },
   ]
   # ----------------------------------------------------------------------
   rate_eff_cov_table = list()
   not_used       = 0.0
   for node_id in range( len( node_table ) ) :
      for sex_name in sex_name2value :
         node_name      = node_table[node_id]['name']
         split_value    = sex_name2value[sex_name]
         if node_name == 'north_america' and sex_name == 'male' :
            weight_name = 'normalized_income_north_america_male'
         else :
            weight_name = 'normalized_income_other'
         row = {
            'node_name'      : node_name,
            'covariate_name' : 'normalized_income',
            'split_value'    : split_value,
            'weight_name'    : weight_name,
         }
         rate_eff_cov_table.append( row )
   # ----------------------------------------------------------------------
   # create database
   dismod_at.create_database(
      file_name       = file_name        ,
      age_list        = age_list         ,
      time_list       = time_list        ,
      integrand_table = integrand_table  ,
      node_table      = node_table       ,
      subgroup_table  = subgroup_table   ,
      weight_table    = weight_table     ,
      covariate_table = covariate_table  ,
      data_table      = data_table       ,
      prior_table     = prior_table      ,
      smooth_table    = smooth_table     ,
      rate_table      = rate_table       ,
      mulcov_table    = mulcov_table     ,
      option_table    = option_table     ,
      rate_eff_cov_table  = rate_eff_cov_table   ,
   )
# ===========================================================================
file_name = 'example.db'
example_db(file_name)
#
program = '../../devel/dismod_at'
dismod_at.system_command_prc([ program, file_name, 'init' ])
dismod_at.system_command_prc([ program, file_name, 'fit', 'fixed' ])
dismod_at.db2csv_command( file_name )
#
# connect to database
new             = False
connection      = dismod_at.create_connection(file_name, new)
var_table       = dismod_at.get_table_dict(connection, 'var')
fit_var_table   = dismod_at.get_table_dict(connection, 'fit_var')
#
assert len(var_table) == 2
for (var_id, row_var) in enumerate(var_table) :
   row_fit     = fit_var_table[var_id]
   var_type    = row_var['var_type']
   fit_value   = row_fit['fit_var_value']
   if var_type == 'rate' :
      assert fit_value == iota_no_effect
   else :
      assert var_type == 'mulcov_rate_value'
      rel_error = fit_value / mulcov_true - 1.0
      if abs(rel_error) > 1e-3 :
         print('rel_error = ' , rel_error)
         assert False
#
# -----------------------------------------------------------------------------
print('rate_eff_cov_table.py: OK')
# -----------------------------------------------------------------------------
# END PYTHON
